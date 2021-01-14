#include <exception>
#include <iostream>
#include <string>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/bind.hpp>
#include <boost/cstdlib.hpp>
#include <boost/make_shared.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/system/detail/error_code.hpp>

#include "quotes/server.h"
#include "quotes/utils.h"

quotes::Setup parseArgs(int argc, char * argv[]) {
    namespace po = boost::program_options;
    po::options_description description{"Allowed options"};

    // clang-format off
    description.add_options()
        ("help,h", "show help message")
        ("ip,i", po::value<std::string>(), "specify ip address (default 127.0.0.1)")
        ("threads,t", po::value<size_t>(), "number of threads (default 4)")
        ("port,p", po::value<int>(), "specify port number (default 1100)")
        ("data,d", po::value<std::string>(), "path to file with saved data (default \"assets.data\")")
        ("no-data,n", "suppress loading/storing data");
    // clang-format on

    po::variables_map map;
    po::store(po::parse_command_line(argc, argv, description), map);
    po::notify(map);

    if (map.count("help")) {
        quotes::log(description);
        exit(boost::exit_success);
    }

    quotes::Setup setup;
    setup.threads = 4;
    std::string ip = "127.0.0.1";
    int port = 1100;

    if (map.count("port"))
        port = map["port"].as<decltype(port)>();

    if (map.count("ip"))
        ip = map["ip"].as<decltype(ip)>();

    if (map.count("threads"))
        setup.threads = map["threads"].as<size_t>();

    if (!map.count("no-data")) {
        setup.dataPath = "assets.data";
        if (map.count("data"))
            setup.dataPath = map["data"].as<decltype(setup.dataPath)>();
    }

    boost::asio::ip::tcp::endpoint endpoint;
    endpoint.address(boost::asio::ip::make_address(ip));
    endpoint.port(port);

    setup.endpoint = endpoint;
    return setup;
}

int main(int argc, char * argv[]) {
    try {
        auto setup = parseArgs(argc, argv);
        setup.service = boost::make_shared<boost::asio::io_service>();
        auto server = quotes::Server::create(setup);

        quotes::log("Server address:", setup.endpoint);

        boost::asio::signal_set signals(*setup.service, SIGINT, SIGTERM);
        signals.async_wait(
            [&setup](const boost::system::error_code & error, int) {
                if (!error)
                    setup.service->stop();
                quotes::log();
            });

        boost::asio::thread_pool pool{setup.threads};
        boost::asio::post(
            pool, boost::bind(&boost::asio::io_service::run, setup.service));
        pool.join();

        return boost::exit_success;
    } catch (std::exception & e) {
        std::cerr << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unrecognized error" << std::endl;
    }
    return boost::exit_failure;
}

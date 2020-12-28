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
#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/system/detail/error_code.hpp>

#include "quotes/server.h"

boost::asio::ip::tcp::endpoint parseArgs(int argc, char * argv[]) {
    namespace po = boost::program_options;
    po::options_description description{"Allowed options"};

    // clang-format off
        description.add_options()
            ("help,h", "show help message")
            ("ip,i", po::value<std::string>(), "specify ip address (default 127.0.0.1)")
            ("port,p", po::value<int>(), "specify port number (default 1100)");
    // clang-format on

    po::variables_map map;
    po::store(po::parse_command_line(argc, argv, description), map);
    po::notify(map);

    if (map.count("help")) {
        std::cout << description << std::endl;
        exit(boost::exit_success);
    }

    std::string ip = "127.0.0.1";
    int port = 1100;

    if (map.count("port"))
        port = map["port"].as<decltype(port)>();

    if (map.count("ip"))
        ip = map["ip"].as<decltype(ip)>();

    boost::asio::ip::tcp::endpoint endpoint;
    endpoint.address(boost::asio::ip::make_address(ip));
    endpoint.port(port);
    return endpoint;
}

int main(int argc, char * argv[]) {
    try {
        auto service = boost::make_shared<boost::asio::io_service>();
        auto endpoint = parseArgs(argc, argv);
        auto server = quotes::Server::create(service, endpoint);

        std::cout << "Server address: " << endpoint << std::endl;

        boost::asio::signal_set signals(*service, SIGINT, SIGTERM);
        signals.async_wait(
            [&service](const boost::system::error_code & error, int) {
                if (!error)
                    service->stop();
            });

        boost::asio::thread_pool pool{4};
        boost::asio::post(
            pool, boost::bind(&boost::asio::io_service::run, service));
        pool.join();

        return boost::exit_success;

    } catch (std::exception & e) {
        std::cerr << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unrecognized error" << std::endl;
    }
    return boost::exit_failure;
}

#include <boost/asio/io_service.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/system/detail/error_code.hpp>

#include "quotes/server.h"

int main(int argc, char * argv[]) {
    auto service = boost::make_shared<boost::asio::io_service>();
    auto server = quotes::Server::create(service);

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

    return 0;
}

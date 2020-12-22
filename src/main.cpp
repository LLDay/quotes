#include "quotes/server.h"

#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/make_shared.hpp>
#include <boost/memory_order.hpp>
#include <boost/system/detail/error_code.hpp>

int main(int argc, char * argv[]) {
    auto service = boost::make_shared<boost::asio::io_service>();
    quotes::Server server{service};

    boost::asio::signal_set signals(*service, SIGINT, SIGTERM);
    signals.async_wait(
        [&service](const boost::system::error_code & error, int) {
            if (!error)
                service->stop();
        });

    service->run();
    return 0;
}

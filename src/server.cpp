#include "server.hpp"
#include "dispatcher.hpp"
#include "service_handler.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <memory>

namespace net = boost::asio;                // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

net::io_context ioc;
net::thread_pool tp(5);
tcp::endpoint ep(tcp::v4(), 8080);

int main(){
    auto dispatcher = std::make_shared<Dispatcher<ExampleServiceHandler>>(tp, ioc, ep);
    std::cout << "Listening on port " << ep.port() << std::endl;
    dispatcher->do_accept();
}

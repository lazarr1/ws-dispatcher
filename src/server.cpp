#include "server.hpp"
#include "dispatcher.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/beast/core/make_printable.hpp>
#include <boost/beast/http/parser.hpp>
#include <iostream>
#include <boost/beast/core.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <iostream>
#include <memory>

namespace beast = boost::beast;             // from <boost/beast.hpp>
namespace http = beast::http;               // from <boost/beast/http.hpp>
namespace websocket = beast::websocket;     // from <boost/beast/websocket.hpp>
namespace net = boost::asio;                // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;           // from <boost/asio/ip/tcp.hpp>



void accept_handler(const boost::system::error_code& error)
{
  std::cout << "got a message" << error.message() << std::endl;
  if (!error)
  {
    std::cout << "socket accepted w status: " << error.message() << std::endl;

    std:: cout << error.to_string() << std::endl;

  }
}

net::io_context ioc;

void do_accept(tcp::acceptor& acceptor){
    boost::system::error_code ec;
    tcp::socket Socket(ioc);

    // acceptor.accept(Socket);

    // beast::flat_buffer buffer;
    // http::request<http::string_body> req;
    // http::read(Socket, buffer, req);

    // std::cout << "data:: " << beast::make_printable(buffer.data()) << std::endl;

    acceptor.async_accept(Socket, 
    [& acceptor](const boost::system::error_code& error){
        if (!error) {
            beast::flat_buffer buffer;
            http::request<http::string_body> req;

            std::cout << "hello from in here!" << std::endl;
        }
        do_accept(acceptor);
    });
}

int main(){

    net::thread_pool tp(5);
    tcp::endpoint ep(tcp::v4(), 8080);
    std::shared_ptr<Dispatcher> dispatcher = std::make_shared<Dispatcher>(tp, ioc, ep);
    dispatcher->do_accept();

}

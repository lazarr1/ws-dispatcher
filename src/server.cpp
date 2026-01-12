#include "server.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/beast/core/make_printable.hpp>
#include <boost/beast/http/parser.hpp>
#include <iostream>
#include <boost/beast/core.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <iostream>

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

void do_accept(){
    net::io_context ioc;
    boost::system::error_code ec;
    tcp::socket Socket(ioc);

    ioc.run();
    tcp::acceptor acceptor(ioc, {tcp::v4(), 8080});

    std::cout << "Listening on port " << acceptor.local_endpoint().port() << std::endl;

    acceptor.accept(Socket);

    beast::flat_buffer buffer;
    http::request<http::string_body> req;
    http::read(Socket, buffer, req);


    std::cout << "data:: " << beast::make_printable(buffer.data()) << std::endl;

    acceptor.async_accept(Socket, 
    [&Socket](const boost::system::error_code& error){
        if (!error) {
            beast::flat_buffer buffer;
            http::request<http::string_body> req;

            std::cout << "hello from in here!" << std::endl;
        }

    });

}

int main(){


    do_accept();


    // while (true) {
    //     try {
    //         tcp::socket socket(ioc);
    //         acceptor.accept(socket);

    //         beast::flat_buffer buffer;
    //             http::request<http::string_body> req;
            
    //         auto test = socket.read_some(10);

    //         std::cout << "test: " << test << std::endl;
    //         std::cout << "data: " << beast::make_printable(buffer.data()) << std::endl;


    //         // acceptor.async_accept(socket, accept_handler);
    //         ioc.run();
    //     } catch (const std::exception& e){
    //         std::cout << e.what() << std::endl;
    //     }
    // }
}

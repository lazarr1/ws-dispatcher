#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/beast/core/make_printable.hpp>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/thread_pool.hpp>
#include <iostream>


class IServiceHandler {
    public:
        virtual void acceptSession() = 0;

};

namespace beast = boost::beast;             // from <boost/beast.hpp>
namespace http = beast::http;               // from <boost/beast/http.hpp>
namespace websocket = beast::websocket;     // from <boost/beast/websocket.hpp>
namespace net = boost::asio;                // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

class Dispatcher {
    public:
        Dispatcher();


    private:

        tcp::acceptor acceptor_;
        tcp::socket socket_;

        net::thread_pool& tp;
        net::io_context& ioc_;

};
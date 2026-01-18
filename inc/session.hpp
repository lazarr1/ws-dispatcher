#pragma once

#include <memory>

#include <boost/beast/core/make_printable.hpp>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/websocket.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/thread_pool.hpp>

namespace beast = boost::beast;             // from <boost/beast.hpp>
namespace http = beast::http;               // from <boost/beast/http.hpp>
namespace websocket = beast::websocket;     // from <boost/beast/websocket.hpp>

namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class Session: public std::enable_shared_from_this<Session>{
public:
    Session(tcp::socket socket, net::thread_pool &tp, net::io_context& ioc);


private:
    websocket::stream<tcp::socket> ws_;
    net::io_context& ioc;
    net::thread_pool& workers_;

};
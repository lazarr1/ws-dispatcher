#pragma once

#include <atomic>
#include <deque>
#include <memory>

#include <boost/beast/core/make_printable.hpp>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/websocket.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/thread_pool.hpp>
#include <mutex>

#include "service_handler.hpp"

namespace beast = boost::beast;             // from <boost/beast.hpp>
namespace http = beast::http;               // from <boost/beast/http.hpp>
namespace websocket = beast::websocket;     // from <boost/beast/websocket.hpp>

namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class Session: public std::enable_shared_from_this<Session>{
public:
    Session(tcp::socket& socket, net::thread_pool &tp, net::io_context& ioc, std::unique_ptr<IServiceHandler> sh);
    ~Session();

    void start();
    void do_read();

    void do_write();


private:
    websocket::stream<tcp::socket> ws_;
    net::io_context& ioc_;
    net::thread_pool& workers_;
    std::unique_ptr<IServiceHandler> sh_;

    http::request<http::string_body> req_;
    beast::flat_buffer buffer_;

    std::mutex dequeMutex_;
    std::deque<std::string> messageQueue_;

    bool writeInProgress_;

};
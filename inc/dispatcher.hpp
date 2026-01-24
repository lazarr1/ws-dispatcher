#pragma once

#include "session.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/thread_pool.hpp>

#include <memory>

namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class Dispatcher: public std::enable_shared_from_this<Dispatcher>{
    public:
        Dispatcher(net::thread_pool& tp, net::io_context& ioc, tcp::endpoint& endpoint);
        ~Dispatcher();

        void do_accept();

    private:

        tcp::socket socket_;
        tcp::acceptor acceptor_;

        net::thread_pool& tp_;
        net::io_context& ioc_;

};
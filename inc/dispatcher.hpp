#pragma once

#include "session.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/thread_pool.hpp>

#include <memory>

namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

template <typename T>
concept ServiceHandler =
    std::derived_from<T, IServiceHandler>;

template <ServiceHandler T>
class Dispatcher: public std::enable_shared_from_this<Dispatcher<T>>{
public:

    Dispatcher(net::thread_pool &tp, net::io_context& ioc, tcp::endpoint& endpoint)
    : socket_(ioc), acceptor_(ioc, endpoint), tp_(tp), ioc_(ioc)
    {
    }

    void do_accept(){
        auto weakSelf = this->weak_from_this();

        acceptor_.async_accept(socket_, 
            [weakSelf](const boost::system::error_code& error) {
                auto self = weakSelf.lock();
                if(self){
                    if (!error) {
                        std::make_shared<Session>(self->socket_, self->tp_, self->ioc_, std::make_unique<T>())->start();
                        self->socket_ = tcp::socket(self->ioc_);
                    } else {
                        std::cout << "Failed to accept at dispatcher level, with error: " << error << std::endl;
                    }

                    self->do_accept();
                } 
            }
        );


        ioc_.run();
    }

private:

    tcp::socket socket_;
    tcp::acceptor acceptor_;

    net::thread_pool& tp_;
    net::io_context& ioc_;
};
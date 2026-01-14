#include "dispatcher.hpp"
#include <iostream>
#include <utility>

Dispatcher::Dispatcher(net::thread_pool &tp, net::io_context& ioc, tcp::endpoint endpoint)
: socket_(ioc), acceptor_(ioc, endpoint), tp_(tp), ioc_(ioc)
{
}

Dispatcher::~Dispatcher(){
    std::cout << "deinitialising" << std::endl;
}


void Dispatcher::do_accept(){
    auto weakSelf = this->weak_from_this();

    acceptor_.async_accept(socket_, 
        [&weakSelf](const boost::system::error_code& error) {
            auto self = weakSelf.lock();
            if(self){
                std::cout << error.message() << std::endl;
                if (!error) {
                    std::cout << "move socket and stuff to worker thread" << std::endl;
                    // net::post(std::move(self->socket_));
                    self->socket_.close();
                }

                self->do_accept();
            } else {
                std::cout << "already deinitialised" << std::endl;
            }
        }
    );


    ioc_.run();
}
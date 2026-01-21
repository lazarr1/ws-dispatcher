#include "dispatcher.hpp"
#include "session.hpp"
#include "service_handler.hpp"

#include <boost/system/detail/error_code.hpp>
#include <iostream>


Dispatcher::Dispatcher(net::thread_pool &tp, net::io_context& ioc, tcp::endpoint& endpoint)
: socket_(ioc), acceptor_(ioc, endpoint), tp_(tp), ioc_(ioc)
{
}

Dispatcher::~Dispatcher(){
    std::cout << "deinitialising" << std::endl;
}


void Dispatcher::do_accept(){
    auto weakSelf = this->weak_from_this();
    std::cout << "Listening on port " << this->acceptor_.local_endpoint() << std::endl;

    acceptor_.async_accept(socket_, 
        [weakSelf](const boost::system::error_code& error) {
            auto self = weakSelf.lock();
            if(self){
                std::cout << error.message() << std::endl;
                if (!error) {
                    std::cout << "move socket and stuff to worker thread" << std::endl;

                    std::make_shared<Session>(self->socket_, self->tp_, self->ioc_, std::make_unique<ExampleServiceHandler>())->start();
                    self->socket_ = tcp::socket(self->ioc_);
                }

                self->do_accept();
            } else {
                std::cout << "already deinitialised" << std::endl;
            }
        }
    );


    ioc_.run();
}
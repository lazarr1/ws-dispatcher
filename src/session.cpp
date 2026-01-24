#include "session.hpp"
#include "service_handler.hpp"
#include <boost/asio/buffer.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/websocket/rfc6455.hpp>

Session::Session(tcp::socket& socket, net::thread_pool &tp, net::io_context& ioc, std::unique_ptr<IServiceHandler> sh)
 : ws_(std::move(socket)), ioc_(ioc), workers_(tp), sh_(std::move(sh))
{

}

Session::~Session(){
    std::cout << "Session killed" << std::endl;
}


void Session::start(){
    auto self = shared_from_this();

    http::async_read(ws_.next_layer(), buffer_, this->req_,
    [self] (beast::error_code error, size_t msgSize) {
        // std::shared_ptr<Session> self = weakSelf.lock();
        if (error) {
            std::cout << "MSG SIZE: " << msgSize << std::endl;
            // std::cout << beast::make_printable(self->buffer_.data()) << std::endl;
            std::cout << self->req_ << std::endl;
            std::cout << "Http read error: " << error << std::endl;
            return;
        }

        if (!websocket::is_upgrade(self->req_)) {
            std::cout << "Not an upgrade request" << std::endl;
            return;
        }

        if (self) {
            // Accept the ws handshale/ upgrade
            self->ws_.async_accept(self->req_, 
            [self](beast::error_code ec) {
                if (ec) {
                    std::cout << "accept error: " << ec << std::endl;
                }

                self->do_read();
            });
        }
    }
    );
}

void Session::do_read() {
    std::shared_ptr<Session> self = shared_from_this();

    ws_.async_read(buffer_,
        [self](beast::error_code ec, std::size_t) {
            if (ec) {
                if (ec == websocket::error::closed) return;
                std::cerr << "Read error: " << ec.message() << std::endl;
                return;
            }

            std::string message = beast::buffers_to_string(self->buffer_.data());
            self->buffer_.consume(self->buffer_.size());

            // Offload message processing to the worker pool (5 threads)
            net::post(self->workers_,
                [self, message = std::move(message)]() mutable {
                    // Each session has its own Router instance (no shared state)
                    auto sr = self->sh_->onMessage(message);

                    {
                        std::lock_guard lock(self->dequeMutex_);
                        self->messageQueue_.insert(self->messageQueue_.end(), sr.outgoing_msgs.begin(), sr.outgoing_msgs.end());
                    }

                    net::post(self->ioc_, [self]() mutable {
                        self->do_write();
                    });

                    if (sr.action == SessionAction::Continue) {
                        net::post(self->ioc_, [self]() {
                            self->do_read();
                        });
                    } else {
                        net::post(self->ioc_, [self]() {
                            self->ws_.close(websocket::close_code::normal);
                        });
                    }
                }
            );
        }
    );
}


void Session::do_write() {
    if (writeInProgress_){
        return;
    }
    this->writeInProgress_ = true;

    std::shared_ptr<Session> self = shared_from_this();
    net::mutable_buffer buffer;

    {
        std::lock_guard lock(dequeMutex_);
        buffer = net::buffer(messageQueue_.front());
    }

    ws_.async_write(buffer, 
    [self](const beast::error_code ec, const size_t len) {
        std::cout << "MSG Sent EC: " << ec << " size: " << len << std::endl;

        if (!ec) {
            std::lock_guard lock(self->dequeMutex_);
            self->messageQueue_.pop_front();
        }

        self->writeInProgress_ = false;
        if (!self->messageQueue_.empty()) {
            self->do_write();
        }
    });
}
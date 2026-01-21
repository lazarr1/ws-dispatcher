#include "session.hpp"
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
    auto weakSelf = weak_from_this();

    http::async_read(ws_.next_layer(), buffer_, this->req,
    [weakSelf] (beast::error_code error, size_t msgSize) {
        if (error) {
            std::cout << "MSG SIZE: " << msgSize << std::endl;
            std::cout << "Http read error: " << error << std::endl;
            return;
        }

        std::shared_ptr<Session> self = weakSelf.lock();
        if (!websocket::is_upgrade(self->req)) {
            std::cout << "Not an upgrade request" << std::endl;
            return;
        }

        if (self) {
            // Accept the ws handshale/ upgrade
            self->ws_.async_accept(self->req, 
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
    std::weak_ptr<Session> weakSelf = weak_from_this();

    ws_.async_read(buffer_,
        [weakSelf](beast::error_code ec, std::size_t) {
            if (ec) {
                if (ec == websocket::error::closed) return;
                std::cerr << "Read error: " << ec.message() << std::endl;
                return;
            }

            auto self = weakSelf.lock();
            std::string message = beast::buffers_to_string(self->buffer_.data());
            self->buffer_.consume(self->buffer_.size());

            // Offload message processing to the worker pool (5 threads)
            auto weak = std::weak_ptr<Session>(self);
            net::post(self->workers_,
                [weak, message = std::move(message)]() mutable {
                    if (auto s = weak.lock()) {
                        // Each session has its own Router instance (no shared state)
                        s->sh_->processMsg(const_cast<std::string&>(message));
                        auto responseOpt = s->sh_->getResponse();
                        if (responseOpt.has_value()) {
                            std::string response = responseOpt.value();

                            // Post the write back to the main io_context so websocket writes happen on that thread
                            net::post(s->ioc_, [s, response = std::move(response)]() mutable {
                                s->do_write(response);
                            });
                        }
                    }
                }
            );

            if (self->sh_->keepAlive()){
                // Keep reading further messages (until close)
                self->do_read();
            } else {
                self->ws_.async_close("Error in server. (or job done)" ,
                [](beast::error_code ec) {
                    std::cout << "WS Closed with EC: " << ec << std::endl;
                });
            }
        }
    );
}


void Session::do_write(const std::string msg) {
    std::weak_ptr<Session> weakSelf = weak_from_this();

    auto buffer = boost::asio::buffer(msg);
    ws_.async_write(buffer, 
    [](const beast::error_code ec, const size_t len) {
        std::cout << "MSG Sent EC: " << ec << "size: todo check what this means" << len << std::endl;
    });
}
#include "server.h"

#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>

#include <memory>

namespace server
{

Server::Server(boost::asio::io_context& ioc, uint16_t port,
               calc::Application& app, std::function<void()> onReady) :
    acceptor_(ioc, tcp::endpoint(tcp::v4(), port)), app_(app), onReady_(onReady)
{
    acceptor_.listen();
    if (onReady_)
    {
        onReady_();
    }
}

void Server::Run()
{
    DoAccept();
}

void Server::DoAccept()
{
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec)
            {
                DoRead(std::move(socket));
            }
            DoAccept();
        });
}

void Server::DoRead(tcp::socket socket)
{
    auto buf = std::make_shared<boost::asio::streambuf>();
    auto sock = std::make_shared<tcp::socket>(std::move(socket));

    boost::asio::async_read_until(
        *sock, *buf, '\n',
        [this, sock, buf](boost::system::error_code ec, std::size_t) {
            if (ec)
            {
                return;
            }

            std::istream stream(buf.get());
            std::string json;
            std::getline(stream, json);

            std::string response = app_.applicationRun(json);

            boost::asio::async_write(
                *sock, boost::asio::buffer(response),
                [this, sock, buf](boost::system::error_code ec, std::size_t) {
                    if (!ec)
                        DoRead(std::move(*sock));
                });
        });
}

} // namespace server
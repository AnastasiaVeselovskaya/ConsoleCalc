#pragma once
#include "../application.h"

#include <boost/asio.hpp>

namespace server
{

using tcp = boost::asio::ip::tcp;

class Server
{
  public:
    explicit Server(boost::asio::io_context& ioc, uint16_t port,
                    calc::Application& app,
                    std::function<void()> onReady = nullptr);
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;
    Server(Server&&) noexcept = default;
    Server& operator=(Server&&) noexcept = default;
    ~Server() = default;
    void Run();

  private:
    void DoAccept();
    void DoRead(tcp::socket socket);
    std::function<void()> onReady_;
    tcp::acceptor acceptor_;
    calc::Application& app_;
};

} // namespace server
#pragma once
#include <boost/asio.hpp>

#include <string>

namespace test
{

class TestClient
{
  public:
    TestClient(const std::string& host, uint16_t port);
    std::string Send(const std::string& jsonRequest);

  private:
    boost::asio::io_context ioc_;
    boost::asio::ip::tcp::socket socket_;
};

} // namespace test
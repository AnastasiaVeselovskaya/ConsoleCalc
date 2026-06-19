#include "tests_client.h"

#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>

namespace test
{

TestClient::TestClient(const std::string& host, uint16_t port) : socket_(ioc_)
{
    boost::asio::ip::tcp::endpoint ep(
        boost::asio::ip::address::from_string(host), port);
    socket_.connect(ep);
}

std::string TestClient::Send(const std::string& jsonRequest)
{
    boost::asio::write(socket_, boost::asio::buffer(jsonRequest + "\n"));

    boost::asio::streambuf buf;
    boost::asio::read_until(socket_, buf, '\n');
    std::istream stream(&buf);
    std::string response;
    std::getline(stream, response);
    return response;
}

} // namespace test
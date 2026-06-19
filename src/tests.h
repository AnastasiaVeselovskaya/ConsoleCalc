#pragma once

#include "application.h"
#include "server/server.h"
#include "tests_client.h"

#include <boost/asio.hpp>

#include <atomic>
#include <chrono>
#include <memory>
#include <thread>

#include <gtest/gtest.h>

class ServerTest : public ::testing::Test
{
  protected:
    void SetUp() override;
    void TearDown() override;
    std::string Send(const std::string& request);

  private:
    static constexpr uint16_t kPort = 18080;

    boost::asio::io_context ioc_;
    std::unique_ptr<server::Server> server_;
    std::unique_ptr<calc::Application> app_;
    std::unique_ptr<test::TestClient> client_;
    std::thread workerThread_;
    std::atomic<bool> serverReady_{false};
};

class ApplicationTest : public ::testing::Test
{
  protected:
    void SetUp() override;
    void TearDown() override;
    std::string CaptureCommandOutput(const char* command);
    std::string CaptureCommandError(const char* command);
    static std::string HelpOutput();
    std::unique_ptr<calc::Application> app;
};

class CalculatorTest : public ::testing::Test
{
  protected:
    calc::Task task;

    void setTask(int64_t left, char op, int64_t right = 0);
};
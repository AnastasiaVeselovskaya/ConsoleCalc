#include "tests.h"

void ApplicationTest::SetUp()
{
    app = std::make_unique<calc::Application>();
    app->initCache();
}

void ApplicationTest::TearDown()
{
    app.reset();
}

void ServerTest::SetUp()
{
    app_ = std::make_unique<calc::Application>();
    app_->initCache();

    server_ = std::make_unique<server::Server>(
        ioc_, kPort, *app_, [this]() { serverReady_.store(true); });
    server_->Run();

    workerThread_ = std::thread([this]() { ioc_.run(); });

    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    while (!serverReady_.load())
    {
        if (std::chrono::steady_clock::now() > deadline)
            throw std::runtime_error("Server did not start in time");
        std::this_thread::yield();
    }

    client_ = std::make_unique<test::TestClient>("127.0.0.1", kPort);
}

void ServerTest::TearDown()
{
    client_.reset();
    ioc_.stop();
    workerThread_.join();
    server_.reset();
    app_.reset();
}

std::string ServerTest::Send(const std::string& request)
{
    return client_->Send(request);
}

TEST_F(ServerTest, SumReturnsCorrectResult)
{
    auto result = json::parse(
        Send("{\"left operand\":5,\"operation\":\"+\",\"right operand\":9}"));
    ASSERT_EQ(result["result"], 14);
}

TEST_F(ServerTest, SubtractReturnsCorrectResult)
{
    auto result = json::parse(
        Send("{\"left operand\":10,\"operation\":\"-\",\"right operand\":3}"));
    ASSERT_EQ(result["result"], 7);
}

TEST_F(ServerTest, MultiplyReturnsCorrectResult)
{
    auto result = json::parse(
        Send("{\"left operand\":6,\"operation\":\"*\",\"right operand\":7}"));
    ASSERT_EQ(result["result"], 42);
}

TEST_F(ServerTest, DivideReturnsCorrectResult)
{
    auto result = json::parse(
        Send("{\"left operand\":10,\"operation\":\"/\",\"right operand\":2}"));
    ASSERT_EQ(result["result"], 5);
}

TEST_F(ServerTest, PowerReturnsCorrectResult)
{
    auto result = json::parse(
        Send("{\"left operand\":2,\"operation\":\"^\",\"right operand\":10}"));
    ASSERT_EQ(result["result"], 1024);
}

TEST_F(ServerTest, FactorialReturnsCorrectResult)
{
    auto result = json::parse(Send("{\"left operand\":5,\"operation\":\"!\"}"));
    ASSERT_EQ(result["result"], 120);
}

TEST_F(ServerTest, DivisionByZeroReturnsError)
{
    auto result = json::parse(
        Send("{\"left operand\":5,\"operation\":\"/\",\"right operand\":0}"));
    ASSERT_TRUE(result.contains("error"));
    ASSERT_EQ(result["error"], "Division by zero");
}

TEST_F(ServerTest, NegativeFactorialReturnsError)
{
    auto result =
        json::parse(Send("{\"left operand\":-3,\"operation\":\"!\"}"));
    ASSERT_TRUE(result.contains("error"));
    ASSERT_EQ(result["error"], "Negative factorial argument");
}

TEST_F(ServerTest, InvalidJsonReturnsError)
{
    auto result = json::parse(Send("not a json"));
    ASSERT_TRUE(result.contains("error"));
}

TEST_F(ApplicationTest, InvalidJsonFormatPrintsHelp)
{
    auto result = json::parse(app->applicationRun("1 + 2"));
    ASSERT_TRUE(result.contains("error"));
}

TEST_F(ApplicationTest, PowerPrintsResult)
{
    auto result = json::parse(app->applicationRun(
        "{\"left operand\":5,\"operation\":\"^\",\"right operand\":9}"));
    ASSERT_EQ(result["result"], 1953125);
}

TEST_F(ApplicationTest, NegativeFactorialPrintsError)
{
    auto result = json::parse(
        app->applicationRun("{\"left operand\":-5,\"operation\":\"!\"}"));
    ASSERT_TRUE(result.contains("error"));
    ASSERT_EQ(result["error"], "Negative factorial argument");
}

TEST_F(ApplicationTest, LargeFactorialPrintsError)
{
    auto result = json::parse(app->applicationRun(
        "{\"left operand\":500000000,\"operation\":\"!\"}"));
    ASSERT_TRUE(result.contains("error"));
    ASSERT_EQ(result["error"], "Factorial argument too large");
}

TEST_F(ApplicationTest, DivisionByZeroPrintsError)
{
    auto result = json::parse(app->applicationRun(
        "{\"left operand\":5,\"operation\":\"/\",\"right operand\":0}"));
    ASSERT_TRUE(result.contains("error"));
    ASSERT_EQ(result["error"], "Division by zero");
}

TEST_F(ApplicationTest, OverflowPrintsError)
{
    auto result = json::parse(app->applicationRun(
        "{\"left operand\":9223372036854775807,\"operation\":\"*\",\"right "
        "operand\":2}"));
    ASSERT_TRUE(result.contains("error"));
    ASSERT_EQ(result["error"], "Overflow");
}

TEST_F(ApplicationTest, InvalidOperationPrintsError)
{
    auto result = json::parse(
        app->applicationRun("{\"left operand\":5,\"operation\":\")\"}"));
    ASSERT_TRUE(result.contains("error"));
    ASSERT_EQ(result["error"], "Operation is invalid or not supported");
}

TEST_F(ApplicationTest, FactorialPrintsResult)
{
    auto result = json::parse(
        app->applicationRun("{\"left operand\":5,\"operation\":\"!\"}"));
    ASSERT_EQ(result["result"], 120);
}

TEST_F(ApplicationTest, SumPrintsResult)
{
    auto result = json::parse(app->applicationRun(
        "{\"left operand\":5,\"operation\":\"+\",\"right operand\":9}"));
    ASSERT_EQ(result["result"], 14);
}

TEST_F(ApplicationTest, SubtractPrintsResult)
{
    auto result = json::parse(app->applicationRun(
        "{\"left operand\":5,\"operation\":\"-\",\"right operand\":9}"));
    ASSERT_EQ(result["result"], -4);
}

TEST_F(ApplicationTest, MultiplyPrintsResult)
{
    auto result = json::parse(app->applicationRun(
        "{\"left operand\":5,\"operation\":\"*\",\"right operand\":9}"));
    ASSERT_EQ(result["result"], 45);
}

TEST_F(ApplicationTest, DividePrintsResult)
{
    auto result = json::parse(app->applicationRun(
        "{\"left operand\":5,\"operation\":\"/\",\"right operand\":9}"));
    ASSERT_EQ(result["result"], 0.555556);
}

void CalculatorTest::setTask(int64_t left, char op, int64_t right)
{
    task.setFirstNum(left);
    task.setOperation(op);
    task.setSecondNum(right);
}

TEST_F(CalculatorTest, PowerReturnsCorrectResult)
{
    setTask(5, '^', 9);
    task.makeCalculate();
    ASSERT_EQ(static_cast<int>(std::pow(5, 9)), task.getResult());
}

TEST_F(CalculatorTest, FactorialReturnsCorrectResult)
{
    setTask(5, '!');
    task.makeCalculate();
    ASSERT_EQ(120, task.getResult());
}

TEST_F(CalculatorTest, MultiplicationReturnsCorrectResult)
{
    setTask(5, '*', 9);
    task.makeCalculate();
    ASSERT_EQ(45, task.getResult());
}

TEST_F(CalculatorTest, SubtractionReturnsCorrectResult)
{
    setTask(5, '-', 9);
    task.makeCalculate();
    ASSERT_EQ(-4, task.getResult());
}

TEST_F(CalculatorTest, DivisionReturnsCorrectResult)
{
    setTask(10, '/', 2);
    task.makeCalculate();
    ASSERT_EQ(5, task.getResult());
}

TEST_F(CalculatorTest, SumReturnsCorrectResult)
{
    setTask(5, '+', 9);
    task.makeCalculate();
    ASSERT_EQ(14, task.getResult());
}

TEST_F(CalculatorTest, NegativeFactorialThrows)
{
    ASSERT_THROW(
        {
            setTask(-5, '!');
            task.makeCalculate();
        },
        calc::NegativeFactorialError);
}

TEST_F(CalculatorTest, LargeFactorialThrows)
{
    ASSERT_THROW(
        {
            setTask(500000000, '!');
            task.makeCalculate();
        },
        std::out_of_range);
}

TEST_F(CalculatorTest, DivisionByZeroThrows)
{
    ASSERT_THROW(
        {
            setTask(5, '/', 0);
            task.makeCalculate();
        },
        std::runtime_error);
}

TEST_F(CalculatorTest, InvalidOperationThrows)
{
    ASSERT_THROW(task.setOperation(')'), std::invalid_argument);
}
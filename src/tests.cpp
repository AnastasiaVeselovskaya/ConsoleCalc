#include "tests.h"

void ApplicationTest::SetUp()
{
    app = std::make_unique<calc::Application>();
    argv = new char*[2];
    argv[0] = const_cast<char*>("consolecalc");
}

void ApplicationTest::TearDown()
{
    app.reset();
    delete[] argv;
}

std::string ApplicationTest::CaptureCommandOutput(const char* command)
{
    argv[1] = const_cast<char*>(command);
    ::testing::internal::CaptureStdout();
    try
    {
        app->applicationRun(2, argv);
    }
    catch (...)
    {
        ::testing::internal::GetCapturedStdout();
        throw;
    }
    std::string captured = ::testing::internal::GetCapturedStdout();
    return captured;
}

std::string ApplicationTest::CaptureCommandError(const char* command)
{
    argv[1] = const_cast<char*>(command);
    ::testing::internal::CaptureStderr();
    try
    {
        app->applicationRun(2, argv);
    }
    catch (...)
    {
        ::testing::internal::GetCapturedStdout();
        throw;
    }
    std::string captured = ::testing::internal::GetCapturedStderr();
    return captured;
}

std::string ApplicationTest::HelpOutput()
{
    return "Usage: {\"left operand\":<operand>, \"operation\":<operation>, "
           "\"right operand\":<operand>}\n"
           "-------------------------------------------------------------------"
           "---------------------------------------------------------------\n"
           "\tNote that only operations below are supported now:\n"
           "\t+ - '/' '*' ! ^\n"
           "\tUnary operations will only use left operand and do not require "
           "right operand.\n";
}

TEST_F(ApplicationTest, InvalidJsonFormatPrintsHelp)
{
    ASSERT_EQ(HelpOutput(), CaptureCommandOutput("1 + 2"));
}

TEST_F(ApplicationTest, FactorialPrintsResult)
{
    ASSERT_EQ("5! = 120\n",
              CaptureCommandOutput("{\"left operand\":5,\"operation\":\"!\"}"));
}

TEST_F(ApplicationTest, SumPrintsResult)
{
    ASSERT_EQ(
        "5 + 9 = 14\n",
        CaptureCommandOutput(
            "{\"left operand\":5,\"operation\":\"+\",\"right operand\":9}"));
}

TEST_F(ApplicationTest, SubtractPrintsResult)
{
    ASSERT_EQ(
        "5 - 9 = -4\n",
        CaptureCommandOutput(
            "{\"left operand\":5,\"operation\":\"-\",\"right operand\":9}"));
}

TEST_F(ApplicationTest, MultiplyPrintsResult)
{
    ASSERT_EQ(
        "5 * 9 = 45\n",
        CaptureCommandOutput(
            "{\"left operand\":5,\"operation\":\"*\",\"right operand\":9}"));
}

TEST_F(ApplicationTest, DividePrintsResult)
{
    ASSERT_EQ(
        "5 / 9 = 0.555556\n",
        CaptureCommandOutput(
            "{\"left operand\":5,\"operation\":\"/\",\"right operand\":9}"));
}

TEST_F(ApplicationTest, PowerPrintsResult)
{
    ASSERT_EQ(
        "5 ^ 9 = 1953125\n",
        CaptureCommandOutput(
            "{\"left operand\":5,\"operation\":\"^\",\"right operand\":9}"));
}

TEST_F(ApplicationTest, NegativeFactorialPrintsError)
{
    ASSERT_EQ("Negative number passed for factorial argument\n",
              CaptureCommandError("{\"left operand\":-5,\"operation\":\"!\"}"));
}

TEST_F(ApplicationTest, LargeFactorialPrintsError)
{
    ASSERT_EQ("Factorial argument too large and will cause overflow\n",
              CaptureCommandError(
                  "{\"left operand\":500000000,\"operation\":\"!\"}"));
}

TEST_F(ApplicationTest, DivisionByZeroPrintsError)
{
    ASSERT_EQ(
        "Division by zero\n",
        CaptureCommandError(
            "{\"left operand\":5,\"operation\":\"/\",\"right operand\":0}"));
}

TEST_F(ApplicationTest, OverflowPrintsError)
{
    ASSERT_EQ(
        "Operation resulted in type overflow\n",
        CaptureCommandError(
            "{\"left operand\":9223372036854775807,\"operation\":\"*\",\"right "
            "operand\":2}"));
}

TEST_F(ApplicationTest, InvalidOperationPrintsError)
{
    ASSERT_EQ("Operation is invalid or not supported at the moment\n",
              CaptureCommandError("{\"left operand\":5,\"operation\":\")\"}"));
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
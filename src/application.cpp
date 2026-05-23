#include "application.h"

namespace calc
{
void to_json(json& jsonOutput, const calc::Task& taskInput)
{
    jsonOutput = json{{"left operand", taskInput.getFirstNum()},
                      {"operation", taskInput.getOperation()},
                      {"right operand", taskInput.getSecondNum()},
                      {"result", taskInput.getResult()}};
}

void from_json(const json& jsonInput, calc::Task& taskOutput)
{
    if (jsonInput.contains("right operand"))
    {
        taskOutput.setSecondNum(jsonInput.at("right operand").get<int64_t>());
    }

    const int64_t first = jsonInput.at("left operand").get<int64_t>();
    const char operation = jsonInput.at("operation").get<std::string>().at(0);

    taskOutput.setFirstNum(first);
    try
    {
        taskOutput.setOperation(operation);
    }
    catch (const std::invalid_argument& e)
    {
        throw;
    }
}

void Application::displayHelp()
{
    std::cout
        << ("Usage: {\"left operand\":<operand>, \"operation\":<operation>, "
            "\"right operand\":<operand>}\n"
            "------------------------------------------------------------------"
            "-"
            "---------------------------------------------------------------\n"
            "\tNote that only operations below are supported now:\n"
            "\t+ - '/' '*' ! ^\n"
            "\tUnary operations will only use left operand and do not require "
            "right operand.\n");
}

void Application::makeTask(char** argv)
{
    const std::string strJson =
        argv[1]; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    Logger::GetInstance().LogDebug("Input JSON: " + strJson);
    const json jsonInput = json::parse(strJson);
    try
    {
        calculator_ = std::make_unique<Task>(jsonInput.get<Task>());
    }
    catch (const std::invalid_argument& e)
    {
        Logger::GetInstance().LogError("Error " + std::string(e.what()));
        throw;
    }
}

void Application::printResult() const
{
    const double result = calculator_->getResult();

    Logger::GetInstance().LogInfo("Result: " + std::to_string(result));

    auto getResultString = [this, result]() -> std::string {
        auto firstNum = std::to_string(calculator_->getFirstNum());
        auto operation = std::string(1, calculator_->getOperation());

        auto formatResult = [this](double res) -> std::string {
            if (res == static_cast<double>(static_cast<int64_t>(res)))
            {
                return std::to_string(static_cast<int64_t>(res));
            }
            std::ostringstream oss;
            oss << std::setprecision(doublePrecision_) << res;
            return oss.str();
        };

        auto strRes = formatResult(result);

        if (operation == "!")
        {
            return firstNum + operation + " = " + strRes + "\n";
        }
        return firstNum + " " + operation + " " +
               std::to_string(calculator_->getSecondNum()) + " = " + strRes +
               "\n";
    };

    std::cout << getResultString();
}

void Application::applicationRun(int argc, char** argv)
{
    if (argc != 2)
    {
        displayHelp();
        Logger::GetInstance().LogDebug(
            "Application launched with incorrect arguments count");
        return;
    }

    Logger::GetInstance().LogInfo("Application started");

    try
    {
        makeTask(argv);
        Logger::GetInstance().LogInfo("Task successfully formed");
    }
    catch (const nlohmann::json::parse_error& e)
    {
        displayHelp();
        Logger::GetInstance().LogError("Json parsing error");
        return;
    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << "Operation is invalid or not supported at the moment\n";
        Logger::GetInstance().LogError("Invalid opeartion");
    }

    try
    {
        if (!calculator_)
        {
            Logger::GetInstance().LogDebug(
                "Calculator was not initialized correctly");
            return;
        }
        calculator_->makeCalculate();
        printResult();
    }
    // NegativeFactorialError is derived from std::invalid_argument,
    // must catch it before base class.
    catch (const calc::NegativeFactorialError& e)
    {
        std::cerr << "Negative number passed for factorial argument\n";
        Logger::GetInstance().LogWarning(e.what());
    }
    catch (const std::overflow_error& e)
    {
        std::cerr << "Operation resulted in type overflow\n";
        Logger::GetInstance().LogError("Overflow: " + std::string(e.what()));
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "Division by zero\n";
        Logger::GetInstance().LogError("Division by zero");
    }
    catch (const std::out_of_range& e)
    {
        std::cerr << "Factorial argument too large and will cause overflow\n";
        Logger::GetInstance().LogError("Factorial argument too large");
    }
    catch (const std::invalid_argument& e)
    {
        displayHelp();
        Logger::GetInstance().LogError(
            "Invalid argument syntax caught, display help");
    }
    catch (const std::exception& e)
    {
        std::cerr << "Unknown exception\n";
        Logger::GetInstance().LogCritical("Unexpected exception caught: " +
                                          std::string(e.what()));
    }
}

} // namespace calc
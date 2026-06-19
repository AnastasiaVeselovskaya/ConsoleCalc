#include "application.h"

namespace calc
{

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

void Application::makeTask(const std::string& jsonStr)
{
    const json jsonInput = json::parse(jsonStr);
    calculator_ = std::make_unique<Task>(Task::fromJson(jsonInput));
}

void Application::initCache()
{
    cacher_ = std::make_unique<cache::Cacher>();
    cacher_->InitCache();
    Logger::GetInstance().LogInfo("Cache initialized successfully");
}

void Application::SetCachedResult()
{
    auto cachedValue = cacher_->GetResultFromCache(calculator_->getFirstNum(),
                                                   calculator_->getOperation(),
                                                   calculator_->getSecondNum());

    if (std::holds_alternative<double>(cachedValue))
    {
        calculator_->setResult(std::get<double>(cachedValue));
        return;
    }

    auto ec = std::get<cache::ErrorCode>(cachedValue);

    // Caching internal errors do not affect the overall ApplicationRun,
    // so they're not throwing errors, but logging them instead.
    // Errors as a result of calculation will throw errors according to ec.
    switch (ec)
    {
        case cache::ErrorCode::Success:
            Logger::GetInstance().LogError(
                "Result undefined for successfully cached opeation");
            return;
        case cache::ErrorCode::DivisionByZero:
            throw std::runtime_error("");
        case cache::ErrorCode::FactorialTooLarge:
            throw std::out_of_range("");
        case cache::ErrorCode::NegativeFactorial:
            throw NegativeFactorialError("");
        case cache::ErrorCode::Overflow:
            throw std::overflow_error("");
        case cache::ErrorCode::NotFoundInCache:
            return;
        default:
            Logger::GetInstance().LogError(
                "Unexpected error code in cache: " +
                std::to_string(static_cast<int>(ec)));
            return;
    }
}

void Application::printResult() const
{
    const double result = calculator_->getResult().value();

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
               std::to_string(calculator_->getSecondNum().value()) + " = " +
               strRes + "\n";
    };

    std::cout << getResultString();
}

void Application::CacheCalculation(cache::ErrorCode ec)
{
    cache::CalculationLog log{
        calculator_->getFirstNum(), calculator_->getSecondNum(),
        calculator_->getOperation(),
        (ec == cache::ErrorCode::Success ? calculator_->getResult()
                                         : std::nullopt),
        ec};

    cacher_->Cache(log);
}

std::string Application::applicationRun(const std::string& jsonRequest)
{
    Logger::GetInstance().LogInfo("Application started");
    
    try
    {
        makeTask(jsonRequest);
        Logger::GetInstance().LogInfo("Task successfully formed");
    }
    catch (const nlohmann::json::parse_error& e)
    {
        Logger::GetInstance().LogError("Json parsing error");
        return json::object({{"error", "Invalid JSON"}}).dump() + "\n";
    }
    catch (const std::invalid_argument& e)
    {
        Logger::GetInstance().LogError("Invalid operation");
        return json::object(
                   {{"error", "Operation is invalid or not supported"}})
                   .dump() +
               "\n";
    }

    try
    {
        if (!calculator_)
        {
            Logger::GetInstance().LogDebug(
                "Calculator was not initialized correctly");
            return json::object({{"error", "Internal error"}}).dump() + "\n";
        }

        SetCachedResult();

        if (calculator_->getResult().has_value())
        {
            Logger::GetInstance().LogInfo("Got result from cache");
            return json::object({{"result", calculator_->getResult().value()}})
                       .dump() +
                   "\n";
        }

        Logger::GetInstance().LogInfo(
            "Calculating the result using Task class");
        calculator_->makeCalculate();
        CacheCalculation(cache::ErrorCode::Success);
        return json::object({{"result", calculator_->getResult().value()}})
                   .dump() +
               "\n";
    }
    catch (const calc::NegativeFactorialError& e)
    {
        CacheCalculation(cache::ErrorCode::NegativeFactorial);
        Logger::GetInstance().LogWarning(e.what());
        return json::object({{"error", "Negative factorial argument"}}).dump() +
               "\n";
    }
    catch (const std::overflow_error& e)
    {
        CacheCalculation(cache::ErrorCode::Overflow);
        Logger::GetInstance().LogError("Overflow: " + std::string(e.what()));
        return json::object({{"error", "Overflow"}}).dump() + "\n";
    }
    catch (const std::runtime_error& e)
    {
        CacheCalculation(cache::ErrorCode::DivisionByZero);
        Logger::GetInstance().LogError("Division by zero");
        return json::object({{"error", "Division by zero"}}).dump() + "\n";
    }
    catch (const std::out_of_range& e)
    {
        CacheCalculation(cache::ErrorCode::FactorialTooLarge);
        Logger::GetInstance().LogError("Factorial argument too large");
        return json::object({{"error", "Factorial argument too large"}})
                   .dump() +
               "\n";
    }
    catch (const std::exception& e)
    {
        Logger::GetInstance().LogCritical("Unexpected exception: " +
                                          std::string(e.what()));
        return json::object({{"error", e.what()}}).dump() + "\n";
    }
}

} // namespace calc
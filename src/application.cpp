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

void Application::makeTask(char** argv)
{
    const std::string strJson =
        argv[1]; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    Logger::GetInstance().LogDebug("Input JSON: " + strJson);
    const json jsonInput = json::parse(strJson);

    calculator_ = std::make_unique<Task>(Task::fromJson(jsonInput));
}

void Application::initCache() {
    cacher_ = std::make_unique<cache::Cacher>();
    cacher_->InitCache();
    Logger::GetInstance().LogInfo("Cache initialized successfully");
}

void Application::SetCachedResult() {
    auto cachedValue = cacher_->GetResultFromCache(
        calculator_->getFirstNum(), calculator_->getOperation(),
        calculator_->getSecondNum());
        
    if (std::holds_alternative<double>(cachedValue)) {
        calculator_->setResult(std::get<double>(cachedValue));
        return;
    }

    auto ec = std::get<cache::ErrorCode>(cachedValue);

    // Caching internal errors do not affect the overall ApplicationRun,
    // so they're not throwing errors, but logging them instead.
    // Errors as a result of calculation will throw errors according to ec.
    switch (ec) {
    case cache::ErrorCode::Success:
        Logger::GetInstance().LogError("Result undefined for successfully cached opeation");
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
        Logger::GetInstance().LogError("Unexpected error code in cache: "
             + std::to_string(static_cast<int>(ec)));
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
               std::to_string(calculator_->getSecondNum().value()) + " = " + strRes +
               "\n";
    };

    std::cout << getResultString();
}

void Application::CacheCalculation(cache::ErrorCode ec) {
    cache::CalculationLog log{
        calculator_->getFirstNum(), 
        calculator_->getSecondNum(),
        calculator_->getOperation(), 
        (ec == cache::ErrorCode::Success ? 
            calculator_->getResult() 
            : std::nullopt), 
        ec
    };

    cacher_->Cache(log);
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

    initCache();

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

        SetCachedResult();

        if (calculator_->getResult().has_value()) {
            Logger::GetInstance().LogInfo("Got result from cache: " 
                + std::to_string(calculator_->getResult().value()));
            printResult();
            return;
        }

        Logger::GetInstance().LogInfo("Cached result not found");
        Logger::GetInstance().LogInfo("Calculating the result using Task class");
        calculator_->makeCalculate();
        CacheCalculation(cache::ErrorCode::Success);
        printResult();
    }
    // NegativeFactorialError is derived from std::invalid_argument,
    // must catch it before base class.
    catch (const calc::NegativeFactorialError& e)
    {
        std::cerr << "Negative number passed for factorial argument\n";
        CacheCalculation(cache::ErrorCode::NegativeFactorial);
        Logger::GetInstance().LogWarning(e.what());
    }
    catch (const std::overflow_error& e)
    {
        std::cerr << "Operation resulted in type overflow\n";
        CacheCalculation(cache::ErrorCode::Overflow);
        Logger::GetInstance().LogError("Overflow: " + std::string(e.what()));
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "Division by zero\n";
        CacheCalculation(cache::ErrorCode::DivisionByZero);
        Logger::GetInstance().LogError("Division by zero");
    }
    catch (const std::out_of_range& e)
    {
        std::cerr << "Factorial argument too large and will cause overflow\n";
        CacheCalculation(cache::ErrorCode::FactorialTooLarge);
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
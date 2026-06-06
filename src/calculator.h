#pragma once

#include <integermath/integermath.h>

#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <stdexcept>

namespace calc
{

class NegativeFactorialError : public std::invalid_argument
{
  public:
    explicit NegativeFactorialError(const std::string& msg) :
        std::invalid_argument(msg)
    {}
};

class Task
{
  public:
    double makeCalculate();

    void setFirstNum(int64_t firstNum);
    void setSecondNum(int64_t secondNum);
    void setOperation(char operation);
    void setResult(double result);

    int64_t getFirstNum() const;
    std::optional<int64_t> getSecondNum() const;
    char getOperation() const;
    std::optional<double> getResult() const;
    
    static Task fromJson(const nlohmann::json& j);

  private:
    static bool isValidOperation(char operation);

    std::unique_ptr<integermath::CalculationModule<int64_t>> calc_module_{
        std::make_unique<integermath::CalculationModule<int64_t>>()};
    int64_t firstNum_;
    std::optional<int64_t> secondNum_ = std::nullopt;
    char operation_;
    std::optional<double> result_ = std::nullopt;
    // Since factorial works recursively, passing too large a number will cause stack overflow.
    // Hardcoded capacity for it will prevent segmentation faults.
    int64_t factorialArgCap_ = 20;
};

} // namespace calc
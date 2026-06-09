#include "calculator.h"

namespace calc
{

Task Task::fromJson(const nlohmann::json& j)
{
    Task task;
    if (j.contains("right operand")) {
        task.setSecondNum(j.at("right operand").get<int64_t>());
    }

    task.setFirstNum(j.at("left operand").get<int64_t>());
    task.setOperation(j.at("operation").get<std::string>().at(0));
    return task;
}

bool Task::isValidOperation(char operation)
{
    return operation == '+' || operation == '-' || operation == '*' ||
           operation == '/' || operation == '!' || operation == '^';
}

double Task::makeCalculate()
{
    calc_module_->SetLeftNumber(firstNum_);
    if (secondNum_.has_value()) {
        calc_module_->SetRightNumber(secondNum_.value());        
    }

    double result = 0;

    try
    {
        switch (operation_)
        {
            case '+':
                result = calc_module_->sum();
                break;
            case '-':
                result = calc_module_->subtract();
                break;
            case '*':
                result = calc_module_->multiply();
                break;
            case '/':
                result = calc_module_->divide();
                break;
            case '!':
                if (firstNum_ < 0)
                {
                    throw NegativeFactorialError(
                        "Negative number passed for factorial argument\n");
                }
                if (firstNum_ > factorialArgCap_)
                {
                    throw std::out_of_range("Factorial argument too large and "
                                            "will cause owerflow\n");
                }
                result = calc_module_->factorial();
                break;
            case '^':
                result = calc_module_->power();
                break;
            default:
                break;
        }
    }
    catch (...)
    {
        throw;
    }

    result_ = result;

    return result;
}

int64_t Task::getFirstNum() const
{
    return firstNum_;
}

std::optional<int64_t> Task::getSecondNum() const
{
    return secondNum_;
}

char Task::getOperation() const
{
    return operation_;
}

std::optional<double> Task::getResult() const
{
    return result_;
}

void Task::setFirstNum(int64_t firstNum)
{
    firstNum_ = firstNum;
}

void Task::setSecondNum(int64_t secondNum)
{
    secondNum_ = secondNum;
}

void Task::setResult(double result) {
    result_ = result;
}

void Task::setOperation(char operation)
{
    if (!isValidOperation(operation))
    {
        throw std::invalid_argument(
            std::string(1, operation) +
            " is invalid operation or not supported at the moment\n");
    }

    operation_ = operation;
}

} // namespace calc
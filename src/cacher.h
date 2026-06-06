#pragma once
#include <cstdint>
#include <climits>
#include <deque>
#include <optional>
#include <unordered_map>
#include <variant>
#include "libpqwrapper.h"

namespace cache {

enum class ErrorCode
{
    Success = 0,
    DivisionByZero = 1,
    NegativeFactorial = 2,
    Overflow = 3,
    FactorialTooLarge = 4,
    UnknownError = 5,
    NotFoundInCache = 6
};


struct CalculationLog {
    int64_t leftNum;
    std::optional<int64_t> rightNum;
    char operation;
    std::optional<double> result;
    ErrorCode status;
};


class Cacher {
public:
    explicit Cacher();
    void InitCache() const;
    void Cache(const CalculationLog& record);
    std::variant<ErrorCode, double> GetResultFromCache(int64_t first, 
                                             char operation, 
                                             std::optional<int64_t> second);

private:
    const char* dbConnectionStr =
        "host=127.0.0.1 port=5432 dbname=CalcCache user=calcuser password=qweasd123";
    void SaveToDBCache(const CalculationLog& record) const;
    void SaveToShortTermCache(const CalculationLog& record);
    std::string MakeCacheKey(int64_t first, 
                             char operation, 
                             std::optional<int64_t> second);
    void WarmUpCache();
    std::variant<ErrorCode, double> GetResultFromDB(int64_t first,
                                                    char operation,
                                                    std::optional<int64_t> second) const;
    libpqwrapper::PgConnection connection_;
    std::unordered_map<std::string, std::variant<ErrorCode, double>> shortTermCache_;
};

}
#include "cacher.h"

namespace cache {

Cacher::Cacher() 
    : connection_(dbConnectionStr) {
    WarmUpCache();
}

void Cacher::InitCache() const {
    connection_.Exec("SET client_min_messages = WARNING");
    connection_.Exec(R"(
        CREATE TABLE IF NOT EXISTS calc_history (
            id          BIGSERIAL PRIMARY KEY,
            left_num    BIGINT        NOT NULL,
            right_num   BIGINT,
            operation   CHAR(1)       NOT NULL,
            result      DOUBLE PRECISION,
            status      INTEGER       NOT NULL DEFAULT 0,
            created_at  TIMESTAMPTZ   NOT NULL DEFAULT NOW()
        )
    )");
}

void Cacher::Cache(const CalculationLog& record) {
    SaveToDBCache(record);
    SaveToShortTermCache(record);
}

std::variant<ErrorCode, double> Cacher::GetResultFromCache(int64_t first, 
                                                           char operation, 
                                                           std::optional<int64_t> second) const {
    std::string key = MakeCacheKey(first, operation, second);
    auto it = shortTermCache_.find(key);
    if (it != shortTermCache_.end()) {
        return it->second;
    }
    return GetResultFromDB(first, operation, second);
}

void Cacher::SaveToDBCache(const CalculationLog& record) const {
    const std::string sql = R"(
        INSERT INTO calc_history
            (left_num, right_num, operation, result, status)
        VALUES ($1, $2, $3, $4, $5)
    )";

    std::vector<std::string> params = {
        std::to_string(record.leftNum),
        record.rightNum ? std::to_string(*record.rightNum) : "",
        std::string(1, record.operation),
        record.result ? std::to_string(*record.result) : "",
        std::to_string(static_cast<int>(record.status))
    };

    connection_.ExecWithParams(sql, params);
}

void Cacher::SaveToShortTermCache(const CalculationLog& record) {
    std::string key = MakeCacheKey(record.leftNum, record.operation, 
        record.rightNum.has_value() ? record.rightNum.value() : LONG_MAX);
    
    if (record.status == ErrorCode::Success) {
        shortTermCache_[key] = record.result.value();
    } else {
        shortTermCache_[key] = record.status;
    }
}

std::string Cacher::MakeCacheKey(int64_t first, 
                             char operation, 
                             std::optional<int64_t> second) const noexcept {
    std::string key = std::to_string(first) + operation;
    if (second.has_value()) {
        key += std::to_string(second.value());
    }
    return key;
}

void Cacher::WarmUpCache() {
    const std::string sql = R"(
        SELECT left_num, right_num, operation, result, status
        FROM calc_history
        ORDER BY created_at DESC
        LIMIT 1000
    )";

    libpqwrapper::PgResult res = connection_.Exec(sql);

    for (int i = 0; i < res.Rows(); ++i) {
        int64_t leftNum = std::stoll(res.Value(i, 0));
        std::optional<int64_t> rightNum = res.IsNull(i, 1) 
            ? std::nullopt 
            : std::optional{std::stoll(res.Value(i, 1))};
        char operation = res.Value(i, 2)[0];
        std::optional<double> result = res.IsNull(i, 3)
            ? std::nullopt
            : std::optional{std::stod(res.Value(i, 3))};
        ErrorCode status = static_cast<ErrorCode>(std::stoi(res.Value(i, 4)));

        std::string key = MakeCacheKey(leftNum, operation, rightNum);

        if (status == ErrorCode::Success) {
            shortTermCache_[key] = result.value();
        } else {
            shortTermCache_[key] = status;
        }
    }
}

std::variant<ErrorCode, double> Cacher::GetResultFromDB(int64_t first,
                                              char operation,
                                              std::optional<int64_t> second) const {
    const std::string sql = R"(
        SELECT result FROM calc_history 
        WHERE left_num = $1 
          AND operation = $2 
          AND status = 0
          AND ($3::BIGINT IS NULL OR right_num = $3::BIGINT)
        ORDER BY created_at DESC 
        LIMIT 1
    )";

    std::vector<std::string> params = {
        std::to_string(first),
        std::string(1, operation),
        second ? std::to_string(*second) : ""
    };

    libpqwrapper::PgResult res = connection_.ExecWithParams(sql, params);
    
    if (res.Rows() == 0 || res.IsNull(0, 0)) {
        return ErrorCode::NotFoundInCache;;
    }
    
    return std::stod(res.Value(0, 0));
}

} // namespace cache
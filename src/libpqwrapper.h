#pragma once
#include <libpq-fe.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

namespace libpqwrapper {

class PgResult {
public:
    PgResult(PGresult* result);
    void CheckStatus() const;
    int Rows() const;
    int Cols() const;
    std::string Value(int row, int col) const;
    bool IsNull(int row, int col) const;

private:
    std::unique_ptr<PGresult, decltype(&PQclear)> result_;
};

class PgConnection {
public:
    PgConnection (const std::string& connStr);
    PgResult Exec(const std::string& sql) const;
    PgResult ExecWithParams(
        const std::string& sql,
        const std::vector<std::string>& params) const;
    bool IsConnected() const;
    std::string LastError() const;
private:
    std::unique_ptr<PGconn, decltype(&PQfinish)> connection_;
};

} // namespace libpqwrapper
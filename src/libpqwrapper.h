#pragma once
#include <libpq-fe.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace libpqwrapper
{

class PgResult
{
  public:
    explicit PgResult(PGresult* result);
    PgResult(const PgResult&) = delete;
    PgResult& operator=(const PgResult&) = delete;
    PgResult(PgResult&&) noexcept = default;
    PgResult& operator=(PgResult&&) noexcept = default;
    ~PgResult() = default;

    void CheckStatus() const;
    int Rows() const noexcept;
    int Cols() const noexcept;
    std::string Value(int row, int col) const;
    bool IsNull(int row, int col) const noexcept;

  private:
    std::unique_ptr<PGresult, decltype(&PQclear)> result_;
};

class PgConnection
{
  public:
    explicit PgConnection(const std::string& connStr);
    PgConnection(const PgConnection&) = delete;
    PgConnection& operator=(const PgConnection&) = delete;
    PgConnection(PgConnection&&) noexcept = default;
    PgConnection& operator=(PgConnection&&) noexcept = default;
    ~PgConnection() = default;

    PgResult Exec(const std::string& sql) const;
    PgResult ExecWithParams(const std::string& sql,
                            const std::vector<std::string>& params) const;
    bool IsConnected() const noexcept;
    std::string LastError() const;

  private:
    std::unique_ptr<PGconn, decltype(&PQfinish)> connection_;
};

} // namespace libpqwrapper
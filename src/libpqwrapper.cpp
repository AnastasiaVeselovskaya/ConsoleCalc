#include "libpqwrapper.h"

namespace libpqwrapper {

PgResult::PgResult(PGresult* result) 
    : result_(result, &PQclear) {}

void PgResult::CheckStatus() const {
    const auto status = PQresultStatus(result_.get());
    if (status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK)
    {
        std::string err = PQresultErrorMessage(result_.get());
        throw std::runtime_error("PostgreSQL error: " + err);
    }
}

int PgResult::Rows() const {
    return PQntuples(result_.get());
}

int PgResult::Cols() const {
    return PQnfields(result_.get());
}

std::string PgResult::Value(int row, int col) const {
    return PQgetvalue(result_.get(), row, col);
}

bool PgResult::IsNull(int row, int col) const {
    return PQgetisnull(result_.get(), row, col) == 1;
}

PgConnection::PgConnection (const std::string& connStr)
    : connection_(PQconnectdb(connStr.c_str()), &PQfinish) {}

PgResult PgConnection::Exec(const std::string& sql) const {
    return PgResult(PQexec(connection_.get(), sql.c_str()));
}

PgResult PgConnection::ExecWithParams(const std::string& sql, const std::vector<std::string>& params) const {
    std::vector<const char*> cParams;
    for (const auto& param : params) {
        cParams.push_back(param  == ""? nullptr : param.c_str() );
    }

    return PgResult(PQexecParams(
        connection_.get(),
        sql.c_str(),
        static_cast<int>(cParams.size()),
        nullptr,
        cParams.data(),
        nullptr,
        nullptr,
        0
    ));
}

bool PgConnection::IsConnected() const {
    return PQstatus(connection_.get());
}

std::string PgConnection::LastError() const {
    return PQerrorMessage(connection_.get());
}

} // namespace libpqwrapper
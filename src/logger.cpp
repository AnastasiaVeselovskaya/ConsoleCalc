#include "logger.h"

Logger::Logger()
{
    spdlog::drop_all();
    std::string logsPath_;

    const char* logsDir = getenv("LOGS_DIRECTORY");
    logsPath_ = logsDir != nullptr
                ? std::string(logsDir) + "/calc_logs.txt"
                : "/var/log/consolecalc/calc_logs.txt";
    
    spdLogger_ = spdlog::rotating_logger_mt("calc_logger", logsPath_, maxSize_,
                                            maxFiles_);
    spdLogger_->flush_on(spdlog::level::trace);
}

Logger& Logger::GetInstance()
{
    static Logger logger;
    return logger;
}

void Logger::SetLogPath(const std::string& filepath)
{
    logsPath_ = filepath;
}
void Logger::SetMaxFileSize(int newSize)
{
    maxSize_ = newSize;
}
void Logger::SetMaxFilesCount(int filesCount)
{
    maxFiles_ = filesCount;
}

std::string Logger::GetLogPath() const
{
    return logsPath_;
}

int Logger::GetMaxFileSize() const
{
    return maxSize_;
}

int Logger::GetFilesCount() const
{
    return maxFiles_;
}

void Logger::LogInfo(const std::string& infoMessage)
{
    spdLogger_->info(infoMessage);
}

void Logger::LogDebug(const std::string& debugMessage)
{
    spdLogger_->debug(debugMessage);
}

void Logger::LogWarning(const std::string& warningMessage)
{
    spdLogger_->warn(warningMessage);
}

void Logger::LogError(const std::string& errorMessage)
{
    spdLogger_->error(errorMessage);
}

void Logger::LogCritical(const std::string& criticalMessage)
{
    spdLogger_->critical(criticalMessage);
}
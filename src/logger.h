#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/logger.h"
#include "spdlog/sinks/rotating_file_sink.h"

#include <string>

class Logger
{
  private:
    explicit Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;
    ~Logger() = default;

    std::string logsPath_;
    int maxSize_ = 1024 * 1024 * 10;
    int maxFiles_ = 3;
    // Unique_ptr would fit better for Meyers' Singleton,
    // but spdlog API uses shared_ptr for its global registry.
    std::shared_ptr<spdlog::logger> spdLogger_;

  public:
    static Logger& GetInstance();

    void SetLogPath(const std::string& filepath);
    void SetMaxFileSize(int newSize);
    void SetMaxFilesCount(int filesCount);

    std::string GetLogPath() const;
    int GetMaxFileSize() const;
    int GetFilesCount() const;

    void LogInfo(const std::string& infoMessage);
    void LogDebug(const std::string& debugMessage);
    void LogWarning(const std::string& warningMessage);
    void LogError(const std::string& errorMessage);
    void LogCritical(const std::string& criticalMessage);
};
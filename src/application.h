#pragma once

#include "cacher.h"
#include "calculator.h"
#include "logger.h"

#include <nlohmann/json.hpp>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <variant>

using json = nlohmann::json;

namespace calc
{

class Application
{
  public:
    void applicationRun(int argc, char** argv);
    void printResult() const;
    static void displayHelp();

  private:
    void initCache();
    void SetCachedResult();
    void makeTask(char** argv);
    void CacheCalculation(cache::ErrorCode ec);

    int doublePrecision_ = 6;
    std::unique_ptr<Task> calculator_ = nullptr;
    std::unique_ptr<cache::Cacher> cacher_ = nullptr;
};

} // namespace calc
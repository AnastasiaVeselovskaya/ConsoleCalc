#pragma once

#include "calculator.h"
#include "logger.h"

#include <nlohmann/json.hpp>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>

using json = nlohmann::json;

namespace calc
{
void to_json(json& jsonOutput, const calc::Task& taskInput);
void from_json(const json& jsonInput, calc::Task& taskOutput);

class Application
{
  public:
    void applicationRun(int argc, char** argv);
    void printResult() const;
    static void displayHelp();

  private:
    void makeTask(char** argv);

    int doublePrecision_ = 6;
    std::unique_ptr<Task> calculator_ = nullptr;
};

} // namespace calc
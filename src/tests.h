#pragma once

#include "application.h"

#include <cmath>

#include <gtest/gtest.h>

class ApplicationTest : public ::testing::Test
{
  protected:
    void SetUp() override;
    void TearDown() override;
    std::string CaptureCommandOutput(const char* command);
    std::string CaptureCommandError(const char* command);
    static std::string HelpOutput();

  private:
    std::unique_ptr<calc::Application> app;
    char** argv;
};

class CalculatorTest : public ::testing::Test
{
  protected:
    calc::Task task;

    void setTask(int64_t left, char op, int64_t right = 0);
};
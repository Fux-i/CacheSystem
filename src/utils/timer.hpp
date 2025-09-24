#pragma once

#include <chrono>
#include <string>

class Timer
{
  public:
    Timer(std::string name);
    ~Timer();

  private:
    using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;
    std::string name_;
    time_point  start_;
};
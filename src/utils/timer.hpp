#pragma once

#include <chrono>
#include <string>

class Timer
{
  public:
    Timer(std::string name, bool autoStop = false);
    ~Timer();

    // 获取已经过的毫秒数（不结束计时）
    double getElapsedMilliseconds() const;

    void restart();

  private:
    using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;
    std::string name_;
    bool        autoStop_;
    time_point  start_;
};
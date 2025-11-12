#pragma once

#include <chrono>
#include <iostream>
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

inline Timer::Timer(std::string name, bool autoStop) : name_(std::move(name)), autoStop_(autoStop)
{
    start_ = std::chrono::high_resolution_clock::now();
}

inline Timer::~Timer()
{
    if (autoStop_)
        return;
    using duration = std::chrono::duration<double, std::milli>;
    time_point end = std::chrono::high_resolution_clock::now();
    duration   dur = end - start_;
    std::cout << "{Timer}" << "[" << name_ << "] took " << dur.count() << " ms\n";
}

inline double Timer::getElapsedMilliseconds() const
{
    time_point end      = std::chrono::high_resolution_clock::now();
    auto       duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_);
    return static_cast<double>(duration.count());
}

inline void Timer::restart() { start_ = std::chrono::high_resolution_clock::now(); }
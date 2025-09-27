#include "timer.hpp"
#include <iostream>

Timer::Timer(std::string name, bool autoStop) : name_(std::move(name)), autoStop_(autoStop)
{
    start_ = std::chrono::high_resolution_clock::now();
}

Timer::~Timer()
{
    if (autoStop_)
        return;
    using duration = std::chrono::duration<double, std::milli>;
    time_point end = std::chrono::high_resolution_clock::now();
    duration   dur = end - start_;
    std::cout << "{Timer}"
              << "[" << name_ << "] took " << dur.count() << " ms\n";
}

double Timer::getElapsedMilliseconds() const
{
    time_point end      = std::chrono::high_resolution_clock::now();
    auto       duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_);
    return static_cast<double>(duration.count());
}

void Timer::restart() { start_ = std::chrono::high_resolution_clock::now(); }
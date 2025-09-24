#include "timer.hpp"
#include <iostream>

Timer::Timer(std::string name) : name_(std::move(name))
{
    start_ = std::chrono::high_resolution_clock::now();
}

Timer::~Timer()
{
    using duration = std::chrono::duration<double>;
    time_point end = std::chrono::high_resolution_clock::now();
    duration   dur = end - start_;
    std::cout << "{Timer}"
              << "[" << name_ << "] took " << dur.count() << " seconds\n";
}
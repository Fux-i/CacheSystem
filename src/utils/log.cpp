#pragma once
#include <iostream>

static bool DEBUG = false;

template <typename... Args>
void log(Args... args)
{
    if (DEBUG)
        ((std::cout << args), ...);
}
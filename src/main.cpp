#include "lfu/lfu.hpp"
#include "utils/log.hpp"
#include "utils/timer.cpp"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc > 1 && argv[1] != nullptr && std::string(argv[1]) == "-d")
    {
        log("DEBUG mode");
        DEBUG = true;
    }

    HashLFUCache<std::string, int> cache(50, 2, 5);
    cache.put("A", 1);
    cache.put("B", 2);
    cache.put("C", 3);
    cache.get("A");
    cache.get("A");
    cache.get("A");
    cache.get("A");
    cache.get("A");
    cache.get("A");
    cache.get("A");
    cache.get("A");
    cache.get("B");
    cache.get("C");

    return 0;
}

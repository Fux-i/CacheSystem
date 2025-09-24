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

    LFUCache<std::string, int> cache(100, 50);
    cache.put("A", 1);
    cache.put("B", 2);
    cache.put("C", 3);
    std::cout << cache.get("A") << std::endl;
    std::cout << cache.get("B") << std::endl;
    std::cout << cache.get("C") << std::endl;

    return 0;
}

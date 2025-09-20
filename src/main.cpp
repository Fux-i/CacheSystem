#include "LRU-K.cpp"
#include "log.cpp"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc > 1 && argv[1] != nullptr && std::string(argv[1]) == "-d")
    {
        log("DEBUG mode");
        DEBUG = true;
    }

    std::cout << "Hello, from CacheSystem!\n";
    LRUKCache<std::string, int> cache(2, 3, 3);

    int result;
    cache.put("a", 1);
    cache.get("a", result);

    cache.put("b", 2);
    cache.put("b", 12);
    cache.put("c", 3);
    cache.put("d", 15);

    cache.get("a", result);
    cache.get("b", result);

    cache.put("b", 4);
    cache.put("d", 8);
    cache.get("b", result);
    cache.get("d", result);
}

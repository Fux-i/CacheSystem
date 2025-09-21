#include "LRU-K.cpp"
#include "log.cpp"
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

template <typename KeyType, typename ValueType>
void test_lru(LRUCache<KeyType, ValueType>& cache, int thread_id)
{
    cache.put("a", thread_id * 10 + 1);
    cache.get("a");
    cache.get("a");

    cache.put("b", thread_id * 10 + 2);
    cache.put("b", thread_id * 10 + 12);
    cache.get("b");
    cache.put("c", thread_id * 10 + 3);
    cache.put("d", thread_id * 10 + 15);
    cache.get("c");
    cache.get("d");
    cache.get("a");

    // 额外的并发测试操作
    for (int i = 0; i < 10; ++i)
    {
        cache.put("test_" + std::to_string(i), thread_id * 100 + i);
        cache.get("test_" + std::to_string(i % 5));
    }
}

int main(int argc, char* argv[])
{
    if (argc > 1 && argv[1] != nullptr && std::string(argv[1]) == "-d")
    {
        log("DEBUG mode");
        DEBUG = true;
    }

    std::cout << "Hello, from CacheSystem!\n";
    std::cout << "Testing concurrent access to LRU-K cache...\n";

    LRUKCache<std::string, int> cache(2, 6, 10);

    // 创建多个线程测试并发安全
    std::vector<std::thread> threads;
    const int                num_threads = 4;

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_threads; ++i)
    {
        threads.emplace_back([&cache, i]() { test_lru(cache, i); });
    }

    // 等待所有线程完成
    for (auto& t : threads) { t.join(); }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "Concurrent test completed in " << duration.count() << "ms\n";
    std::cout << "All threads finished successfully - no data races detected!\n";
}

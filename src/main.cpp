#include "arc/ARC.hpp"
#include "lfu/HashLFU/HashLFU.hpp"
#include "lru/HashLRU/HashLRU.hpp"
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

    constexpr int CACHE_CAPACITY   = 3; // 使用更小的容量来更容易观察动态调整
    constexpr int MAX_AVERAGE_FREQ = 2;
    constexpr int SLICE_COUNT      = 5;

    ARCCache<std::string, int> cache(CACHE_CAPACITY, MAX_AVERAGE_FREQ);

    log("=== Phase 1: Initial insertions (filling LRU part) ===\n");
    cache.put("A", 1); // LRU: [A]
    cache.put("B", 2); // LRU: [A, B]
    cache.put("C", 3); // LRU: [A, B, C]

    log("\n=== Phase 2: Insert more to trigger evictions ===\n");
    cache.put("D", 4); // Should evict A -> LRU Ghost, LRU: [B, C, D]
    cache.put("E", 5); // Should evict B -> LRU Ghost, LRU: [C, D, E]

    log("\n=== Phase 3: Access evicted items (dynamic partition adjustment) ===\n");
    cache.get("A"); // Found in LRU Ghost -> promote to LFU, adjust capacities (LRU-1, LFU+1)
    cache.get("B"); // Found in LRU Ghost -> promote to LFU, adjust capacities (LRU-1, LFU+1)

    log("\n=== Phase 4: Continue operations to observe adjusted partitions ===\n");
    cache.put("F", 6); // New item to LRU part
    cache.get("C");    // Access existing LRU item
    cache.get("A");    // Access LFU item (should increase frequency)

    log("\n=== Phase 5: Test LFU ghost list behavior ===\n");
    cache.put("G", 7); // Trigger more evictions
    cache.put("H", 8); // Continue filling
    cache.get("A");    // If A gets evicted and we access it again, it should adjust partitions

    return 0;
}

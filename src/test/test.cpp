#include "test.hpp"
#include <array>
#include <random>

// 性能结果打印函数实现
void printPerformanceResults(const std::string& testName, int capacity,
                             const std::vector<CachePerformanceResult>& results)
{
    std::cout << "\n=== " << testName << " 性能测试结果 ===" << std::endl;
    std::cout << "缓存容量: " << capacity << std::endl;
    std::cout << std::string(100, '-') << std::endl;

    // 表头
    std::cout << std::left << "\t"
              << "算法"
              << "\t"
              << "命中率(%)"
              << "\t"
              << "命中次数"
              << "\t"
              << "总操作数"
              << "\t"
              << "执行时间(ms)"
              << "\t"
              << "吞吐量(ops/s)" << std::endl;
    std::cout << std::string(100, '-') << std::endl;

    // 输出结果
    for (const auto& result : results)
    {
        std::cout << std::left << "\t" << result.algorithmName << "\t" << std::fixed
                  << std::setprecision(2) << result.hitRate << "\t\t" << result.hitCount << "\t\t"
                  << result.totalOperations << "\t\t" << std::fixed << std::setprecision(0)
                  << result.executionTime << "\t\t" << result.throughput << std::endl;
    }
    std::cout << std::string(100, '-') << std::endl;
}

// 性能对比表格打印函数实现
void printComparisonTable(const std::string&                         testName,
                          const std::vector<CachePerformanceResult>& results)
{
    if (results.empty())
        return;

    std::cout << "\n=== " << testName << " 性能对比分析 ===" << std::endl;

    // 找出最佳性能指标
    double      bestHitRate    = 0.0;
    double      bestThroughput = 0.0;
    std::string bestHitRateAlgorithm;
    std::string bestThroughputAlgorithm;

    for (const auto& result : results)
    {
        if (result.hitRate > bestHitRate)
        {
            bestHitRate          = result.hitRate;
            bestHitRateAlgorithm = result.algorithmName;
        }
        if (result.throughput > bestThroughput)
        {
            bestThroughput          = result.throughput;
            bestThroughputAlgorithm = result.algorithmName;
        }
    }

    std::cout << "💡 最佳命中率: " << bestHitRateAlgorithm << " (" << std::fixed
              << std::setprecision(2) << bestHitRate << "%)" << std::endl;
    std::cout << "⚡ 最佳吞吐量: " << bestThroughputAlgorithm << " (" << std::fixed
              << std::setprecision(0) << bestThroughput << " ops/s)" << std::endl;
    std::cout << std::endl;
}

void testHotDataAccess()
{
    std::cout << "\n=== 测试场景1：热点数据访问测试 ===" << std::endl;

    const int CAPACITY   = 20;     // 缓存容量
    const int OPERATIONS = 500000; // 总操作次数
    const int HOT_KEYS   = 20;     // 热点数据数量
    const int COLD_KEYS  = 5000;   // 冷数据数量

    // 创建六种缓存算法实例，总容量相同
    LRUCache<int, std::string>     lru(CAPACITY); // 基础LRU
    LRUKCache<int, std::string>    lruk(2, CAPACITY,
                                     HOT_KEYS + COLD_KEYS); // k=2, 主缓存容量=CAPACITY
    HashLRUCache<int, std::string> hashLru(CAPACITY, 4);       // 分片LRU，4个分片
    LFUCache<int, std::string>     lfu(CAPACITY, 100);         // 基础LFU，maxAverageFreq=100
    HashLFUCache<int, std::string> hashLfu(CAPACITY,
                                           100,
                                           4); // 分片LFU，4个分片，降低maxAverageFreq
    ARCCache<int, std::string>     arc(CAPACITY / 2, 100); // ARC使用完整容量

    std::random_device rd;
    std::mt19937       gen(rd());

    // 基类指针指向派生类对象
    std::array<BaseCache<int, std::string>*, 6> caches =
        {&lru, &lruk, &hashLru, &lfu, &hashLfu, &arc};
    std::vector<int>         hits(6, 0);
    std::vector<int>         get_operations(6, 0);
    std::vector<double>      executionTimes(6, 0.0);
    std::vector<std::string> names = {"LRU", "LRU-K", "HashLRU", "LFU", "HashLFU", "ARC"};
    Timer                    performanceTimer("性能测量", true);

    // 为所有的缓存对象进行相同的操作序列测试
    for (size_t i = 0; i < caches.size(); ++i)
    {
        std::cout << "正在测试 " << names[i] << " 算法..." << std::endl;

        // 创建Timer进行性能监控
        Timer algorithmTimer(names[i] + " 算法");

        // 重新开始计时（用于精确测量）
        performanceTimer.restart();

        // 先预热缓存，插入一些数据
        for (int key = 0; key < HOT_KEYS; ++key)
        {
            std::string value = "value" + std::to_string(key);
            caches[i]->put(key, value);
        }

        // 交替进行put和get操作，模拟真实场景
        for (int op = 0; op < OPERATIONS; ++op)
        {
            // 大多数缓存系统中读操作比写操作频繁
            // 所以设置30%概率进行写操作
            bool isPut = (gen() % 100 < 30);
            int  key;

            // 70%概率访问热点数据，30%概率访问冷数据
            if (gen() % 100 < 70)
            {
                key = gen() % HOT_KEYS; // 热点数据
            }
            else
            {
                key = HOT_KEYS + (gen() % COLD_KEYS); // 冷数据
            }

            if (isPut)
            {
                // 执行put操作
                std::string value = "value" + std::to_string(key) + "_v" + std::to_string(op % 100);
                caches[i]->put(key, value);
            }
            else
            {
                // 执行get操作并记录命中情况
                std::string result;
                get_operations[i]++;
                if (caches[i]->get(key, result))
                {
                    hits[i]++;
                }
            }
        }

        // 记录执行时间
        executionTimes[i] = performanceTimer.getElapsedMilliseconds();
    }

    // 创建性能结果
    std::vector<CachePerformanceResult> results;
    for (size_t i = 0; i < caches.size(); ++i)
    {
        results.emplace_back(names[i], hits[i], get_operations[i], executionTimes[i]);
    }

    // 打印测试结果
    printPerformanceResults("热点数据访问测试", CAPACITY, results);
    printComparisonTable("热点数据访问测试", results);
}

void testLoopPattern()
{
    std::cout << "\n=== 测试场景2：循环扫描测试 ===" << std::endl;

    const int CAPACITY   = 50;     // 缓存容量
    const int LOOP_SIZE  = 500;    // 循环范围大小
    const int OPERATIONS = 200000; // 总操作次数

    // 创建六种缓存算法实例，总容量相同
    LRUCache<int, std::string>     lru(CAPACITY); // 基础LRU
    LRUKCache<int, std::string>    lruk(2,
                                     CAPACITY,
                                     LOOP_SIZE * 2); // k=2, 历史记录容量为循环大小的两倍
    HashLRUCache<int, std::string> hashLru(CAPACITY, 4); // 分片LRU，4个分片
    LFUCache<int, std::string>     lfu(CAPACITY, 200);   // 基础LFU，maxAverageFreq=200
    HashLFUCache<int, std::string> hashLfu(CAPACITY,
                                           200,
                                           4); // 分片LFU，4个分片，降低maxAverageFreq
    ARCCache<int, std::string>     arc(CAPACITY / 2, 200); // ARC使用完整容量

    std::array<BaseCache<int, std::string>*, 6> caches =
        {&lru, &lruk, &hashLru, &lfu, &hashLfu, &arc};
    std::vector<int>         hits(6, 0);
    std::vector<int>         get_operations(6, 0);
    std::vector<double>      executionTimes(6, 0.0);
    std::vector<std::string> names = {"LRU", "LRU-K", "HashLRU", "LFU", "HashLFU", "ARC"};
    Timer                    performanceTimer("性能测量", true);

    std::random_device rd;
    std::mt19937       gen(rd());

    // 为每种缓存算法运行相同的测试
    for (size_t i = 0; i < caches.size(); ++i)
    {
        std::cout << "正在测试 " << names[i] << " 算法..." << std::endl;

        // 创建Timer进行性能监控
        Timer algorithmTimer(names[i] + " 算法");

        // 重新开始计时（用于精确测量）
        performanceTimer.restart();

        // 先预热一部分数据（只加载20%的数据）
        for (int key = 0; key < LOOP_SIZE / 5; ++key)
        {
            std::string value = "loop" + std::to_string(key);
            caches[i]->put(key, value);
        }

        // 设置循环扫描的当前位置
        int current_pos = 0;

        // 交替进行读写操作，模拟真实场景
        for (int op = 0; op < OPERATIONS; ++op)
        {
            // 20%概率是写操作，80%概率是读操作
            bool isPut = (gen() % 100 < 20);
            int  key;

            // 按照不同模式选择键
            if (op % 100 < 60)
            { // 60%顺序扫描
                key         = current_pos;
                current_pos = (current_pos + 1) % LOOP_SIZE;
            }
            else if (op % 100 < 90)
            { // 30%随机跳跃
                key = gen() % LOOP_SIZE;
            }
            else
            { // 10%访问范围外数据
                key = LOOP_SIZE + (gen() % LOOP_SIZE);
            }

            if (isPut)
            {
                // 执行put操作，更新数据
                std::string value = "loop" + std::to_string(key) + "_v" + std::to_string(op % 100);
                caches[i]->put(key, value);
            }
            else
            {
                // 执行get操作并记录命中情况
                std::string result;
                get_operations[i]++;
                if (caches[i]->get(key, result))
                {
                    hits[i]++;
                }
            }
        }

        // 记录执行时间
        executionTimes[i] = performanceTimer.getElapsedMilliseconds();
    }

    // 创建性能结果
    std::vector<CachePerformanceResult> results;
    for (size_t i = 0; i < caches.size(); ++i)
    {
        results.emplace_back(names[i], hits[i], get_operations[i], executionTimes[i]);
    }

    // 打印测试结果
    printPerformanceResults("循环扫描测试", CAPACITY, results);
    printComparisonTable("循环扫描测试", results);
}

void testWorkloadShift()
{
    std::cout << "\n=== 测试场景3：工作负载剧烈变化测试 ===" << std::endl;

    const int CAPACITY     = 30;             // 缓存容量
    const int OPERATIONS   = 8000;           // 减少操作次数以避免潜在的死循环
    const int PHASE_LENGTH = OPERATIONS / 5; // 每个阶段的长度

    // 创建六种缓存算法实例，总容量相同
    LRUCache<int, std::string>     lru(CAPACITY);          // 基础LRU
    LRUKCache<int, std::string>    lruk(2, CAPACITY, 500); // k=2, 历史记录容量500
    HashLRUCache<int, std::string> hashLru(CAPACITY, 4);   // 分片LRU，4个分片
    LFUCache<int, std::string>     lfu(CAPACITY, 300);     // 基础LFU，maxAverageFreq=300
    HashLFUCache<int, std::string> hashLfu(CAPACITY,
                                           300,
                                           4); // 分片LFU，4个分片，降低maxAverageFreq
    ARCCache<int, std::string>     arc(CAPACITY / 2, 300); // ARC使用完整容量

    std::random_device                          rd;
    std::mt19937                                gen(rd());
    std::array<BaseCache<int, std::string>*, 6> caches =
        {&lru, &lruk, &hashLru, &lfu, &hashLfu, &arc};
    std::vector<int>         hits(6, 0);
    std::vector<int>         get_operations(6, 0);
    std::vector<double>      executionTimes(6, 0.0);
    std::vector<std::string> names = {"LRU", "LRU-K", "HashLRU", "LFU", "HashLFU", "ARC"};
    Timer                    performanceTimer("性能测量", true);

    // 为每种缓存算法运行相同的测试
    for (size_t i = 0; i < caches.size(); ++i)
    {
        std::cout << "正在测试 " << names[i] << " 算法..." << std::endl;

        // 创建Timer进行性能监控
        Timer algorithmTimer(names[i] + " 算法");

        // 重新开始计时（用于精确测量）
        performanceTimer.restart();

        // 先预热缓存，只插入少量初始数据
        for (int key = 0; key < 30; ++key)
        {
            std::string value = "init" + std::to_string(key);
            caches[i]->put(key, value);
        }

        // 进行多阶段测试，每个阶段有不同的访问模式
        for (int op = 0; op < OPERATIONS; ++op)
        {
            // 确定当前阶段
            int phase = op / PHASE_LENGTH;

            // 每个阶段的读写比例不同
            int putProbability;
            switch (phase)
            {
            case 0: putProbability = 15; break; // 阶段1: 热点访问，15%写入更合理
            case 1: putProbability = 30; break; // 阶段2: 大范围随机，写比例为30%
            case 2: putProbability = 10; break; // 阶段3: 顺序扫描，10%写入保持不变
            case 3: putProbability = 25; break; // 阶段4: 局部性随机，微调为25%
            case 4: putProbability = 20; break; // 阶段5: 混合访问，调整为20%
            default: putProbability = 20;
            }

            // 确定是读还是写操作
            bool isPut = (gen() % 100 < putProbability);

            // 根据不同阶段选择不同的访问模式生成key - 优化后的访问范围
            int key;
            if (op < PHASE_LENGTH)
            { // 阶段1: 热点访问 - 热点数量5，使热点更集中
                key = gen() % 5;
            }
            else if (op < PHASE_LENGTH * 2)
            { // 阶段2: 大范围随机 - 范围400，更适合30大小的缓存
                key = gen() % 400;
            }
            else if (op < PHASE_LENGTH * 3)
            { // 阶段3: 顺序扫描 - 保持100个键
                key = (op - PHASE_LENGTH * 2) % 100;
            }
            else if (op < PHASE_LENGTH * 4)
            { // 阶段4: 局部性随机 - 优化局部性区域大小
                // 产生5个局部区域，每个区域大小为15个键，与缓存大小20接近但略小
                int locality = (op / 800) % 5;               // 调整为5个局部区域
                key          = locality * 15 + (gen() % 15); // 每区域15个键
            }
            else
            { // 阶段5: 混合访问 - 增加热点访问比例
                int r = gen() % 100;
                if (r < 40)
                {                    // 40%概率访问热点（从30%增加）
                    key = gen() % 5; // 5个热点键
                }
                else if (r < 70)
                {                           // 30%概率访问中等范围
                    key = 5 + (gen() % 45); // 缩小中等范围为50个键
                }
                else
                {                             // 30%概率访问大范围（从40%减少）
                    key = 50 + (gen() % 350); // 大范围也相应缩小
                }
            }

            if (isPut)
            {
                // 执行写操作
                std::string value = "value" + std::to_string(key) + "_p" + std::to_string(phase);
                caches[i]->put(key, value);
            }
            else
            {
                // 执行读操作并记录命中情况
                std::string result;
                get_operations[i]++;
                if (caches[i]->get(key, result))
                {
                    hits[i]++;
                }
            }
        }

        // 记录执行时间
        executionTimes[i] = performanceTimer.getElapsedMilliseconds();
    }

    // 创建性能结果
    std::vector<CachePerformanceResult> results;
    for (size_t i = 0; i < caches.size(); ++i)
    {
        results.emplace_back(names[i], hits[i], get_operations[i], executionTimes[i]);
    }

    // 打印测试结果
    printPerformanceResults("工作负载剧烈变化测试", CAPACITY, results);
    printComparisonTable("工作负载剧烈变化测试", results);
}

// 综合性能测试函数
void runAllPerformanceTests()
{
    std::cout << "\n🚀 开始缓存系统综合性能测试 🚀" << std::endl;
    std::cout << "本次测试将对比 LRU、LRU-K(k=2)、HashLRU、LFU、HashLFU、ARC 六种缓存淘汰策略"
              << std::endl;
    std::cout << "测试包含三个场景：热点数据访问、循环扫描、工作负载变化" << std::endl;
    std::cout << std::string(120, '=') << std::endl;

    Timer totalTimer("综合性能测试", true);

    try
    {
        testHotDataAccess();
        testLoopPattern();
        testWorkloadShift();
    }
    catch (const std::exception& e)
    {
        std::cerr << "❌ 测试过程中发生错误: " << e.what() << std::endl;
        return;
    }

    auto totalDuration = totalTimer.getElapsedMilliseconds();

    std::cout << "\n" << std::string(100, '=') << std::endl;
    std::cout << "🎉 缓存系统综合性能测试完成！" << std::endl;
    std::cout << "总耗时: " << totalDuration << " 毫秒" << std::endl;
    std::cout << std::string(100, '=') << std::endl;
}
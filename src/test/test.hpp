#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "../arc/arc.hpp"
#include "../lfu/lfu.hpp"
#include "../lru/lru.hpp"
#include "../utils/timer.hpp"

// 性能测试结果结构体
struct CachePerformanceResult
{
    std::string algorithmName;   // 算法名称
    int         hitCount;        // 命中次数
    int         totalOperations; // 总操作次数
    double      hitRate;         // 命中率(%)
    double      executionTime;   // 执行时间(毫秒)
    double      throughput;      // 吞吐量(操作/秒)

    CachePerformanceResult(std::string name = "", int hits = 0, int total = 0, double time = 0.0)
        : algorithmName(std::move(name))
        , hitCount(hits)
        , totalOperations(total)
        , executionTime(time)
        , hitRate(total > 0 ? (100.0 * hits / total) : 0.0)
        , throughput(time > 0 ? (total * 1000.0 / time) : 0.0)
    {
    }
};

// 测试结果打印函数
void printPerformanceResults(const std::string& testName, int capacity,
                             const std::vector<CachePerformanceResult>& results);

// 性能对比表格打印函数
void printComparisonTable(const std::string&                         testName,
                          const std::vector<CachePerformanceResult>& results);

// 测试函数声明
void testHotDataAccess();
void testLoopPattern();
void testWorkloadShift();

// 综合性能测试函数
void runAllPerformanceTests();

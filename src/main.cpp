#include "test/test.hpp"
#include "utils/log.hpp"

#include <iostream>

int main(int argc, char* argv[])
{
    // 检查调试模式
    if (argc > 1 && std::string(argv[1]) == "-d")
    {
        log("启用调试模式");
        DEBUG = true;
    }
    else
        DEBUG = false;

    std::cout << "🔧 缓存系统性能测试程序" << std::endl;
    std::cout << "\n🎯 本程序将对比以下6种缓存淘汰算法的性能表现:" << std::endl;
    std::cout << "\n📋 测试指标包括:" << std::endl;
    std::cout << "  • 命中率 (Hit Rate) - 缓存命中的百分比" << std::endl;
    std::cout << "  • 执行时间 (Execution Time) - 算法执行耗时" << std::endl;
    std::cout << "  • 吞吐量 (Throughput) - 每秒处理的操作数" << std::endl;

    // 运行综合性能测试
    runAllPerformanceTests();

    std::cout << "\n✅ 程序执行完成！" << std::endl;

    return 0;
}

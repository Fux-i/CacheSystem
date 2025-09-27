cmake_minimum_required(VERSION 3.10.0)
project(CacheSystem VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

# 设置头文件包含目录
include_directories(${CMAKE_SOURCE_DIR})

# 定义源文件
set(SRC
    src/main.cpp
    src/test/test.cpp
    src/utils/timer.cpp
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SRC})

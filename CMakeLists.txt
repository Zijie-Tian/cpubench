cmake_minimum_required(VERSION 3.16)
project(cpubench LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 强制使用Clang编译器
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    message(WARNING "推荐使用Clang编译器，当前编译器: ${CMAKE_CXX_COMPILER_ID}")
endif()

# 检测处理器架构
if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
    set(ARM64 TRUE)
else()
    message(FATAL_ERROR "此项目仅支持ARM64架构")
endif()

# 平台检测
if(APPLE)
    set(PLATFORM "apple")
    message(STATUS "检测到Apple平台 (M系列芯片)")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND EXISTS "/etc/nv_tegra_release")
    set(PLATFORM "jetson")
    message(STATUS "检测到NVIDIA Jetson平台")
else()
    set(PLATFORM "generic")
    message(STATUS "通用ARM64平台")
endif()

# 设置默认构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 通用编译选项
add_compile_options(-O3 -Wall -Wextra)

# 平台特定的架构标志
if(PLATFORM STREQUAL "apple")
    # Apple M4支持ARMv9，但为了兼容性使用ARMv8.4+fp16
    add_compile_options(-march=armv8.4-a+fp16)
elseif(PLATFORM STREQUAL "jetson")
    # NVIDIA AGX Orin使用ARMv8.2-A
    add_compile_options(-march=armv8.2-a+fp16)
else()
    # 通用ARM64最小配置
    add_compile_options(-march=armv8-a)
endif()

# 可配置的benchmark参数
set(BENCH_ITERS "20000000" CACHE STRING "迭代次数")
set(BENCH_UNROLL "32" CACHE STRING "循环展开倍数")

# 添加编译定义
add_compile_definitions(
    ITERS=${BENCH_ITERS}
    UNROLL=${BENCH_UNROLL}
)

# 输出构建信息
message(STATUS "========================================")
message(STATUS "构建配置:")
message(STATUS "  平台: ${PLATFORM}")
message(STATUS "  编译器: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  构建类型: ${CMAKE_BUILD_TYPE}")
message(STATUS "  迭代次数: ${BENCH_ITERS}")
message(STATUS "  展开倍数: ${BENCH_UNROLL}")
message(STATUS "========================================")

# 添加子目录
add_subdirectory(benchmarks/fp16)
add_subdirectory(benchmarks/tbl)
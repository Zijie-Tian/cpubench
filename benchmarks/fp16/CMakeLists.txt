# FP16 向量加法基准测试

# 根据平台选择要编译的目标
if(PLATFORM STREQUAL "apple")
    # Apple平台只编译M4版本
    add_executable(fp16_add_bench_m4 fp16_add_bench_m4.cpp)
    target_compile_definitions(fp16_add_bench_m4 PRIVATE
        ITERS=${BENCH_ITERS}
        UNROLL=${BENCH_UNROLL}
    )
    
    # 创建通用别名
    add_executable(fp16_add_bench fp16_add_bench_m4.cpp)
    target_compile_definitions(fp16_add_bench PRIVATE
        ITERS=${BENCH_ITERS}
        UNROLL=${BENCH_UNROLL}
    )
    
    # 安装目标
    install(TARGETS fp16_add_bench fp16_add_bench_m4
        RUNTIME DESTINATION bin
    )
    
elseif(PLATFORM STREQUAL "jetson")
    # Jetson平台只编译Orin版本
    add_executable(fp16_add_bench_orin fp16_add_bench_orin.cpp)
    target_compile_definitions(fp16_add_bench_orin PRIVATE
        ITERS=${BENCH_ITERS}
        UNROLL=${BENCH_UNROLL}
    )
    
    # 创建通用别名
    add_executable(fp16_add_bench fp16_add_bench_orin.cpp)
    target_compile_definitions(fp16_add_bench PRIVATE
        ITERS=${BENCH_ITERS}
        UNROLL=${BENCH_UNROLL}
    )
    
    # 安装目标
    install(TARGETS fp16_add_bench fp16_add_bench_orin
        RUNTIME DESTINATION bin
    )
    
else()
    # 通用平台：同时编译两个版本，让用户选择
    message(STATUS "通用平台：编译所有FP16版本")
    
    # Apple M4版本
    add_executable(fp16_add_bench_m4 fp16_add_bench_m4.cpp)
    target_compile_definitions(fp16_add_bench_m4 PRIVATE
        ITERS=${BENCH_ITERS}
        UNROLL=${BENCH_UNROLL}
    )
    
    # NVIDIA Jetson Orin版本
    add_executable(fp16_add_bench_orin fp16_add_bench_orin.cpp)
    target_compile_definitions(fp16_add_bench_orin PRIVATE
        ITERS=${BENCH_ITERS}
        UNROLL=${BENCH_UNROLL}
    )
    
    # 默认使用M4版本作为通用版本
    add_executable(fp16_add_bench fp16_add_bench_m4.cpp)
    target_compile_definitions(fp16_add_bench PRIVATE
        ITERS=${BENCH_ITERS}
        UNROLL=${BENCH_UNROLL}
    )
    
    # 安装目标
    install(TARGETS fp16_add_bench fp16_add_bench_m4 fp16_add_bench_orin
        RUNTIME DESTINATION bin
    )
endif()
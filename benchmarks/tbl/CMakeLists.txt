# TBL (Table Lookup) 吞吐量基准测试

# 无符号8位版本
add_executable(tbl_u8 tbl_throughput.cpp)
target_compile_definitions(tbl_u8 PRIVATE
    TBL_OP='u'
    ITERS=${BENCH_ITERS}
    UNROLL=${BENCH_UNROLL}
)

# 有符号8位版本
add_executable(tbl_s8 tbl_throughput.cpp)
target_compile_definitions(tbl_s8 PRIVATE
    TBL_OP='s'
    ITERS=${BENCH_ITERS}
    UNROLL=${BENCH_UNROLL}
)

# 通用TBL测试（默认使用无符号）
add_executable(tbl_throughput tbl_throughput.cpp)
target_compile_definitions(tbl_throughput PRIVATE
    TBL_OP='u'
    ITERS=${BENCH_ITERS}
    UNROLL=${BENCH_UNROLL}
)

# 安装目标
install(TARGETS tbl_u8 tbl_s8 tbl_throughput
    RUNTIME DESTINATION bin
)
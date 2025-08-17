# CPU Benchmark Suite

ARM NEON指令吞吐量微基准测试套件，专门用于测试FP16运算和TBL指令的峰值性能。

## 支持平台

- **Apple Silicon** (M系列芯片): ARMv8.4-A + FP16扩展
- **NVIDIA Jetson AGX Orin**: ARMv8.2-A + FP16扩展
- 其他ARM64平台（需要NEON支持）

## 基准测试项目

### 1. FP16向量加法 (benchmarks/fp16/)
- `fp16_add_bench_m4.cpp`: 针对Apple M4优化（4路累加器）
- `fp16_add_bench_orin.cpp`: 针对Jetson Orin优化（2路累加器）
- 测量FP16 vaddq_f16指令的峰值吞吐量

### 2. TBL查表指令 (benchmarks/tbl/)
- `tbl_throughput.cpp`: 测试vqtbl1q_u8/s8指令吞吐量
- 支持无符号(u8)和有符号(s8)两种变体

## 快速开始

### 使用CMake构建（推荐）

```bash
# 配置（使用Clang编译器）
CC=clang CXX=clang++ cmake -B build

# 或者带自定义参数
CC=clang CXX=clang++ cmake -B build -DBENCH_ITERS=50000000 -DBENCH_UNROLL=64

# 编译
cmake --build build -j

# 运行测试
./build/benchmarks/fp16/fp16_add_bench
./build/benchmarks/tbl/tbl_u8
```

### 直接编译

#### Apple Silicon (M4)
```bash
clang++ -std=c++17 -O3 -march=armv8.4-a+fp16 \
    benchmarks/fp16/fp16_add_bench_m4.cpp \
    -o fp16_add_bench
```

#### NVIDIA Jetson AGX Orin
```bash
clang++ -std=c++17 -O3 -march=armv8.2-a+fp16 \
    benchmarks/fp16/fp16_add_bench_orin.cpp \
    -o fp16_add_bench
```

#### TBL测试
```bash
# 无符号版本
clang++ -std=c++17 -O3 -march=armv8-a -DTBL_OP='u' \
    benchmarks/tbl/tbl_throughput.cpp \
    -o tbl_u8

# 有符号版本  
clang++ -std=c++17 -O3 -march=armv8-a -DTBL_OP='s' \
    benchmarks/tbl/tbl_throughput.cpp \
    -o tbl_s8
```

## 可配置参数

编译时可通过宏定义调整：

- `ITERS`: 外层循环迭代次数（默认20000000）
- `UNROLL`: 内层循环展开倍数（默认32）
- `TBL_OP`: TBL操作类型，'u'表示无符号，'s'表示有符号

示例：
```bash
# CMake方式
cmake -B build -DBENCH_ITERS=50000000 -DBENCH_UNROLL=64

# 直接编译方式
clang++ -DITERS=50000000 -DUNROLL=64 ...
```

## 输出说明

每个基准测试会输出：
- **ITERS**: 实际执行的迭代次数
- **UNROLL**: 循环展开倍数
- **Time**: 总执行时间（秒）
- **Executed**: 理论执行的操作数（GFLOPs或GOPS）
- **Throughput**: 吞吐量（GFLOPs/s或GOPS/s）

## 注意事项

1. **编译器选择**: 强烈推荐使用Clang/Clang++，它对ARM NEON有更好的优化
2. **架构标志**: 
   - Apple M4虽支持ARMv9，但代码使用ARMv8.4-a+fp16以确保兼容性
   - Jetson AGX Orin使用ARMv8.2-a+fp16
3. **性能优化**: 代码使用多累加器、循环展开和周期性内存屏障来达到峰值性能

## 目录结构

```
cpubench/
├── CMakeLists.txt           # 主CMake配置
├── README.md                 # 本文档
├── CLAUDE.md                 # Claude Code指导文档
├── benchmarks/
│   ├── fp16/                # FP16基准测试
│   │   ├── CMakeLists.txt
│   │   ├── fp16_add_bench_m4.cpp
│   │   └── fp16_add_bench_orin.cpp
│   └── tbl/                  # TBL基准测试
│       ├── CMakeLists.txt
│       └── tbl_throughput.cpp
└── build/                    # 构建目录（gitignore）
```

## 开发者信息

这是一个用于测试ARM NEON指令吞吐量的微基准测试集合，主要关注：
- 验证硬件峰值性能
- 测试编译器优化效果
- 评估不同循环展开策略的影响
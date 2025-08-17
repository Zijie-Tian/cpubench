# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a CPU benchmark repository focused on ARM NEON instruction throughput testing, specifically:
- FP16 (half-precision floating point) vector addition benchmarks
- TBL (table lookup) instruction throughput measurements
- Optimized for Apple Silicon (M-series) and NVIDIA Jetson AGX Orin platforms

## Repository Structure

```
cpubench/
├── CMakeLists.txt              # Main CMake configuration
├── benchmarks/
│   ├── fp16/                   # FP16 benchmarks
│   │   ├── fp16_add_bench_m4.cpp    # Apple M4 optimized
│   │   └── fp16_add_bench_orin.cpp  # Jetson Orin optimized
│   └── tbl/                    # TBL benchmarks
│       └── tbl_throughput.cpp        # TBL instruction throughput
└── build/                      # Build directory (gitignored)
```

## Build System

The project now uses CMake as the primary build system. Always prefer using Clang/Clang++ compiler.

### Quick Build with CMake

```bash
# Setup build directory
mkdir build && cd build

# Configure with Clang (REQUIRED)
CC=clang CXX=clang++ cmake ..

# Build all benchmarks
make -j

# Run benchmarks
./benchmarks/fp16/fp16_add_bench
./benchmarks/tbl/tbl_u8
```

### CMake Configuration Options

```bash
# Custom iteration count and unroll factor
CC=clang CXX=clang++ cmake .. -DBENCH_ITERS=50000000 -DBENCH_UNROLL=64

# Debug build
CC=clang CXX=clang++ cmake .. -DCMAKE_BUILD_TYPE=Debug
```

### Direct Compilation (Fallback)

If CMake is not available, use these commands:

**FP16 Benchmarks**:
```bash
# Apple Silicon
clang++ -std=c++17 -O3 -march=armv8.4-a+fp16 benchmarks/fp16/fp16_add_bench_m4.cpp -o fp16_add_bench

# Jetson AGX Orin  
clang++ -std=c++17 -O3 -march=armv8.2-a+fp16 benchmarks/fp16/fp16_add_bench_orin.cpp -o fp16_add_bench
```

**TBL Benchmarks**:
```bash
# Unsigned version
clang++ -std=c++17 -O3 -march=armv8-a -DTBL_OP='u' benchmarks/tbl/tbl_throughput.cpp -o tbl_u8

# Signed version
clang++ -std=c++17 -O3 -march=armv8-a -DTBL_OP='s' benchmarks/tbl/tbl_throughput.cpp -o tbl_s8
```

## Key Architecture Patterns

### Benchmark Structure
All benchmarks follow a similar pattern:
1. **Configurable Parameters**: `ITERS` (iterations) and `UNROLL` (loop unrolling factor) can be set at compile time
2. **Multiple Accumulators**: Use 2-4 vector accumulators to maximize instruction-level parallelism
3. **Periodic Sinks**: Volatile writes every 1024 iterations prevent compiler optimizations from eliminating the computation
4. **Timing**: Uses `std::chrono::steady_clock` or `high_resolution_clock` for precise measurements

### Platform-Specific Optimizations
- **Apple Silicon (M4)**: Uses 4-way accumulators with aggressive unrolling (default 8x)
- **Jetson Orin**: Uses 2-way accumulators optimized for dual-pipeline execution
- Both use ARM NEON intrinsics (`arm_neon.h`) for vectorized operations

### Performance Metrics
Benchmarks calculate and report:
- Total execution time in seconds
- Theoretical FLOPs or OPS executed
- Throughput in GFLOPs/s or GOPS/s

## Development Notes

- Code uses Chinese comments explaining optimization rationale
- Benchmarks are designed to measure peak theoretical throughput of specific instructions
- The `volatile` keyword and inline assembly barriers prevent unwanted compiler optimizations
- All source files are standalone C++ programs with no external dependencies beyond standard library and ARM NEON
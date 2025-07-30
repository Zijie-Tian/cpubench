// fp16_add_bench_orin.cpp
// 适用：Jetson Orin (Arm v8.2-A), NEON FP16, 单线程基准
#include <arm_neon.h>
#include <chrono>
#include <iostream>

#ifndef ITERS
#define ITERS 20000000UL      // 2e7 × UNROLL × 8 ≈ 5.12e9 FLOPs (@UNROLL=32)
#endif
#ifndef UNROLL
#define UNROLL 1            // 必须是 2 或 4 的倍数；32 已能吃满管线
#endif

static inline float16x8_t fadd(float16x8_t x, float16x8_t y) {
    return vaddq_f16(x, y);
}

__attribute__((noinline))
float16x8_t kernel() {
    float16x8_t acc0 = vdupq_n_f16(0.0f);
    float16x8_t acc1 = acc0;          // 双累加器 → 填满 2 管线

    for (size_t i = 0; i < ITERS; ++i) {

#pragma clang loop unroll_count(UNROLL)
        for (int u = 0; u < UNROLL; u += 2) {
            float16_t v = static_cast<float16_t>((i + u) & 0x7FF);   // 变化数据
            float16x8_t a = vdupq_n_f16(v);

            acc0 = fadd(acc0, a);
            acc1 = fadd(acc1, a);
        }

        // 周期性写回防掉指令
        if ((i & 1023) == 0) {
            volatile float16_t sink = vgetq_lane_f16(acc0, 0);
            (void)sink;
        }
    }
    return vaddq_f16(acc0, acc1);
}

int main() {
    auto t0 = std::chrono::steady_clock::now();
    volatile float16x8_t res = kernel();
    (void)res;
    auto t1 = std::chrono::steady_clock::now();

    double secs = std::chrono::duration<double>(t1 - t0).count();
    double flops = static_cast<double>(ITERS) * UNROLL * 8;   // 8 FLOPs / vaddq_f16

    std::cout << "ITERS      = " << ITERS  << '\n'
              << "UNROLL     = " << UNROLL << '\n'
              << "Time       = " << secs   << " s\n"
              << "Executed   = " << flops / 1e9 << " GFLOPs (theoretical)\n"
              << "Throughput = " << flops / secs / 1e9 << " GFLOPs/s\n";
}


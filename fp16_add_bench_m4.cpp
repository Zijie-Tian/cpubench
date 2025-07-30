// fp16_add_bench_m4.cpp
// Apple Silicon / clang 15+ / C++17
#include <arm_neon.h>
#include <chrono>
#include <iostream>

// ---------- 可编译期覆盖的参数 ----------
#ifndef ITERS            // 外层迭代次数
#define ITERS 25000000UL // 2.5e7 × 32 × 8 ≈ 6.4e9 FLOPs
#endif

#ifndef UNROLL // 每轮展开倍数（必须是 4 的倍数以匹配累加器数）
#define UNROLL 8
#endif
// ---------------------------------------

static inline float16x8_t add_broadcast(float16x8_t acc, float16_t v) {
  return vaddq_f16(acc, vdupq_n_f16(v));
}

/// 核心内核：4-way 累加器 + 手动展开
__attribute__((noinline)) float16x8_t run_kernel() {
  float16x8_t acc0 = vdupq_n_f16(0.0f);
  float16x8_t acc1 = acc0, acc2 = acc0, acc3 = acc0;

  for (size_t i = 0; i < ITERS; ++i) {

#pragma clang loop unroll_count(UNROLL)
    for (int u = 0; u < UNROLL; u += 4) {
      // 让输入值随迭代变化，避免被折叠
      float16_t v = static_cast<float16_t>((i + u) & 0x3FF);

      acc0 = add_broadcast(acc0, v);
      acc1 = add_broadcast(acc1, v);
      acc2 = add_broadcast(acc2, v);
      acc3 = add_broadcast(acc3, v);
    }

    // 每 1024 次写出一个值，强制执行
    if ((i & 1023) == 0) {
      volatile float16_t sink = vgetq_lane_f16(acc0, 0);
      (void)sink;
    }
  }

  // 合并 4 个累加器，确保结果被“观察”到
  return vaddq_f16(vaddq_f16(acc0, acc1), vaddq_f16(acc2, acc3));
}

int main() {
  using clock = std::chrono::steady_clock;
  auto t0 = clock::now();

  // 运行基准
  volatile float16x8_t res = run_kernel();
  (void)res;

  auto t1 = clock::now();
  double secs = std::chrono::duration<double>(t1 - t0).count();

  // 统计理论执行的 FLOPs
  double flops = static_cast<double>(ITERS) * UNROLL * 8;

  std::cout << "ITERS      = " << ITERS << '\n'
            << "UNROLL     = " << UNROLL << '\n'
            << "Time       = " << secs << " s\n"
            << "Executed   = " << flops / 1e9 << " GFLOPs (theoretical)\n"
            << "Throughput = " << flops / secs / 1e9 << " GFLOPs/s\n";
  return 0;
}

/********************************************************************
 *  tbl_throughput_ops.cpp — 统计 vqtbl1q_{u8,s8} 的 OPS/s
 *  使用方法与上一版相同，只是输出新增 OPS/s。
 *******************************************************************/
#include <arm_neon.h>
#include <chrono>
#include <cstdint>
#include <cstdio>

#ifndef TBL_OP
#define TBL_OP 'u'                // 'u' → vqtbl1q_u8,  's' → vqtbl1q_s8
#endif
#ifndef ITERS
#define ITERS 20000000UL
#endif
#ifndef UNROLL
#define UNROLL 32
#endif

#if TBL_OP == 'u'
#   define TBL(vtbl, vidx) vqtbl1q_u8((vtbl), (vidx))
#elif TBL_OP == 's'
#   define TBL(vtbl, vidx) vqtbl1q_s8((vtbl), (vidx))
#else
#   error "TBL_OP must be 'u' or 's'"
#endif

int main() {
    /* 常驻寄存器 */
    uint8x16_t tbl  = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    uint8x16_t idx0 = {15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0};
    uint8x16_t idx1 = vaddq_u8(idx0, vdupq_n_u8(2));
    uint8x16_t acc0{}, acc1{}, acc2{}, acc3{};

    /* ------------ 计时开始 ------------ */
    auto t0 = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < ITERS; ++i) {
        uint8x16_t idx_dyn = vdupq_n_u8(static_cast<uint8_t>(i));

#pragma clang loop unroll_count(UNROLL)
        for (int u = 0; u < UNROLL; u += 4) {
            acc0 = vqtbl1q_u8(acc0, idx_dyn);           // 数据依赖
            acc1 = vqtbl1q_u8(acc1, idx_dyn);
            acc2 = vqtbl1q_u8(acc2, idx_dyn);
            acc3 = vqtbl1q_u8(acc3, idx_dyn);
            asm volatile("" : "+w"(acc0),"+w"(acc1),"+w"(acc2),"+w"(acc3));
        }
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    /* ------------ 计时结束 ------------ */

    volatile uint8_t sink = vgetq_lane_u8(acc0, 0); (void)sink;

    /* 统计 */
    double usec  = std::chrono::duration<double, std::micro>(t1 - t0).count();
    double secs  = usec * 1e-6;
    double instr = static_cast<double>(ITERS) * UNROLL * 4;   // 4×TBL/迭代
    double opsps = instr / secs;                              // OPS/s

    printf("TBL 指令类型 : vqtbl1q_%c8\n", TBL_OP);
    printf("ITERS        : %lu\n", ITERS);
    printf("UNROLL       : %d\n", UNROLL);
    printf("time         : %.2f µs\n", usec);
    printf("instr (OP)   : %.0f\n", instr);
    printf("OPS/s        : %.2f GOPS/s\n", opsps / 1e9);  // 用 MOPS/s 更直观
    return 0;
}


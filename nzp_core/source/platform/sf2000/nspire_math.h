#ifndef NSPIRE_MATH_H
#define NSPIRE_MATH_H

#include <stdint.h>

static inline unsigned int udiv_fast_32_32_incorrect(unsigned int d, unsigned int n) {
    if (d == 0) return 0xffffffff;
    return n / d;
}

static inline unsigned int udiv_64_32(unsigned long long n, unsigned int d) {
    if (d == 0) return 0xffffffff;
    return (unsigned int)(n / d);
}

static inline unsigned int udiv_s31_32(unsigned int d, unsigned int n) {
    unsigned long long result;
    if (d == 0) return 0x7fffffff;
    result = ((unsigned long long)n << 31) / d;
    if (result > 0x7fffffff) return 0x7fffffff;
    return (unsigned int)result;
}

static inline long long mul_64_32_r64(long long m0, int m1) {
    return m0 * (long long)m1;
}

static inline long long llmull_s0(int m0, int m1) {
    return (long long)m0 * (long long)m1;
}

static inline int llmull_s16(int m0, int m1) {
    return (int)(((long long)m0 * (long long)m1) >> 16);
}

static inline int llmull_s21(int m0, int m1) {
    return (int)(((long long)m0 * (long long)m1) >> 21);
}

static inline int llmull_s24(int m0, int m1) {
    return (int)(((long long)m0 * (long long)m1) >> 24);
}

static inline int llmull_s27(int m0, int m1) {
    return (int)(((long long)m0 * (long long)m1) >> 27);
}

static inline int llmull_s28(int m0, int m1) {
    return (int)(((long long)m0 * (long long)m1) >> 28);
}

typedef long long fixed32_t;
extern void test_float32_shift(float *f, int shift);

#define CALCG_FIXED_FTOF32(f, f32) \
do { \
    float temp = (f); \
    test_float32_shift(&temp, 32); \
    (f32) = (fixed32_t)(temp); \
} while(0)

#define FIXED_FIXEDTOFLOAT(in_fixed, out_float, frac) \
do { \
    float temp = (float)(in_fixed); \
    test_float32_shift(&temp, -(frac)); \
    (out_float) = temp; \
} while(0)

#define FIXED_FLOATTOFIXED(in_float, out_fixed, frac) \
do { \
    float temp = (in_float); \
    test_float32_shift(&temp, (frac)); \
    (out_fixed) = (fixed16_t)(temp); \
} while(0)

#define FIXED_SHIFT_FLOAT(in_float, out_float, shift) \
do { \
    (out_float) = (in_float); \
    test_float32_shift(&(out_float), (shift)); \
} while(0)

#endif

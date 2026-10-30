#include <stddef.h>
#include <stdint.h>
#include "extra.h"

#ifndef USE_SHIFT_MULT
#define USE_SHIFT_MULT 1
#endif

#if (USE_SHIFT_MULT != 0) && (USE_SHIFT_MULT != 1)
#error "USE_SHIFT_MULT must be 0 or 1"
#endif

size_t mult_const_SHAKE128_RATE(size_t in)
{
    /* SHAKE128_RATE = 168 */
#if USE_SHIFT_MULT
    return (in << 7) + (in << 5) + (in << 3);
#else
    return in * (size_t)168;
#endif
}

size_t mult_const_SHAKE256_RATE(size_t in)
{
#if USE_SHIFT_MULT
    return (in << 7) + (in << 3);
#else
    return in * (size_t)136;
#endif
}

int64_t binary_mult(int32_t a, int32_t b)
{
#if USE_SHIFT_MULT
    /* Magnitudes are computed in int64_t to handle INT32_MIN safely. */
    const int negative = (a < 0) != (b < 0);
    const uint64_t ua = (a < 0) ? (uint64_t)(-(int64_t)a) : (uint64_t)a;
    uint32_t ub = (b < 0) ? (uint32_t)(-(int64_t)b) : (uint32_t)b;
    uint64_t result = 0;
    uint64_t multiplicand = ua;

    while (ub != 0) {
        if ((ub & 1u) != 0u) {
            result += multiplicand;
        }
        multiplicand <<= 1;
        ub >>= 1;
    }
    return negative ? -(int64_t)result : (int64_t)result;
#else
    return (int64_t)a * (int64_t)b;
#endif
}

unsigned int mult_const_POLYT1_PACKEDBYTES(unsigned int in)
{
#if USE_SHIFT_MULT
    return (in << 8) + (in << 6);
#else
    return in * 320u;
#endif
}

unsigned int mult_const_POLYETA_PACKEDBYTES(unsigned int param, unsigned int in)
{
    unsigned int tmp = 0;
    if (param == 96u) {
#if USE_SHIFT_MULT
        tmp = (in << 6) + (in << 5);
#else
        tmp = in * 96u;
#endif
    }
    if (param == 128u) {
#if USE_SHIFT_MULT
        tmp = (in << 7);
#else
        tmp = in * 128u;
#endif
    }
    return tmp;
}

unsigned int mult_const_POLYT0_PACKEDBYTES(unsigned int in)
{
#if USE_SHIFT_MULT
    return (in << 8) + (in << 7) + (in << 5);
#else
    return in * 416u;
#endif
}

unsigned int mult_const_POLYZ_PACKEDBYTES(unsigned int param, unsigned int in)
{
    unsigned int tmp = 0;
    if (param == 576u) {
#if USE_SHIFT_MULT
        tmp = (in << 9) + (in << 6);
#else
        tmp = in * 576u;
#endif
    }
    if (param == 640u) {
#if USE_SHIFT_MULT
        tmp = (in << 9) + (in << 7);
#else
        tmp = in * 640u;
#endif
    }
    return tmp;
}

unsigned int mod3_u32(unsigned int x)
{
    while (x > 3u) {
        x = (x >> 2) + (x & 3u);
    }
    if (x == 3u) {
        return 0;
    }
    return x;
}

uint32_t mult_const_205(uint32_t in)
{
#if USE_SHIFT_MULT
    return (in << 7) + (in << 6) + (in << 3) + (in << 2) + in;
#else
    return in * UINT32_C(205);
#endif
}

uint32_t mult_const_5(uint32_t in)
{
#if USE_SHIFT_MULT
    return (in << 2) + in;
#else
    return in * UINT32_C(5);
#endif
}

unsigned int mult_const_POLYW1_PACKEDBYTES(unsigned int param, unsigned int in)
{
    unsigned int tmp = 0;
    if (param == 192u) {
#if USE_SHIFT_MULT
        tmp = (in << 7) + (in << 6);
#else
        tmp = in * 192u;
#endif
    }
    if (param == 128u) {
#if USE_SHIFT_MULT
        tmp = (in << 7);
#else
        tmp = in * 128u;
#endif
    }
    return tmp;
}

int32_t mult_const_QINV(int32_t in)
{
    const uint32_t x = (uint32_t)in;
    uint32_t r;
#if USE_SHIFT_MULT
    r = (x << 25) + (x << 24) + (x << 23) + (x << 13) + x;
#else
    r = x * UINT32_C(58728449);
#endif
    /* Conversion to int32_t is implementation-defined when r > INT32_MAX.
       On typical x86/x64 compilers it preserves the low 32 bits. */
    return (int32_t)r;
}

int64_t mult_const_Q(int64_t in)
{
    /* Defined unsigned arithmetic, followed by signed interpretation.
       As with the original code, values outside int64_t range depend on
       the compiler's uint64_t-to-int64_t conversion semantics. */
    const uint64_t ux = (uint64_t)in;
    uint64_t r;
#if USE_SHIFT_MULT
    r = (ux << 23) - (ux << 13) + ux;
#else
    r = ux * UINT64_C(8380417);
#endif
    return (int64_t)r;
}

uint32_t mult_const_1025(uint32_t in)
{
#if USE_SHIFT_MULT
    return (in << 10) + in;
#else
    return in * UINT32_C(1025);
#endif
}

uint32_t mult_const_11275(uint32_t in)
{
#if USE_SHIFT_MULT
    return (in << 13) + (in << 11) + (in << 10)
         + (in << 3) + (in << 1) + in;
#else
    return in * UINT32_C(11275);
#endif
}

int32_t mult_const_GAMMA2(int32_t param, int32_t in)
{
    uint32_t tmp = 0;
    const uint32_t x = (uint32_t)in;
    if (param == 95232) {
#if USE_SHIFT_MULT
        tmp = (x << 16) + (x << 14) + (x << 13)
            + (x << 12) + (x << 10);
#else
        tmp = x * UINT32_C(95232);
#endif
    }
    if (param == 261888) {
#if USE_SHIFT_MULT
        tmp = (x << 18) - (x << 8);
#else
        tmp = x * UINT32_C(261888);
#endif
    }
    /* Conversion is implementation-defined if tmp > INT32_MAX. */
    return (int32_t)tmp;
}

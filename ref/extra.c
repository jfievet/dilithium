#include <stddef.h>
#include <stdint.h>
#include "extra.h"

size_t mult_const_SHAKE128_RATE(size_t in)
{
    // 148
    return (in << 7) + (in << 5) + (in << 3);
}

size_t mult_const_SHAKE256_RATE(size_t in)
{
    //136
    return (in << 7) + (in << 3);
}

#include <stdint.h>

int64_t binary_mult(int32_t a, int32_t b)
{
    int sign = 0;

    if (a < 0) { a = -a; sign ^= 1; }
    if (b < 0) { b = -b; sign ^= 1; }

    int64_t result = 0;
    int64_t multiplicand = (int64_t)a;
    uint32_t multiplier = (uint32_t)b; // <-- important

    while (multiplier != 0)
    {
        if (multiplier & 1)
        {
            result += multiplicand;
        }

        multiplicand <<= 1;
        multiplier >>= 1;
    }

    return sign ? -result : result;
}

unsigned int mult_const_POLYT1_PACKEDBYTES(unsigned int in){
    //320
    return (in << 8) + (in << 6);
}

unsigned int mult_const_POLYETA_PACKEDBYTES(unsigned int param, unsigned int in){
    unsigned int tmp=0;
    //POLYETA_PACKEDBYTES = 96
    if (param==96){
        tmp = (in << 6) + (in << 5);
    }
    //POLYETA_PACKEDBYTES = 128
    if (param==128){
        tmp = (in << 7);
    }
    return tmp;
}

unsigned int mult_const_POLYT0_PACKEDBYTES(unsigned int in){
    //POLYT0_PACKEDBYTES = 416
    return (in << 8) + (in << 7) + (in << 5);
}


unsigned int mult_const_POLYZ_PACKEDBYTES(unsigned int param, unsigned int in){
    unsigned int tmp=0;
    //POLYZ_PACKEDBYTES = 576
    if (param==576){
        tmp = (in << 9) + (in << 6);
    }
    //POLYZ_PACKEDBYTES = 640
    if (param==640){
        tmp = (in << 9) + (in << 7);
    }
    return tmp;
}

unsigned int mod3_u32(unsigned int x)
{
    while (x > 3)
    {
        x = (x >> 2) + (x & 3);
    }

    if (x == 3)
        return 0;

    return x;
}

uint32_t mult_const_205(uint32_t in){
    //205
    return (in << 7) + (in << 6) + (in << 3) + (in << 2) + in;
}

uint32_t mult_const_5(uint32_t in){
    //5
    return (in << 2) + in;
}
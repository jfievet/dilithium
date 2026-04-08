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

unsigned int mult_const_POLYW1_PACKEDBYTES(unsigned int param, unsigned int in){
    unsigned int tmp=0;
    //POLYW1_PACKEDBYTES = 192
    if (param==192){
        tmp = (in << 7) + (in << 6);
    }
    //POLYW1_PACKEDBYTES = 128
    if (param==128){
        tmp = (in << 7);
    }
    return tmp;
}

int32_t mult_const_QINV(int32_t in){
    uint32_t x = (uint32_t)in;
    uint32_t r = 0;

    r += (x << 25);
    r += (x << 24);
    r += (x << 23);
    r += (x << 13);
    r += x;

    return (int32_t)r;
}

int64_t mult_const_Q(int64_t in)
{
    uint64_t ux;
    int64_t r;

    if (in >= 0) {
        ux = (uint64_t)in;
        r = (int64_t)((ux << 23) - (ux << 13) + ux);
        return r;
    } else {
        ux = (uint64_t)(-(int64_t)in);
        r = (int64_t)((ux << 23) - (ux << 13) + ux);
        return -r;
    }
}

uint32_t mult_const_1025(uint32_t in){
    //1025
    return (in << 10) + in;
}

uint32_t mult_const_11275(uint32_t in){
    //11275
    return (in << 13)
         + (in << 11)
         + (in << 10)
         + (in << 3)
         + (in << 1)
         + in;
}

int32_t mult_const_GAMMA2(int32_t param, int32_t in){
// (8380417-1) / 88 = 95232
// (8380417-1) / 32 = 261888
    int32_t tmp=0;
    if (param == 95232){
        tmp = (in << 16)
            + (in << 14)
            + (in << 13)
            + (in << 12)
            + (in << 10);
    }

    if (param == 261888){
        tmp = (in << 18)
            - (in << 8);
    }
    return tmp;
}
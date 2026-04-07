#include <stddef.h>

size_t mult_const_SHAKE128_RATE(size_t in);
size_t mult_const_SHAKE256_RATE(size_t in);
int64_t binary_mult(int32_t a, int32_t b);
unsigned int mult_const_POLYT1_PACKEDBYTES(unsigned int in);
unsigned int mult_const_POLYETA_PACKEDBYTES(unsigned int param, unsigned int in);
unsigned int mult_const_POLYT0_PACKEDBYTES(unsigned int in);
unsigned int mult_const_POLYZ_PACKEDBYTES(unsigned int param, unsigned int in);
unsigned int mod3_u32(unsigned int x);
uint32_t mult_const_205(uint32_t in);
uint32_t mult_const_5(uint32_t in);
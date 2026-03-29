#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "randombytes.h"

/*************** CONFIG ****************/
#define USE_PRBS31        1
#define PRBS31_INIT       0x7FFFFFFF  // Must be non-zero
#define PRBS_DEBUG_FILE   1           // Enable PRBS logging
/****************************************/

#if USE_PRBS31

// Global PRBS31 state (31-bit LFSR)
static uint32_t prbs31_state = PRBS31_INIT;

#if PRBS_DEBUG_FILE
static FILE *prbs_file = NULL;

static void prbs_debug_init(void) {
    if (!prbs_file) {
        prbs_file = fopen("prbs31.txt", "a");  // append mode
        if (!prbs_file) {
            fprintf(stderr, "PRBS debug file open error\n");
            abort();
        }
    }
}

static void prbs_debug_byte(uint8_t b) {
    fprintf(prbs_file, "%02X\n", b);
}
#endif

// Generate next PRBS31 bit
static inline uint8_t prbs31_next_bit(void) {
    uint32_t new_bit;

    // taps: 31 and 28 → bits 30 and 27 (0-based)
    new_bit = ((prbs31_state >> 30) ^ (prbs31_state >> 27)) & 1;

    prbs31_state = ((prbs31_state << 1) | new_bit) & 0x7FFFFFFF;

    return (uint8_t)new_bit;
}

// Generate one byte from PRBS
static uint8_t prbs31_next_byte(void) {
    uint8_t b = 0;
    for (int i = 0; i < 8; i++) {
        b = (b << 1) | prbs31_next_bit();
    }

#if PRBS_DEBUG_FILE
    prbs_debug_init();
    prbs_debug_byte(b);
#endif

    return b;
}

void randombytes(uint8_t *out, size_t outlen) {
    for (size_t i = 0; i < outlen; i++) {
        out[i] = prbs31_next_byte();
    }
}

#else
// ===== existing OS RNG unchanged =====

#ifdef _WIN32
#include <windows.h>
#include <wincrypt.h>

void randombytes(uint8_t *out, size_t outlen) {
  HCRYPTPROV ctx;
  size_t len;

  if(!CryptAcquireContext(&ctx, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
    abort();

  while(outlen > 0) {
    len = (outlen > 1048576) ? 1048576 : outlen;
    if(!CryptGenRandom(ctx, len, (BYTE *)out))
      abort();

    out += len;
    outlen -= len;
  }

  if(!CryptReleaseContext(ctx, 0))
    abort();
}

#elif defined(__linux__) && defined(SYS_getrandom)

#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>

void randombytes(uint8_t *out, size_t outlen) {
  ssize_t ret;

  while(outlen > 0) {
    ret = syscall(SYS_getrandom, out, outlen, 0);
    if(ret == -1 && errno == EINTR)
      continue;
    else if(ret == -1)
      abort();

    out += ret;
    outlen -= ret;
  }
}

#else

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

void randombytes(uint8_t *out, size_t outlen) {
  static int fd = -1;
  ssize_t ret;

  while(fd == -1) {
    fd = open("/dev/urandom", O_RDONLY);
    if(fd == -1 && errno == EINTR)
      continue;
    else if(fd == -1)
      abort();
  }

  while(outlen > 0) {
    ret = read(fd, out, outlen);
    if(ret == -1 && errno == EINTR)
      continue;
    else if(ret == -1)
      abort();

    out += ret;
    outlen -= ret;
  }
}

#endif
#endif

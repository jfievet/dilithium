#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "../randombytes.h"
#include "../sign.h"

#define MLEN 59
#define CTXLEN 14
#define NTESTS 10000

/*************** CONFIG ****************/
#define ENABLE_LOGGING 1
#define PRBS_DEBUG_FILE 1
/****************************************/

#if ENABLE_LOGGING

static FILE *f_pk, *f_sk, *f_m, *f_sm, *f_ctx, *f_m2;

static void init_files(void) {
    f_pk  = fopen("pk.txt", "w");
    f_sk  = fopen("sk.txt", "w");
    f_m   = fopen("m.txt", "w");
    f_sm  = fopen("sm.txt", "w");
    f_ctx = fopen("ctx.txt", "w");
    f_m2  = fopen("m2.txt", "w");

    if (!f_pk || !f_sk || !f_m || !f_sm || !f_ctx || !f_m2) {
        fprintf(stderr, "File open error\n");
    }
}

static void close_files(void) {
    fclose(f_pk);
    fclose(f_sk);
    fclose(f_m);
    fclose(f_sm);
    fclose(f_ctx);
    fclose(f_m2);
}

static void dump_hex(FILE *f, const char *label, const uint8_t *data, size_t len) {
    fprintf(f, "%s (%lu bytes):\n", label, (unsigned long)len);
    for (size_t i = 0; i < len; i++) {
        fprintf(f, "%02X", data[i]);
        if ((i & 15) == 15) fprintf(f, "\n");
    }
    if (len % 16 != 0) fprintf(f, "\n");
    fprintf(f, "\n");
}

#endif

int main(void)
{
  size_t i, j;
  int ret;
  size_t mlen, smlen;
  uint8_t b;
  uint8_t ctx[CTXLEN] = {0};
  uint8_t m[MLEN + CRYPTO_BYTES];
  uint8_t m2[MLEN + CRYPTO_BYTES];
  uint8_t sm[MLEN + CRYPTO_BYTES];
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];

#if ENABLE_LOGGING
  init_files();
#endif

  snprintf((char*)ctx, CTXLEN, "test_dilitium");

#if ENABLE_LOGGING
  dump_hex(f_ctx, "ctx", ctx, CTXLEN);
#endif

#if ENABLE_LOGGING
#if USE_PRBS31 && PRBS_DEBUG_FILE
    FILE *f_prbs = fopen("prbs31.txt", "w");
    if (f_prbs) fclose(f_prbs);
#endif
#endif

  for(i = 0; i < NTESTS; ++i) {

    randombytes(m, MLEN);

    crypto_sign_keypair(pk, sk);
    crypto_sign(sm, &smlen, m, MLEN, ctx, CTXLEN, sk);
    ret = crypto_sign_open(m2, &mlen, sm, smlen, ctx, CTXLEN, pk);

#if ENABLE_LOGGING
    dump_hex(f_m,  "m",  m,  MLEN);
    dump_hex(f_pk, "pk", pk, CRYPTO_PUBLICKEYBYTES);
    dump_hex(f_sk, "sk", sk, CRYPTO_SECRETKEYBYTES);
    dump_hex(f_sm, "sm", sm, smlen);
    dump_hex(f_m2, "m2", m2, mlen);
#endif

    if(ret) {
      fprintf(stderr, "Verification failed\n");
      return -1;
    }
    if(smlen != MLEN + CRYPTO_BYTES) {
      fprintf(stderr, "Signed message lengths wrong\n");
      return -1;
    }
    if(mlen != MLEN) {
      fprintf(stderr, "Message lengths wrong\n");
      return -1;
    }
    for(j = 0; j < MLEN; ++j) {
      if(m2[j] != m[j]) {
        fprintf(stderr, "Messages don't match\n");
        return -1;
      }
    }

    randombytes((uint8_t *)&j, sizeof(j));
    do {
      randombytes(&b, 1);
    } while(!b);

    sm[j % (MLEN + CRYPTO_BYTES)] += b;

    ret = crypto_sign_open(m2, &mlen, sm, smlen, ctx, CTXLEN, pk);
    if(!ret) {
      fprintf(stderr, "Trivial forgeries possible\n");
      return -1;
    }
  }

#if ENABLE_LOGGING
  close_files();
#endif

  printf("CRYPTO_PUBLICKEYBYTES = %d\n", CRYPTO_PUBLICKEYBYTES);
  printf("CRYPTO_SECRETKEYBYTES = %d\n", CRYPTO_SECRETKEYBYTES);
  printf("CRYPTO_BYTES = %d\n", CRYPTO_BYTES);

  return 0;
}

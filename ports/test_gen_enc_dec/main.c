#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

#include "generated_header.h"
#include "../common/common_lib.h"

#include "../../ref/randombytes.h"
#include "../../ref/sign.h"

#define MLEN    59
#define CTXLEN  14
#define NTESTS  100

/*************** CONFIG ****************/
#define ENABLE_LOGGING  0

#ifndef ENABLE_GPIO_DEBUG
#define ENABLE_GPIO_DEBUG 0
#endif

#if ENABLE_GPIO_DEBUG
#define DEBUG_GPIO_WRITE(value) gpio_write(value)
#else
#define DEBUG_GPIO_WRITE(value) ((void)0)
#endif
/****************************************/

/*
 * Read FPGA 64-bit elapsed cycle counter.
 */
static uint64_t read_elapsed_time(void)
{
    volatile uint32_t *reg_freq = (volatile uint32_t *)0x0030000C;
    volatile uint32_t *reg_low  = (volatile uint32_t *)0x00300010;
    volatile uint32_t *reg_high = (volatile uint32_t *)0x00300014;

    // Read frequency register first to trigger counter capture
    uint32_t freq = *reg_freq;
    (void)freq;

    // Read captured 64-bit counter
    uint32_t low  = *reg_low;
    uint32_t high = *reg_high;

    return ((uint64_t)high << 32) | low;
}

/*
 * Wait until UART TX buffer is empty.
 */
static void uart_wait_empty(void)
{
    volatile uint32_t *level_reg =
        (volatile uint32_t *)(UART_TX_ADDR + 8);

    while (*level_reg != 0) {
        // Wait for UART transmission completion
    }
}

/*
 * Print 64-bit value in hexadecimal.
 */
static void print_hex64(uint64_t value)
{
    uart_send_hex((uint32_t)(value >> 32));
    uart_send_hex((uint32_t)value);
}

/*
 * Dump byte array to UART.
 *
 * Original implementation:
 * fprintf(f, "%s (%lu bytes):\n", label, (unsigned long)len);
 * fprintf(f, "%02X", data[i]);
 */
#if ENABLE_LOGGING

static void dump_hex(const char *label,
                     const uint8_t *data,
                     size_t len)
{
    dprint("%s (%d bytes):\r\n", label, (int)len);

    for (size_t i = 0; i < len; i++) {

        // fprintf(f, "%02X", data[i]);

        static const char hex[] = "0123456789ABCDEF";

        char str[3];

        str[0] = hex[(data[i] >> 4) & 0x0F];
        str[1] = hex[data[i] & 0x0F];
        str[2] = '\0';

        dprint("%s", str);

        if ((i & 15) == 15) {
            dprint("\r\n");
        }
    }

    if (len % 16 != 0) {
        dprint("\r\n");
    }

    dprint("\r\n");

    uart_wait_empty();
}

#endif

/*
 * Print error and stop execution.
 */
static void test_error(const char *message)
{
    dprint("ERROR: %s\r\n", message);
    uart_wait_empty();

    DEBUG_GPIO_WRITE(0xEE);

    while (1) {
        // Stop execution
    }
}

/*
 * Main application
 */
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

    uint64_t start_time;
    uint64_t end_time;
    uint64_t delta_cycles;

    /****************************************/
    /* FPGA INITIALIZATION                  */
    /****************************************/

    DEBUG_GPIO_WRITE(0x01);

    volatile uint32_t *reg0 =
        (volatile uint32_t *)AXI_SLAVE_REG_REG0;

    volatile uint32_t *reg1 =
        (volatile uint32_t *)AXI_SLAVE_REG_REG1;

    uint32_t reg0_val = *reg0;
    uint32_t reg1_val = *reg1;

    DEBUG_GPIO_WRITE(0x02);

    int day   = (int)(reg0_val & 0xFF);
    int month = (int)((reg0_val >> 8) & 0xFF);
    int year  = (int)((reg0_val >> 16) & 0xFFFF);

    uint8_t hour   = reg1_val & 0xFF;
    uint8_t minute = (reg1_val >> 8) & 0xFF;
    uint8_t second = (reg1_val >> 16) & 0xFF;

    DEBUG_GPIO_WRITE(0x01);

    dprint("FPGA build date: %d/%d/%d\r\n",
           day, month, year);

    uart_wait_empty();
    DEBUG_GPIO_WRITE(0x02);

    dprint("FPGA build time: %d:%d:%d\r\n",
           hour, minute, second);

    uart_wait_empty();
    DEBUG_GPIO_WRITE(0x03);

    dprint("Elf build date: %d/%d/%d\r\n",
           generated_day,
           generated_month,
           generated_year);

    uart_wait_empty();
    DEBUG_GPIO_WRITE(0x04);

    dprint("Elf build time: %d:%d:%d\r\n",
           generated_hour,
           generated_minute,
           generated_second);

    uart_wait_empty();
    DEBUG_GPIO_WRITE(0x05);

    volatile uint32_t *freq_reg =
        (volatile uint32_t *)0x30000C;

    uint32_t sys_freq = *freq_reg;

    dprint("FPGA system frequency: %d Hz\r\n",
           (int)sys_freq);

    uart_wait_empty();

    /****************************************/
    /* DILITHIUM TEST INITIALIZATION        */
    /****************************************/

    dprint("\r\n");
    dprint("================================\r\n");
    dprint(" Dilithium test application\r\n");
    dprint("================================\r\n");

    dprint("CRYPTO_PUBLICKEYBYTES = %d\r\n",
           CRYPTO_PUBLICKEYBYTES);

    dprint("CRYPTO_SECRETKEYBYTES = %d\r\n",
           CRYPTO_SECRETKEYBYTES);

    dprint("CRYPTO_BYTES = %d\r\n",
           CRYPTO_BYTES);

    dprint("NTESTS = %d\r\n", NTESTS);

    uart_wait_empty();

    /*
     * Original:
     *
     * init_files();
     *
     * FILE *f_prbs = fopen("prbs31.txt", "w");
     * if (f_prbs) fclose(f_prbs);
     */

    // Initialize context without snprintf dependency
    {
        const char context[] = "test_dilitium";

        for (i = 0; i < CTXLEN; i++) {
            ctx[i] = (uint8_t)context[i];
        }
    }

#if ENABLE_LOGGING
    // dump_hex(f_ctx, "ctx", ctx, CTXLEN);
    dump_hex("ctx", ctx, CTXLEN);
#endif

    /****************************************/
    /* MAIN TEST LOOP                       */
    /****************************************/

    while (1) {

        dprint("\r\nStarting Dilithium tests\r\n");
        uart_wait_empty();

        for (i = 0; i < NTESTS; ++i) {

            DEBUG_GPIO_WRITE(0x10);

            dprint("\r\nTest %d / %d\r\n",
                   (int)(i + 1), NTESTS);

            uart_wait_empty();

            /********************************/
            /* RANDOM MESSAGE                */
            /********************************/

            randombytes(m, MLEN);

            /********************************/
            /* KEY GENERATION                */
            /********************************/

            DEBUG_GPIO_WRITE(0x11);

            start_time = read_elapsed_time();

            ret = crypto_sign_keypair(pk, sk);

            end_time = read_elapsed_time();

            delta_cycles = end_time - start_time;

            if (ret != 0) {
                test_error("Key generation failed");
            }

            dprint("Key generation cycles: 0x");
            print_hex64(delta_cycles);
            dprint("\r\n");

            uart_wait_empty();

            /********************************/
            /* SIGNATURE                     */
            /********************************/

            DEBUG_GPIO_WRITE(0x12);

            start_time = read_elapsed_time();

            ret = crypto_sign(sm,
                              &smlen,
                              m,
                              MLEN,
                              ctx,
                              CTXLEN,
                              sk);

            end_time = read_elapsed_time();

            delta_cycles = end_time - start_time;

            if (ret != 0) {
                test_error("Signature generation failed");
            }

            dprint("Signature cycles: 0x");
            print_hex64(delta_cycles);
            dprint("\r\n");

            uart_wait_empty();

            /********************************/
            /* SIGNATURE VERIFICATION        */
            /********************************/

            DEBUG_GPIO_WRITE(0x13);

            start_time = read_elapsed_time();

            ret = crypto_sign_open(m2,
                                   &mlen,
                                   sm,
                                   smlen,
                                   ctx,
                                   CTXLEN,
                                   pk);

            end_time = read_elapsed_time();

            delta_cycles = end_time - start_time;

            dprint("Verification cycles: 0x");
            print_hex64(delta_cycles);
            dprint("\r\n");

            uart_wait_empty();

            /********************************/
            /* DATA LOGGING                  */
            /********************************/

#if ENABLE_LOGGING

            // dump_hex(f_m, "m", m, MLEN);
            dump_hex("m", m, MLEN);

            // dump_hex(f_pk, "pk", pk, CRYPTO_PUBLICKEYBYTES);
            dump_hex("pk", pk, CRYPTO_PUBLICKEYBYTES);

            // dump_hex(f_sk, "sk", sk, CRYPTO_SECRETKEYBYTES);
            dump_hex("sk", sk, CRYPTO_SECRETKEYBYTES);

            // dump_hex(f_sm, "sm", sm, smlen);
            dump_hex("sm", sm, smlen);

            // dump_hex(f_m2, "m2", m2, mlen);
            if (ret == 0 && mlen <= sizeof(m2)) {
                dump_hex("m2", m2, mlen);
            }

#endif

            /********************************/
            /* VERIFICATION CHECKS           */
            /********************************/

            DEBUG_GPIO_WRITE(0x14);

            if (ret) {
                // fprintf(stderr, "Verification failed\n");
                test_error("Verification failed");
            }

            if (smlen != MLEN + CRYPTO_BYTES) {
                // fprintf(stderr, "Signed message lengths wrong\n");
                test_error("Signed message lengths wrong");
            }

            if (mlen != MLEN) {
                // fprintf(stderr, "Message lengths wrong\n");
                test_error("Message lengths wrong");
            }

            for (j = 0; j < MLEN; ++j) {

                if (m2[j] != m[j]) {
                    // fprintf(stderr, "Messages don't match\n");
                    test_error("Messages don't match");
                }
            }

            /********************************/
            /* FORGERY TEST                  */
            /********************************/

            DEBUG_GPIO_WRITE(0x15);

            randombytes((uint8_t *)&j, sizeof(j));

            do {
                randombytes(&b, 1);
            } while (!b);

            sm[j % (MLEN + CRYPTO_BYTES)] += b;

            start_time = read_elapsed_time();

            ret = crypto_sign_open(m2,
                                   &mlen,
                                   sm,
                                   smlen,
                                   ctx,
                                   CTXLEN,
                                   pk);

            end_time = read_elapsed_time();

            delta_cycles = end_time - start_time;

            dprint("Forgery verification cycles: 0x");
            print_hex64(delta_cycles);
            dprint("\r\n");

            uart_wait_empty();

            if (!ret) {
                // fprintf(stderr, "Trivial forgeries possible\n");
                test_error("Trivial forgeries possible");
            }

            DEBUG_GPIO_WRITE(0x16);

            dprint("Test %d PASSED\r\n", (int)(i + 1));

            uart_wait_empty();
        }

        /************************************/
        /* END OF TESTS                     */
        /************************************/

        DEBUG_GPIO_WRITE(0xFF);

        dprint("\r\n");
        dprint("================================\r\n");
        dprint(" All Dilithium tests PASSED\r\n");
        dprint("================================\r\n");

        uart_wait_empty();

        /*
         * Original:
         *
         * close_files();
         *
         * printf("CRYPTO_PUBLICKEYBYTES = %d\n",
         *        CRYPTO_PUBLICKEYBYTES);
         *
         * printf("CRYPTO_SECRETKEYBYTES = %d\n",
         *        CRYPTO_SECRETKEYBYTES);
         *
         * printf("CRYPTO_BYTES = %d\n",
         *        CRYPTO_BYTES);
         */

        // Repeat test sequence
    }

    return 0;
}
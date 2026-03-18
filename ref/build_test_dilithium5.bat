@echo off

REM ---- Compiler ----
set CC=gcc

REM ---- Flags ----
set CFLAGS=-Wall -Wextra -Wpedantic -Wmissing-prototypes -Wredundant-decls -Wshadow -Wvla -Wpointer-arith -O3 -fomit-frame-pointer

REM ---- Sources ----
set SOURCES=sign.c packing.c polyvec.c poly.c ntt.c reduce.c rounding.c
set KECCAK_SOURCES=%SOURCES% fips202.c symmetric-shake.c

%CC% %CFLAGS% -DDILITHIUM_MODE=5 ^
 -o test\test_dilithium5.exe ^
 test\test_dilithium.c randombytes.c %KECCAK_SOURCES%

echo Done.
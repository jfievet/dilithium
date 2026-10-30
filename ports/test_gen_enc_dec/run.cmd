@echo off
REM Compile Dilithium Mode 2 for ELAN32, with code and data in SDRAM.

cd /d "%~dp0"
if errorlevel 1 (
    echo ERROR: Cannot enter the application directory.
    exit /b 1
)

python gen_time_header.py
if errorlevel 1 (
    echo ERROR: Build timestamp generation failed.
    exit /b 1
)

REM ---- RISC-V toolchain ----
set RISCV_GCC_PATH=C:\Tools\xpack-riscv-none-elf-gcc-15.2.0-1\bin
set RISCV_PREFIX=riscv-none-elf-

REM ---- Compiler ----
set CC="%RISCV_GCC_PATH%\%RISCV_PREFIX%gcc.exe"
set OBJDUMP="%RISCV_GCC_PATH%\%RISCV_PREFIX%objdump.exe"
set SIZE="%RISCV_GCC_PATH%\%RISCV_PREFIX%size.exe"
set OBJCOPY="%RISCV_GCC_PATH%\%RISCV_PREFIX%objcopy.exe"

REM ---- Compiler flags ----
set CFLAGS=-march=rv32i -mabi=ilp32 -mno-relax -Os
set CFLAGS=%CFLAGS% -DDILITHIUM_MODE=2
set CFLAGS=%CFLAGS% -ffunction-sections -fdata-sections

REM ---- Linker flags ----
set LDFLAGS=-nostartfiles -T app_sdram.ld -Wl,--gc-sections,-Map=main.map

REM ---- Dilithium source directory ----
set DILITHIUM_DIR=..\..\ref

REM ---- Dilithium sources ----
set SOURCES=%DILITHIUM_DIR%\sign.c ^
 %DILITHIUM_DIR%\packing.c ^
 %DILITHIUM_DIR%\polyvec.c ^
 %DILITHIUM_DIR%\poly.c ^
 %DILITHIUM_DIR%\ntt.c ^
 %DILITHIUM_DIR%\reduce.c ^
 %DILITHIUM_DIR%\rounding.c ^
 %DILITHIUM_DIR%\extra.c ^
 %DILITHIUM_DIR%\randombytes.c

REM ---- Keccak sources ----
set KECCAK_SOURCES=%SOURCES% ^
 %DILITHIUM_DIR%\fips202.c ^
 %DILITHIUM_DIR%\symmetric-shake.c

REM ---- Application sources ----
set APP_SOURCES=crt0.S main.c ..\common\common_lib.c

REM ---- Compile and link ----
%CC% %CFLAGS% %LDFLAGS% %APP_SOURCES% %KECCAK_SOURCES% -o main.elf

if errorlevel 1 (
    echo ERROR: Compilation failed!
    exit /b 1
)

REM ---- Generate disassembly ----
%OBJDUMP% -d main.elf > main.dis
if errorlevel 1 (
    echo ERROR: Disassembly generation failed.
    exit /b 1
)

REM ---- Show memory usage ----
%SIZE% main.elf
if errorlevel 1 (
    echo ERROR: Memory size reporting failed.
    exit /b 1
)

REM ---- Generate HEX ----
%OBJCOPY% -O ihex main.elf main.hex
if errorlevel 1 (
    echo ERROR: HEX generation failed.
    exit /b 1
)

echo Compilation finished.

REM ---- Generate the UART upload image, without BRAM initialization ----
python xilinx_hex_to_ram_txt.py
if errorlevel 1 (
    echo ERROR: UART image conversion failed.
    exit /b 1
)

echo UART image generated: %~dp0ram_data.txt
echo Load address: 0x00500000
echo Jump address: 0x00500000

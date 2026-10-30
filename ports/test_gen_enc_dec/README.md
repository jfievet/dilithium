# Dilithium SDRAM application for ELAN32

Run `run.cmd` to build Dilithium mode 2 for RV32I/ILP32. The application,
constants, data, BSS and stack reside in the 8 MiB SDRAM region beginning
at `0x00500000`. The initial stack pointer is `0x00D00000` and grows downward.

The local `app_sdram.ld`, `crt0.S` and `xilinx_hex_to_ram_txt.py` were copied
from `elan32_soc/riscv_soc_shell/c/bootloader_and_app`. They are independent
copies; building does not access that repository. The converter reads
`main.hex` here instead of `main2.hex`. The previous `link.ld` is retained
but is not used by `run.cmd`.

Build outputs are `main.elf`, `main.dis`, `main.map`, `main.hex` and
`ram_data.txt`. This build does not generate FPGA BRAM initialization.
The converter reports the detected image base and writes one 32-bit word
per line as `B3 B2 B1 B0`, without padding before the image base.

For subsequent UART loading:

- Keep the bootloader in BRAM and enable and initialize SDRAM before loading.
- Select this directory's `ram_data.txt` in the UART loader.
- Set both `APP_BASE` and `JUMP_ADDR` to `0x00500000`.
- Use a bootloader supporting the `0xBB` jump-address configuration command.

The application retains its key generation, signing and verification tests,
UART output and optional GPIO markers. It uses `ref/randombytes.c` with
PRBS31 and file logging disabled (`PRBS_DEBUG_FILE=0`). This deterministic
generator is intended for reproducible tests, not production key generation.

Compilation and hardware validation are left to the user. Inspect `main.map`
for memory placement, confirm the converter reports base `0x00500000`, and
check the Dilithium tests after upload. SDRAM placement alone does not
guarantee stack headroom or successful hardware execution.

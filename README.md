# ECC: E-comOS Compiler Center  
A purpose-built compiler hub for E-comOS—bridging bare-metal initialization and embedded development with C-like syntax, backed by the AGPLv3 license to protect open collaboration.  


## What is ECC?  
ECC (E-comOS Compiler Center) is the core compiler infrastructure for the E-comOS ecosystem, designed to eliminate the "assembly-only" barrier in early-stage embedded development.  

Built specifically for E-comOS—a lightweight, embedded-focused operating system—ECC lets developers write low-level code (bootloaders, hardware init routines, and E-comOS kernel stubs) using intuitive C-like syntax. It generates raw machine code that runs directly on bare metal, with no reliance on operating system services, stacks, or runtime environments—critical for CPU power-on sequences where traditional compilers (like C) fail.  

ECC is licensed under the **GNU Affero General Public License v3 (AGPLv3)**, ensuring that any modifications, including those used to provide network services (e.g., online E-comOS development IDEs), must share their source code with users.  


## Core Features  
1. **E-comOS Native**  
   Deeply integrated with E-comOS architecture: supports E-comOS hardware abstractions, memory layouts, and boot protocols. Generated code aligns with E-comOS’s embedded constraints (low memory, real-time requirements).  

2. **Bare-Metal First**  
   Runs directly after CPU power-on, supporting raw hardware states (x86 Real Mode, ARM EL0, RISC-V Machine Mode) without preinitialized stacks or runtime. Ideal for E-comOS’s minimal boot flow.  

3. **Hardware-Centric Syntax**  
   Intuitive constructs for E-comOS hardware interaction:  
   - Register manipulation (`reg.ecomos_cfg = 0x01` for E-comOS config registers),  
   - Physical memory access (`mem.dword[ECOMOS_BOOT_ADDR] = 0x1234`),  
   - E-comOS-specific instructions (`ecomos_clk_enable(3)` for clock control).  

4. **Stackless Design**  
   Function calls use direct jumps (`jmp`) instead of stack-dependent `call` instructions, adapting to E-comOS’s early boot state where stacks are uninitialized.  

5. **Precise Binary Control**  
   Explicit alignment (`align 32`) and padding (`fill ECOMOS_MBR_SIZE - __offset__, 0x00`) to meet E-comOS boot media requirements (e.g., 512-byte MBRs, 4KB sector alignment).  

6. **AGPLv3 Protected**  
   Ensures modifications to ECC—even those used in network services (e.g., E-comOS Web IDEs, cloud compilers)—must distribute source code to users, preserving E-comOS’s open ecosystem.  

7. **Lightweight & Embeddable**  
   Compiler binary <100KB, with no external dependencies. Deployable directly on E-comOS devices for on-device compilation.  


## Quick Start  

### 1. Prerequisites  
- GCC/Clang (v10+)  
- Make  
- QEMU (for E-comOS emulation: `qemu-system-x86_64` or `qemu-system-arm`)  


### 2. Build ECC  
```bash
# Clone the repository
git clone https://github.com/Saladin5101/ecc.git && cd ecc

# Build for E-comOS target support
make ECOMOS_SUPPORT=1

# Verify installation
./bin/ecc --version  # Should show "ECC v0.1 (E-comOS Compiler Center)"
./bin/ecc --help     # List E-comOS-specific targets/options
```


### 3. Write Your First E-comOS Boot Program  
Create `ecomos_boot.coc`—a minimal bootloader to initialize E-comOS’s core hardware:  
```coc
// Target: E-comOS x86 (Real Mode)
// Purpose: Early hardware init for E-comOS (clock, UART, boot flag)

// E-comOS constants (from E-comOS hardware spec)
const ECOMOS_CLOCK_REG = 0x6000  // Clock control register
const ECOMOS_UART_BASE = 0x6040  // UART base address
const ECOMOS_BOOT_FLAG = 0x7000  // Boot completion flag

// Initialize E-comOS system clock (8MHz)
func init_clock() {
    mem.byte[ECOMOS_CLOCK_REG] = 0x03;  // 8MHz mode
}

// Send "E-comOS booting..." via UART
func uart_send_str(str) {
    var i = 0;
    while str[i] != 0 {
        mem.byte[ECOMOS_UART_BASE] = str[i];  // Write byte to UART
        i = i + 1;
    }
}

// Main boot logic
reg.ax = 0x0000;       // Initialize registers
reg.sp = 0x7c00;       // Temp stack pointer (E-comOS spec)

init_clock();          // Start system clock
uart_send_str("E-comOS booting via ECC...\n");

// Set E-comOS boot flag (0xAA = success)
mem.byte[ECOMOS_BOOT_FLAG] = 0xAA;

// Pad to E-comOS MBR size (512 bytes) and add signature
fill 510 - __current_offset__, 0x00;
mem.word[0x7c00 + 510] = 0xaa55;  // E-comOS MBR signature
```


### 4. Compile for E-comOS  
```bash
# Compile for E-comOS x86 (Real Mode), output raw binary
./bin/ecc --target=ecomos-x86-real --format=bin ecomos_boot.coc -o ecomos_boot.bin

# Verify binary size (matches E-comOS MBR spec: 512 bytes)
ls -l ecomos_boot.bin  # Output: -rwxr-xr-x 1 user user 512 Oct 6 10:00 ecomos_boot.bin
```


### 5. Test with E-comOS Emulation  
```bash
# Run in QEMU with E-comOS emulation mode
qemu-system-x86_64 -drive format=raw,file=ecomos_boot.bin -serial stdio

# Expected output (via UART): "E-comOS booting via ECC..."
# Boot flag at 0x7000 will be set to 0xAA (verify with QEMU monitor)
```


## Syntax Overview  
ECC’s syntax is tailored for E-comOS hardware interaction, blending C-like structure with assembly directness:  

| Operation                  | ECC Syntax Example                     | E-comOS Context                          |
|----------------------------|----------------------------------------|------------------------------------------|
| Register Access            | `reg.ecomos_cfg = 0x01`                | Modify E-comOS configuration register    |
| Physical Memory (Byte)     | `mem.byte[ECOMOS_CLOCK_REG] = 0x03`    | Set E-comOS clock control byte           |
| Physical Memory (Dword)    | `mem.dword[ECOMOS_BOOT_ADDR] = 0x1234` | Write 32-bit value to E-comOS boot area  |
| Stackless Function         | `func init_clock() { ... }`            | E-comOS hardware init routine            |
| Function Call              | `init_clock()`                         | Invoke init routine (direct jump)        |
| E-comOS Instruction        | `ecomos_clk_enable(3)`                 | E-comOS-specific hardware macro          |
| Binary Padding             | `fill ECOMOS_MBR_SIZE - __offset__, 0x00` | Meet E-comOS MBR size requirements    |
| Constant Definition        | `const ECOMOS_UART_BASE = 0x6040`      | E-comOS hardware address constant        |  


## Project Structure  
```
ecc/
├── src/
│   ├── lexer.c          # Tokenizer for ECC syntax (registers, E-comOS macros)
│   ├── parser.c         # Builds AST with E-comOS-specific nodes
│   ├── codegen/         # Code generators for E-comOS targets
│   │   ├── ecomos_x86.c # E-comOS x86 (Real Mode) backend
│   │   └── ecomos_arm.c # E-comOS ARM (EL0) backend
│   └── ecomos/          # E-comOS integration layer (constants, macros)
│       ├── ecomos.h     # E-comOS hardware addresses/registers
│       └── macros.coc   # E-comOS-specific syntax macros
├── examples/            # E-comOS-specific examples
│   ├── ecomos_boot.coc  # E-comOS bootloader init
│   └── ecomos_uart.coc  # E-comOS UART communication
├── tests/               # E-comOS compliance tests
│   ├── ecomos_mbr_test.sh  # Validate MBR compliance
│   └── hardware_init_test/ # Test E-comOS hardware init
├── bin/                 # Compiled ECC binary
├── LICENSE              # AGPLv3 License text
└── Makefile             # Build config with E-comOS target support
```


## Supported E-comOS Targets  
| Target Architecture | ECC Target Flag       | E-comOS Use Case                          |
|---------------------|-----------------------|------------------------------------------|
| x86 (Real Mode)     | `--target=ecomos-x86-real` | E-comOS x86 bootloaders, MBRs            |
| ARM (EL0)           | `--target=ecomos-arm-el0`  | E-comOS ARM embedded device init         |
| RISC-V (Machine)    | `--target=ecomos-riscv-m`  | Planned: E-comOS RISC-V SoC support      |  


## AGPLv3 License  
ECC is licensed under the **GNU Affero General Public License v3**. Key terms for E-comOS ecosystem users:  

- **Modifications**: Any changes to ECC must be distributed under AGPLv3, including E-comOS-specific forks.  
- **Network Services**: If you use a modified ECC to provide network services (e.g., E-comOS Web IDEs, cloud compilers), you **must**:  
  - Provide a clear link to the modified source code in the service interface.  
  - Make the source available for 3+ years after service discontinuation.  
- **Patents**: AGPLv3 prohibits using patents to restrict ECC users from exercising their rights under the license.  

Full license text: [LICENSE](LICENSE). For E-comOS ecosystem questions, see the [GNU AGPLv3 FAQ](https://www.gnu.org/licenses/agpl-3.0.html#faq).  


## Contributing  
ECC thrives on E-comOS community collaboration! To contribute:  
1. Submit bug reports for E-comOS target issues (e.g., "x86 Real Mode code misaligns with E-comOS spec").  
2. Add support for new E-comOS hardware (e.g., RISC-V SoCs) via codegen backends.  
3. Improve examples for E-comOS use cases (e.g., sensor initialization, E-comOS kernel stubs).  

See [CONTRIBUTING.md](CONTRIBUTING.md) for workflow details.  


## Contact  
For E-comOS ecosystem integration questions:  
- Project Lead: Saladin5101  
- Repository: [github.com/Saladin5101/ecc](https://github.com/Saladin5101/ecc)

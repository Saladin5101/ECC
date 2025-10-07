# ECC: ELFCOST Compiler  
*The compiler for ELFCOST —— a minimalist language for bare-metal hardware operations.*  


## What is ELFCOST?  
ELFCOST is not a C-like language, nor is it an assembly syntax clone. It’s designed to let you interact with hardware (registers, memory, peripherals) in a **human-readable, syntax-light manner** —— no curly braces, no complex operators, just direct hardware primitives.  


## Key Features of ELFCOST  
- **Unique Memory Operations**: Use syntax like `memory.save-use(1024)` to allocate/use memory, no low-level pointer juggling.  
- **Register Control**: Direct register access (e.g., `register.ax = 0x1234`) without assembly’s verbosity.  
- **Bare-Metal Focus**: Compiles to raw binary (e.g., MBRs, bootloaders) with no runtime dependencies.  
- **Cross-Platform Compiler (ECC)**: Build on Linux/macOS, target x86 real mode (with ARM/RISC-V support planned).  


## Quick Start  
### 1. Clone the Repository  
```bash
git clone https://github.com/your-username/ecc.git
cd ecc
```

### 2. Compile the Compiler (ECC)  
```bash
make debug  # Builds debug version (elfc-compiler-dbg)
# Or for release (optimized binary):
make release
```

### 3. Write a Simple ELFCOST Program  
Create `hello.elfc`:  
```elfcost
use x86_real;  # Import x86 real-mode hardware module

# Allocate 512 bytes for MBR + signature
memory.save-use(510);  # Use 510 bytes for code
memory.write-word(0xAA55);  # MBR signature

# Initialize register and halt
register.ax = 0x0000;
hlt();  # Halt execution
```

### 4. Compile to Binary  
```bash
./elfc-compiler-dbg hello.elfc hello.bin
```

### 5. Test with QEMU  
```bash
qemu-system-x86_64 -drive format=raw,file=hello.bin -nographic
```


## ELFCOST Syntax Highlights  
### Memory Operations  
- `memory.save-use(1024)`: Allocate and use 1024 bytes of memory.  
- `memory.write-byte(0xB8000, 'A')`: Write byte `'A'` to memory address `0xB8000`.  
- `var data = memory.read-word(0x1000)`: Read 2-byte word from `0x1000` into `data`.  

### Register Control  
- `register.ax = 0x1234`: Set AX register to `0x1234`.  
- `var val = register.bx`: Read BX register into `val`.  

### Functions & Control Flow  
```elfcost
func print_char(c) {
    memory.write-byte(0xB8000, c);  # Write char to VGA memory
}

print_char('E');  # Call function
```


## License  
- **Source Code (ECC Compiler)**: [GNU AGPLv3](LICENSE)  
- **Documentation (ELFCOST Syntax)**: [GNU Free Documentation License (FDL) 1.3](docs/LICENSE_FDL.md)  


## Contributing  
We welcome contributions! To get started:  
1. Fork the repository.  
2. Create a feature branch (`git checkout -b feature/your-feature`).  
3. Commit changes (`git commit -m 'Add new feature'`).  
4. Push to the branch (`git push origin feature/your-feature`).  
5. Open a pull request.  


---  
*Built for bare-metal enthusiasts who want hardware control without assembly’s complexity.*  
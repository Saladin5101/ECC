# ECC v1.0 Release

## Download
- **Binary Release**: `ecc-v1.0.tar.gz` (11KB)

## Installation
```bash
tar -xzf ecc-v1.0.tar.gz
cd ecc-mvp
```

## Quick Test
```bash
./elfc-compiler debug -el examples/test.elfc -ma output.bin
hexdump -C output.bin
```

## How to Distribute

### GitHub Release (Recommended)
- Upload `ecc-v1.0.tar.gz` to GitHub releases
- Tag: `v1.0`
- Done. People download and use.

### Or Just Share the File
- Send `ecc-v1.0.tar.gz` directly
- 11KB, fits in email

## What's Included
- `elfc-compiler` - Main executable (57KB)
- `examples/test.elfc` - Sample ELFCOST code
- `README.txt` - Usage instructions

## System Requirements
- Linux x86_64
- No dependencies required
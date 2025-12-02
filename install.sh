#!/bin/bash
# ECC Installer - One-liner install script

set -e

echo "Installing ECC v1.0..."

# Download and extract
curl -L https://github.com/your-username/ecc/releases/download/v1.0/ecc-v1.0.tar.gz | tar -xz

# Make executable
chmod +x ecc-mvp/elfc-compiler

# Test
cd ecc-mvp
./elfc-compiler debug -el examples/test.elfc -ma test.bin

echo "✅ ECC installed successfully!"
echo "📁 Location: ./ecc-mvp/"
echo "🚀 Usage: ./ecc-mvp/elfc-compiler debug -el input.elfc -ma output.bin"
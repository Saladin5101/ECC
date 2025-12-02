CC = gcc
CFLAGS = -I src/ -w -Wno-error
RELEASE_FLAGS = -O2
DEBUG_FLAGS = -g -O0
TARGET = elfc-compiler
DEBUG_TARGET = elfc-compiler-dbg

SRC_FILES = src/main.c src/cli/cli.c src/codegen/codegen.c src/common/utils.c src/lexer/lexer.c src/module/modules.c src/parser/parser.c

all:
	$(CC) $(SRC_FILES) $(CFLAGS) $(RELEASE_FLAGS) -o $(TARGET)
	@echo "Release version compiled: ./$(TARGET)"

debug:
	$(CC) $(SRC_FILES) $(CFLAGS) $(DEBUG_FLAGS) -o $(DEBUG_TARGET)
	@echo "Debug version compiled: ./$(DEBUG_TARGET)"

clean:
	rm -rf $(TARGET) $(DEBUG_TARGET) ecc-mvp *.bin
	@echo "Cleaned all artifacts"

package: all
	mkdir -p ecc-mvp/examples
	cp $(TARGET) ecc-mvp/
	echo "use x86_real;\nreg.ax = 0x1234;\nreg.bx = 0x5678;" > ecc-mvp/examples/test.elfc
	echo "# ECC MVP\nELFCOST compiler supporting basic register operations.\n\nUsage:\n./elfc-compiler debug -el examples/test.elfc -ma output.bin" > ecc-mvp/README.txt
	@echo "Package created in ./ecc-mvp"
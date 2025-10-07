# 默认目标：编译发布版编译器（第一个目标，make无参数时执行）
all:
	$(CC) $(SRC_FILES) $(CFLAGS) $(RELEASE_FLAGS) -o $(TARGET)

# 编译器和编译选项
CC = gcc
CFLAGS = -I src/  # 头文件搜索路径（src/目录）
RELEASE_FLAGS = -O2  # 发布模式：优化
DEBUG_FLAGS = -g -O0  # 调试模式：带符号表，不优化

# 目标文件
TARGET = elfc-compiler  # 发布版编译器
DEBUG_TARGET = elfc-compiler-dbg  # 调试版编译器
TEST_RUNNER = tests/runner  # 测试程序

# 源代码文件（递归匹配src/下的所有.c文件）
SRC_FILES = $(wildcard src/*.c src/*/*.c)
TEST_FILES = $(wildcard tests/*.c src/lexer/*.c src/parser/*.c src/codegen/*.c src/common/*.c)  # 测试需要的所有代码

# 编译调试版编译器
debug:
	$(CC) $(SRC_FILES) $(CFLAGS) $(DEBUG_FLAGS) -o $(DEBUG_TARGET)

# 编译并运行测试程序
test:
	$(CC) $(TEST_FILES) $(CFLAGS) $(DEBUG_FLAGS) -o $(TEST_RUNNER)
	./$(TEST_RUNNER)

# 清理编译产物
clean:
	rm -f $(TARGET) $(DEBUG_TARGET) $(TEST_RUNNER) *.bin
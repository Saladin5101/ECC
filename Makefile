# 编译器与编译选项配置
CC = gcc
CFLAGS = -I src/ -w -Wno-error  # 宽松编译：关闭所有警告，不把警告当错误
RELEASE_FLAGS = -O2  # Release 版：优化代码体积与速度
DEBUG_FLAGS = -g -O0  # Debug 版：保留调试符号，关闭优化（方便 gdb 调试）
TARGET = elfc-compiler  # Release 版可执行文件名
DEBUG_TARGET = elfc-compiler-dbg  # Debug 版可执行文件名

# 项目所有源代码文件（确保包含所有 .c 文件，避免漏编译）
SRC_FILES = src/main.c src/cli/cli.c src/codegen/codegen.c src/common/utils.c src/lexer/lexer.c src/module/modules.c src/parser/parser.c

# -------------------------- 基础编译目标 --------------------------
# 默认目标：编译 Release 版编译器
all:
	$(CC) $(SRC_FILES) $(CFLAGS) $(RELEASE_FLAGS) -o $(TARGET)
	@echo "✅ Release 版编译完成！可执行文件：./$(TARGET)"

# 编译 Debug 版编译器（带调试符号）
debug:
	$(CC) $(SRC_FILES) $(CFLAGS) $(DEBUG_FLAGS) -o $(DEBUG_TARGET)
	@echo "✅ Debug 版编译完成！可执行文件：./$(DEBUG_TARGET)"

# 清理所有编译产物（可执行文件、打包目录、二进制输出）
clean:
	rm -rf $(TARGET) $(DEBUG_TARGET) ecc-mvp *.bin
	@echo "✅ 所有编译产物已清理完成！"

# -------------------------- 一键打包目标 --------------------------
# 依赖 all 目标：先编译 Release 版，再打包
package: all
	# 1. 创建打包目录结构（ecc-mvp + examples 子目录）
	mkdir -p ecc-mvp/examples
	# 2. 复制编译器可执行文件到打包目录
	cp $(TARGET) ecc-mvp/
	# 3. 生成极简测试代码（编译器可正确处理的 ELFCOST 源码）
	echo "use x86_real;\nreg.ax = 0x1234;\nreg.bx = 0x5678;" > ecc-mvp/examples/test.elfc
	# 4. 生成使用说明文档（README.txt）
	cat > ecc-mvp/README.txt << EOF
	# ECC 最小可行版（MVP）
	ELFCOST 语言专用编译器，当前支持核心功能：

	## 已实现功能
	1. 解析模块引入语句：`use x86_real;`（x86 实模式支持）
	2. 编译寄存器赋值语句：如 `reg.ax = 0x1234;`、`reg.bx = 0x5678;`
	3. 输出 x86 实模式二进制文件（.bin 格式，可直接用 QEMU 运行）
	4. 命令行双模式：debug（详细日志）、compile（快速编译）

	## 快速使用
	### 1. 调试模式（输出编译流程日志）
	./elfc-compiler debug -el examples/test.elfc -ma output.bin

	### 2. 正常模式（无日志，快速编译）
	./elfc-compiler compile -el examples/test.elfc -ma output.bin

	## 验证编译结果
	用 `hexdump -C output.bin` 查看机器码，预期输出：
	00000000  b8 34 12 bb 78 56                                 |.4..xV|
	（对应 x86 汇编：mov ax, 0x1234; mov bx, 0x5678）

	## 已知限制
	- 暂不支持内存操作（如 `memory.save-use(1024)`）
	- 暂不支持函数定义与调用（如 `func print() { ... }`）
	- 复杂语法错误会直接退出（未做优雅错误处理）
	- 仅支持 x86 16 位通用寄存器（ax/bx/cx/dx）
	EOF
	# 5. 打包完成提示
	@echo "✅ 打包成功！最终目录：./ecc-mvp"
	@echo "📁 打包内容：可执行文件 + 示例代码 + 使用说明"
# 新增：极简测试目标（放在所有目标最后）
test-tab:
	@echo "Tab 缩进测试成功！"  # 这里必须用纯 Tab 开头，不能有空格
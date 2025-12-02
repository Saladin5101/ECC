#ifndef UTILS_H
#define UTILS_H

#include "types.h"  // 依赖TokenType等type
#include <stdio.h>
#include <stdarg.h>  // 用于可变parameter（错误处理function）

// -------------------------- 错误处理 --------------------------
// 打印错误message并退出程序（support可变parameter，如error("line%d：%s", line, msg)）
// 自动添加换行，错误码固定为1
void error(const char* format, ...);
// src/common/utils.h 新增
// 带组message的错误提示（如“标识符不在certain组中”）
void error_with_group(const char* id, const char* group);
// 同理，也可以有其它的错误提示，可以在下面补充
// 带文件名和line的错误提示
void error_with_file_line(const char* filename, int line, const char* format, ...);
// -------------------------- string转number --------------------------
// string转十六basenumber（support0x前缀，如"0x1234"→4660）
// 若string非法，调用error报错
uint32_t str_to_hex(const char* s);

// string转十basenumber（如"123"→123）
// 若string非法，调用error报错
uint32_t str_to_dec(const char* s);

// -------------------------- TokenHelper --------------------------
// 将TokenTypeconvert为string（如TOKEN_REG→"TOKEN_REG"）
// 用于调试和报错时显示Tokentype
const char* token_type_to_str(TokenType type);

// -------------------------- memoryoperation --------------------------
// 安全allocationmemory（若mallocfailed，调用error报错，避免返回NULL）
void* safe_malloc(size_t size);

#endif // UTILS_H
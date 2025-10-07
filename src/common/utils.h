#ifndef UTILS_H
#define UTILS_H

#include "types.h"  // 依赖TokenType等类型
#include <stdio.h>
#include <stdarg.h>  // 用于可变参数（错误处理函数）

// -------------------------- 错误处理 --------------------------
// 打印错误信息并退出程序（支持可变参数，如error("行号%d：%s", line, msg)）
// 自动添加换行，错误码固定为1
void error(const char* format, ...);

// -------------------------- 字符串转数字 --------------------------
// 字符串转十六进制数字（支持0x前缀，如"0x1234"→4660）
// 若字符串非法，调用error报错
uint32_t str_to_hex(const char* s);

// 字符串转十进制数字（如"123"→123）
// 若字符串非法，调用error报错
uint32_t str_to_dec(const char* s);

// -------------------------- Token辅助 --------------------------
// 将TokenType转换为字符串（如TOKEN_REG→"TOKEN_REG"）
// 用于调试和报错时显示Token类型
const char* token_type_to_str(TokenType type);

// -------------------------- 内存操作 --------------------------
// 安全分配内存（若malloc失败，调用error报错，避免返回NULL）
void* safe_malloc(size_t size);

#endif // UTILS_H
#ifndef LEXER_H
#define LEXER_H

#include "common/types.h"
#include <stdio.h>

// 模拟fpeek：预览下一个字符（不移动文件指针）
static inline int fpeek(FILE* fp) {
    int c = fgetc(fp);
    if (c != EOF) ungetc(c, fp);
    return c;
}

// 词法分析器状态
typedef struct {
    FILE* fp;       // 输入文件
    int line;       // 当前行号
    int current_char; // 当前字符
} Lexer;

// 初始化词法分析器
Lexer* lexer_init(FILE* fp);

// 获取下一个Token
Token lexer_next_token(Lexer* lexer);

// 释放词法分析器
void lexer_free(Lexer* lexer);

#endif // LEXER_H
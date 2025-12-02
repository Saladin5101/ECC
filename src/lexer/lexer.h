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

// lexer状态
typedef struct {
    FILE* fp;       // 输入文件
    int line;       // currentline
    int current_char; // current字符
} Lexer;

// 初始化lexer
Lexer* lexer_init(FILE* fp);

// 获取下一个Token
Token lexer_next_token(Lexer* lexer);

// 释放lexer
void lexer_free(Lexer* lexer);

#endif // LEXER_H
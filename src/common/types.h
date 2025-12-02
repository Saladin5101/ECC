#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>  // 用标准整数type（uint8_t、uint16_t等），避免平台差异

// -------------------------- 词法分析相关type --------------------------
// Tokentype枚举（所有可能的语法单元）
typedef enum {
    TOKEN_EOF,         // 文件结束符
    TOKEN_USE,         // use关键字（引入module）
    TOKEN_REG,         // reg.关键字（registeroperation）
    TOKEN_MEM,         // mem.关键字（memoryoperation）
    TOKEN_CONST,       // const关键字（constantdefinition）
    TOKEN_VAR,         // var关键字（变量definition）
    TOKEN_FUNC,        // func关键字（functiondefinition）
    TOKEN_IF,          // if关键字（条件判断）
    TOKEN_ELSE,        // else关键字（条件分支）
    TOKEN_WHILE,       // while关键字（循环）
    TOKEN_FOR,         // for关键字（循环）
    TOKEN_IN,          // in关键字（for循环范围，如0..2 in ...）
    TOKEN_ID,          // 标识符（变量名、register名、function名等）
    TOKEN_NUM_DEC,     // 十basenumber（如123）
    TOKEN_NUM_HEX,     // 十六basenumber（如0x1234）
    TOKEN_CHAR,        // 字符constant（如'A'）
    TOKEN_EQUALS,      // =（assignment运算符）
    TOKEN_SEMICOLON,   // ;（语句结束符）
    TOKEN_LBRACE,      // {（code block开始）
    TOKEN_RBRACE,      // }（code block结束）
    TOKEN_LPAREN,      // (（括号开始）
    TOKEN_RPAREN,      // )（括号结束）
    TOKEN_LBRACKET,    // [（数组/memory地址开始，补充之前遗漏的）
    TOKEN_RBRACKET,    // ]（数组/memory地址结束，补充之前遗漏的）
    TOKEN_DOT,         // .（如reg.ax、mem.byte中的点）
    TOKEN_PLUS,        // +（加法运算符）
    TOKEN_MINUS,       // -（减法/负号运算符）
    TOKEN_ASTERISK,    // *（乘法/指针运算符）
    TOKEN_SLASH,       // /（除法运算符）
    TOKEN_AMPERSAND,   // &（位与运算符）
    TOKEN_PIPE,        // |（位或运算符）
    TOKEN_DOTDOT,      // ..（范围运算符，如0..2）
} TokenType;

// Token结构体（词法分析的输出单元）
typedef struct {
    TokenType type;    // Tokentype（如TOKEN_REG、TOKEN_NUM_HEX）
    char value[64];    // Token的文本值（如"ax"、"0x1234"、"reg."）
    uint32_t line;     // Token所在line（报错时定位用）
} Token;

// -------------------------- 通用constant --------------------------
#define MAX_TOKEN_LEN 64    // Token的最大长度（对应value数组）
#define MAX_ID_LEN 32       // 标识符（变量名、register名）的最大长度

#endif // TYPES_H
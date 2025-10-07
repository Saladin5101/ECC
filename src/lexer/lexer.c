#include "lexer.h"
#include "../common/utils.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h> // 为了使用malloc/free
// 辅助函数：读取下一个字符，并更新行号
static void next_char(Lexer* lexer) {
    lexer->current_char = fgetc(lexer->fp);
    if (lexer->current_char == '\n') {
        lexer->line++;  // 换行时行号+1
    }
}

// 辅助函数：跳过空白字符（空格、制表符、换行等）
static void skip_whitespace(Lexer* lexer) {
    while (lexer->current_char != EOF && isspace(lexer->current_char)) {
        next_char(lexer);
    }
}

// 辅助函数：跳过单行注释（// ...）
static void skip_comment(Lexer* lexer) {
    if (lexer->current_char == '/' && fpeek(lexer->fp) == '/') {  // 检查//
        next_char(lexer);  // 跳过第一个/
        next_char(lexer);  // 跳过第二个/
        // 一直读到换行或文件结束
        while (lexer->current_char != EOF && lexer->current_char != '\n') {
            next_char(lexer);
        }
    }
}

// 辅助函数：识别标识符或关键字（字母/下划线开头，后跟字母/数字/下划线）
static Token parse_identifier_or_keyword(Lexer* lexer) {
    Token tok;
    tok.line = lexer->line;
    int i = 0;

    // 读取标识符内容
    while (lexer->current_char != EOF && (isalnum(lexer->current_char) || lexer->current_char == '_')) {
        if (i < 63) {  // 避免缓冲区溢出（value大小64）
            tok.value[i++] = lexer->current_char;
        }
        next_char(lexer);
    }
    tok.value[i] = '\0';

    // 判断是否为关键字
    if (strcmp(tok.value, "use") == 0) {
        tok.type = TOKEN_USE;
    } else if (strcmp(tok.value, "const") == 0) {
        tok.type = TOKEN_CONST;
    } else if (strcmp(tok.value, "var") == 0) {
        tok.type = TOKEN_VAR;
    } else if (strcmp(tok.value, "func") == 0) {
        tok.type = TOKEN_FUNC;
    } else if (strcmp(tok.value, "if") == 0) {
        tok.type = TOKEN_IF;
    } else if (strcmp(tok.value, "else") == 0) {
        tok.type = TOKEN_ELSE;
    } else if (strcmp(tok.value, "while") == 0) {
        tok.type = TOKEN_WHILE;
    } else if (strcmp(tok.value, "for") == 0) {
        tok.type = TOKEN_FOR;
    } else if (strcmp(tok.value, "in") == 0) {
        tok.type = TOKEN_IN;
    } else if (strcmp(tok.value, "hlt") == 0) {  // x86指令作为关键字
        tok.type = TOKEN_ID;  // 暂时归为标识符，后续模块加载时验证
    } else {
        tok.type = TOKEN_ID;  // 普通标识符（变量名、寄存器名等）
    }

    return tok;
}

// 辅助函数：识别数字（十进制或十六进制）
static Token parse_number(Lexer* lexer) {
    Token tok;
    tok.line = lexer->line;
    int i = 0;

    // 检查是否为十六进制（0x开头）
    if (lexer->current_char == '0' && (fpeek(lexer->fp) == 'x' || fpeek(lexer->fp) == 'X')) {
        tok.type = TOKEN_NUM_HEX;
        tok.value[i++] = '0';
        next_char(lexer);  // 消耗'0'
        tok.value[i++] = tolower(fgetc(lexer->fp));  // 消耗'x'或'X'，统一存为x
        // 读取十六进制数字（0-9, a-f, A-F）
        while (lexer->current_char != EOF && isxdigit(lexer->current_char)) {
            if (i < 63) {
                tok.value[i++] = tolower(lexer->current_char);  // 统一小写
            }
            next_char(lexer);
        }
    } else {
        // 十进制数字
        tok.type = TOKEN_NUM_DEC;
        while (lexer->current_char != EOF && isdigit(lexer->current_char)) {
            if (i < 63) {
                tok.value[i++] = lexer->current_char;
            }
            next_char(lexer);
        }
    }

    tok.value[i] = '\0';
    return tok;
}

// 辅助函数：识别字符常量（如'A'）
static Token parse_char(Lexer* lexer) {
    Token tok;
    tok.type = TOKEN_CHAR;
    tok.line = lexer->line;
    int i = 0;

    next_char(lexer);  // 跳过开头的'
    if (lexer->current_char == EOF) {
        error("未闭合的字符常量（行号：%d）", lexer->line);
    }

    // 读取字符（暂不支持转义字符，如'\n'，后续扩展）
    tok.value[i++] = lexer->current_char;
    next_char(lexer);

    if (lexer->current_char != '\'') {
        error("未闭合的字符常量（行号：%d）", lexer->line);
    }
    next_char(lexer);  // 跳过结尾的'

    tok.value[i] = '\0';
    return tok;
}

// 核心函数：获取下一个Token
Token lexer_next_token(Lexer* lexer) {
    while (lexer->current_char != EOF) {
        skip_whitespace(lexer);
        skip_comment(lexer);  // 先跳过注释

        if (lexer->current_char == EOF) break;

        // 识别标识符或关键字（字母/下划线开头）
        if (isalpha(lexer->current_char) || lexer->current_char == '_') {
            return parse_identifier_or_keyword(lexer);
        }

        // 识别数字（0-9或0x开头）
        if (isdigit(lexer->current_char)) {
            return parse_number(lexer);
        }

        // 识别字符常量（'...'）
        if (lexer->current_char == '\'') {
            return parse_char(lexer);
        }

        // 识别reg.或mem.（关键字+点）
        if (lexer->current_char == 'r' && fpeek(lexer->fp) == 'e' && fpeek(lexer->fp + 1) == 'g' && fpeek(lexer->fp + 2) == '.') {
            Token tok;
            tok.type = TOKEN_REG;
            tok.line = lexer->line;
            strcpy(tok.value, "reg.");
            // 跳过'reg.'这4个字符
            next_char(lexer);  // r
            next_char(lexer);  // e
            next_char(lexer);  // g
            next_char(lexer);  // .
            return tok;
        }
        if (lexer->current_char == 'm' && fpeek(lexer->fp) == 'e' && fpeek(lexer->fp + 1) == 'm' && fpeek(lexer->fp + 2) == '.') {
            Token tok;
            tok.type = TOKEN_MEM;
            tok.line = lexer->line;
            strcpy(tok.value, "mem.");
            // 跳过'mem.'这4个字符
            next_char(lexer);  // m
            next_char(lexer);  // e
            next_char(lexer);  // m
            next_char(lexer);  // .
            return tok;
        }

        // 识别运算符/分隔符
        Token tok;
        tok.line = lexer->line;
        switch (lexer->current_char) {
            case '=':
                tok.type = TOKEN_EQUALS;
                strcpy(tok.value, "=");
                next_char(lexer);
                return tok;
            case ';':
                tok.type = TOKEN_SEMICOLON;
                strcpy(tok.value, ";");
                next_char(lexer);
                return tok;
            case '{':
                tok.type = TOKEN_LBRACE;
                strcpy(tok.value, "{");
                next_char(lexer);
                return tok;
            case '}':
                tok.type = TOKEN_RBRACE;
                strcpy(tok.value, "}");
                next_char(lexer);
                return tok;
            case '(':
                tok.type = TOKEN_LPAREN;
                strcpy(tok.value, "(");
                next_char(lexer);
                return tok;
            case ')':
                tok.type = TOKEN_RPAREN;
                strcpy(tok.value, ")");
                next_char(lexer);
                return tok;
            case '+':
                tok.type = TOKEN_PLUS;
                strcpy(tok.value, "+");
                next_char(lexer);
                return tok;
            case '-':
                tok.type = TOKEN_MINUS;
                strcpy(tok.value, "-");
                next_char(lexer);
                return tok;
            case '*':
                tok.type = TOKEN_ASTERISK;
                strcpy(tok.value, "*");
                next_char(lexer);
                return tok;
            case '/':
                tok.type = TOKEN_SLASH;
                strcpy(tok.value, "/");
                next_char(lexer);
                return tok;
            case '&':
                tok.type = TOKEN_AMPERSAND;
                strcpy(tok.value, "&");
                next_char(lexer);
                return tok;
            case '|':
                tok.type = TOKEN_PIPE;
                strcpy(tok.value, "|");
                next_char(lexer);
                return tok;
            case '.':
                // 检查是否为..（范围运算符）
                if (fpeek(lexer->fp) == '.') {
                    tok.type = TOKEN_DOTDOT;
                    strcpy(tok.value, "..");
                    next_char(lexer);  // 跳过第一个.
                    next_char(lexer);  // 跳过第二个.
                    return tok;
                } else {
                    // 单个.（如mem.byte中的.，后续由parser处理）
                    tok.type = TOKEN_DOT;
                    strcpy(tok.value, ".");
                    next_char(lexer);
                    return tok;
                }
            default:
                error("未知字符：%c（行号：%d）", lexer->current_char, lexer->line);
        }
    }

    // 到达文件末尾
    Token eof_tok;
    eof_tok.type = TOKEN_EOF;
    eof_tok.line = lexer->line;
    eof_tok.value[0] = '\0';
    return eof_tok;
}

// 初始化和释放函数（之前的框架基础上补充）
Lexer* lexer_init(FILE* fp) {
    Lexer* lexer = malloc(sizeof(Lexer));
    if (!lexer) error("内存分配失败（lexer_init）");
    lexer->fp = fp;
    lexer->line = 1;
    lexer->current_char = fgetc(fp);  // 预读第一个字符
    return lexer;
}

void lexer_free(Lexer* lexer) {
    if (lexer) free(lexer);
}
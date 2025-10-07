#include "utils.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// -------------------------- 错误处理实现 --------------------------
void error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    // 统一错误前缀，便于识别
    fprintf(stderr, "[ERROR] ");
    vfprintf(stderr, format, args);  // 处理可变参数
    fprintf(stderr, "\n");           // 自动换行
    
    va_end(args);
    exit(1);  // 报错后直接退出，避免后续错误
}
// -------------------------- 带组信息的错误提示 --------------------------
void error_with_group(const char* id, const char* group) {
    error("E: cannot find '%s' in file, it's not in group '%s'", id, group);
}
/* -------------------------- 带文件名和行号的错误提示 --------------------------
 *  void error_with_file_line(const char* filename, int line, const char* format, ...) {
 *  va_list args;
 *  va_start(args, format);
 *  fprintf(stderr, "[ERROR] In file '%s' at line %d: ", filename, line);
 *  vfprintf(stderr, format, args);
 *  fprintf(stderr, "\n");
 *  va_end(args);
}*/
// 以上代码暂时预留，后续可根据需要补充
// -------------------------- 字符串转数字实现 --------------------------
uint32_t str_to_hex(const char* s) {
    if (s == NULL || *s == '\0') {
        error("空字符串无法转换为十六进制数字");
    }

    uint32_t result = 0;
    const char* ptr = s;

    // 跳过0x或0X前缀
    if (ptr[0] == '0' && (ptr[1] == 'x' || ptr[1] == 'X')) {
        ptr += 2;
        // 若前缀后无内容（如"0x"），报错
        if (*ptr == '\0') {
            error("无效的十六进制数字：%s（仅含前缀）", s);
        }
    }

    // 逐个字符解析
    while (*ptr != '\0') {
        char c = tolower(*ptr);  // 不区分大小写（A-F和a-f一样）
        if (isdigit(c)) {
            // 数字字符（0-9）
            result = result * 16 + (c - '0');
        } else if (c >= 'a' && c <= 'f') {
            // 字母字符（a-f）
            result = result * 16 + (c - 'a' + 10);
        } else {
            // 非法字符
            error("无效的十六进制字符：%c（数字：%s）", c, s);
        }
        ptr++;
    }

    return result;
}

uint32_t str_to_dec(const char* s) {
    if (s == NULL || *s == '\0') {
        error("空字符串无法转换为十进制数字");
    }

    uint32_t result = 0;
    const char* ptr = s;

    // 逐个字符解析（仅允许0-9）
    while (*ptr != '\0') {
        if (!isdigit(*ptr)) {
            error("无效的十进制字符：%c（数字：%s）", *ptr, s);
        }
        result = result * 10 + (*ptr - '0');
        ptr++;
    }

    return result;
}

// -------------------------- Token辅助实现 --------------------------
const char* token_type_to_str(TokenType type) {
    switch (type) {
        case TOKEN_EOF:         return "TOKEN_EOF";
        case TOKEN_USE:         return "TOKEN_USE";
        case TOKEN_REG:         return "TOKEN_REG";
        case TOKEN_MEM:         return "TOKEN_MEM";
        case TOKEN_CONST:       return "TOKEN_CONST";
        case TOKEN_VAR:         return "TOKEN_VAR";
        case TOKEN_FUNC:        return "TOKEN_FUNC";
        case TOKEN_IF:          return "TOKEN_IF";
        case TOKEN_ELSE:        return "TOKEN_ELSE";
        case TOKEN_WHILE:       return "TOKEN_WHILE";
        case TOKEN_FOR:         return "TOKEN_FOR";
        case TOKEN_IN:          return "TOKEN_IN";
        case TOKEN_ID:          return "TOKEN_ID";
        case TOKEN_NUM_DEC:     return "TOKEN_NUM_DEC";
        case TOKEN_NUM_HEX:     return "TOKEN_NUM_HEX";
        case TOKEN_CHAR:        return "TOKEN_CHAR";
        case TOKEN_EQUALS:      return "TOKEN_EQUALS";
        case TOKEN_SEMICOLON:   return "TOKEN_SEMICOLON";
        case TOKEN_LBRACE:      return "TOKEN_LBRACE";
        case TOKEN_RBRACE:      return "TOKEN_RBRACE";
        case TOKEN_LPAREN:      return "TOKEN_LPAREN";
        case TOKEN_RPAREN:      return "TOKEN_RPAREN";
        case TOKEN_LBRACKET:    return "TOKEN_LBRACKET";
        case TOKEN_RBRACKET:    return "TOKEN_RBRACKET";
        case TOKEN_DOT:         return "TOKEN_DOT";
        case TOKEN_PLUS:        return "TOKEN_PLUS";
        case TOKEN_MINUS:       return "TOKEN_MINUS";
        case TOKEN_ASTERISK:    return "TOKEN_ASTERISK";
        case TOKEN_SLASH:       return "TOKEN_SLASH";
        case TOKEN_AMPERSAND:   return "TOKEN_AMPERSAND";
        case TOKEN_PIPE:        return "TOKEN_PIPE";
        case TOKEN_DOTDOT:      return "TOKEN_DOTDOT";
        default:                return "TOKEN_UNKNOWN";
    }
}

// -------------------------- 内存操作实现 --------------------------
void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr == NULL) {
        error("内存分配失败（需要大小：%zu字节）", size);
    }
    return ptr;
}
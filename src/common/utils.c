#include "utils.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// -------------------------- Error handling implementation --------------------------
void error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    // Unified error prefix for easy identification
    fprintf(stderr, "[ERROR] ");
    vfprintf(stderr, format, args);  // Handle variable arguments
    fprintf(stderr, "\n");           // Automatic newline
    
    va_end(args);
    exit(1);  // Exit directly after error to avoid subsequent errors
}
// -------------------------- Error message with group information --------------------------
void error_with_group(const char* id, const char* group) {
    error("E: cannot find '%s' in file, it's not in group '%s'", id, group);
}
/* -------------------------- Error message with filename and line number --------------------------
 *  void error_with_file_line(const char* filename, int line, const char* format, ...) {
 *  va_list args;
 *  va_start(args, format);
 *  fprintf(stderr, "[ERROR] In file '%s' at line %d: ", filename, line);
 *  vfprintf(stderr, format, args);
 *  fprintf(stderr, "\n");
 *  va_end(args);
}*/
// Above code is reserved for future use
// -------------------------- String to number conversion implementation --------------------------
uint32_t str_to_hex(const char* s) {
    if (s == NULL || *s == '\0') {
        error("Empty string cannot be converted to hexadecimal number");
    }

    uint32_t result = 0;
    const char* ptr = s;

    // Skip 0x or 0X prefix
    if (ptr[0] == '0' && (ptr[1] == 'x' || ptr[1] == 'X')) {
        ptr += 2;
        // If no content after prefix (like "0x"), report error
        if (*ptr == '\0') {
            error("Invalid hexadecimal number: %s (prefix only)", s);
        }
    }

    // Parse character by character
    while (*ptr != '\0') {
        char c = tolower(*ptr);  // Case insensitive (A-F and a-f are the same)
        if (isdigit(c)) {
            // Digit character (0-9)
            result = result * 16 + (c - '0');
        } else if (c >= 'a' && c <= 'f') {
            // Letter character (a-f)
            result = result * 16 + (c - 'a' + 10);
        } else {
            // Invalid character
            error("Invalid hexadecimal character: %c (number: %s)", c, s);
        }
        ptr++;
    }

    return result;
}

uint32_t str_to_dec(const char* s) {
    if (s == NULL || *s == '\0') {
        error("Empty string cannot be converted to decimal number");
    }

    uint32_t result = 0;
    const char* ptr = s;

    // Parse character by character (only allow 0-9)
    while (*ptr != '\0') {
        if (!isdigit(*ptr)) {
            error("Invalid decimal character: %c (number: %s)", *ptr, s);
        }
        result = result * 10 + (*ptr - '0');
        ptr++;
    }

    return result;
}

// -------------------------- TokenHelperimplementation --------------------------
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

// -------------------------- Memory operation implementation --------------------------
void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr == NULL) {
        error("Memory allocation failed (required size: %zu bytes)", size);
    }
    return ptr;
}
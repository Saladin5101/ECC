#include "lexer.h"
#include "../common/utils.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h> // For malloc/free usage
// Helper function: read next character and update line number
static void next_char(Lexer* lexer) {
    lexer->current_char = fgetc(lexer->fp);
    if (lexer->current_char == '\n') {
        lexer->line++;  // Increment line number on newline
    }
}

// Helper function: skip whitespace (space, tab, newline, etc.)
static void skip_whitespace(Lexer* lexer) {
    while (lexer->current_char != EOF && isspace(lexer->current_char)) {
        next_char(lexer);
    }
}

// Helper function: skip single line comments (// ...)
static void skip_comment(Lexer* lexer) {
    if (lexer->current_char == '/' && fpeek(lexer->fp) == '/') {  // Check //
        next_char(lexer);  // Skip first /
        next_char(lexer);  // Skip second /
        // Read until newline or EOF
        while (lexer->current_char != EOF && lexer->current_char != '\n') {
            next_char(lexer);
        }
    }
}

// Helper function: identify identifier or keyword (letter/underscore start, followed by letter/digit/underscore)
static Token parse_identifier_or_keyword(Lexer* lexer) {
    Token tok;
    tok.line = lexer->line;
    int i = 0;

    // Read identifier content
    while (lexer->current_char != EOF && (isalnum(lexer->current_char) || lexer->current_char == '_')) {
        if (i < 63) {  // Avoid buffer overflow (value size 64)
            tok.value[i++] = lexer->current_char;
        }
        next_char(lexer);
    }
    tok.value[i] = '\0';

    // Check if it is a keyword
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
    } else if (strcmp(tok.value, "hlt") == 0) {  // x86 instruction as keyword
        tok.type = TOKEN_ID;  // Temporarily classified as identifier, verify when module loads
    } else {
        tok.type = TOKEN_ID;  // Regular identifier (variable name, register name, etc.)
    }

    return tok;
}

// Helper function: identify number (decimal or hexadecimal)
static Token parse_number(Lexer* lexer) {
    Token tok;
    tok.line = lexer->line;
    int i = 0;

    // Check if hexadecimal (starts with 0x)
    if (lexer->current_char == '0' && (fpeek(lexer->fp) == 'x' || fpeek(lexer->fp) == 'X')) {
        tok.type = TOKEN_NUM_HEX;
        tok.value[i++] = '0';
        next_char(lexer);  // Consume '0'
        tok.value[i++] = tolower(lexer->current_char);  // Consume 'x' or 'X'
        next_char(lexer);  // Move to next character
        // Read hexadecimal digits (0-9, a-f, A-F)
        while (lexer->current_char != EOF && isxdigit(lexer->current_char)) {
            if (i < 63) {
                tok.value[i++] = tolower(lexer->current_char);  // Unified lowercase
            }
            next_char(lexer);
        }
    } else {
        // Decimal number
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

// Helper function: identify character constant (like 'A')
static Token parse_char(Lexer* lexer) {
    Token tok;
    tok.type = TOKEN_CHAR;
    tok.line = lexer->line;
    int i = 0;

    next_char(lexer);  // Skip opening quote
    if (lexer->current_char == EOF) {
        error("Unclosed character constant (line: %d)", lexer->line);
    }

    // Read character (escape characters like '\n' not supported yet, future extension)
    tok.value[i++] = lexer->current_char;
    next_char(lexer);

    if (lexer->current_char != '\'') {
        error("Unclosed character constant (line: %d)", lexer->line);
    }
    next_char(lexer);  // Skip closing quote

    tok.value[i] = '\0';
    return tok;
}

// Core function: get next Token
Token lexer_next_token(Lexer* lexer) {
    while (lexer->current_char != EOF) {
        skip_whitespace(lexer);
        skip_comment(lexer);  // Skip comments first

        if (lexer->current_char == EOF) break;

        // Check special keywords reg. and mem. first
        if (lexer->current_char == 'r') {
            int pos = ftell(lexer->fp);
            char buffer[4];
            if (fread(buffer, 1, 3, lexer->fp) == 3 && 
                buffer[0] == 'e' && buffer[1] == 'g' && buffer[2] == '.') {
                Token tok;
                tok.type = TOKEN_REG;
                tok.line = lexer->line;
                strcpy(tok.value, "reg.");
                next_char(lexer);  // Update current_char
                return tok;
            } else {
                fseek(lexer->fp, pos, SEEK_SET);
                return parse_identifier_or_keyword(lexer);
            }
        }
        if (lexer->current_char == 'm') {
            int pos = ftell(lexer->fp);
            char buffer[4];
            if (fread(buffer, 1, 3, lexer->fp) == 3 && 
                buffer[0] == 'e' && buffer[1] == 'm' && buffer[2] == '.') {
                Token tok;
                tok.type = TOKEN_MEM;
                tok.line = lexer->line;
                strcpy(tok.value, "mem.");
                next_char(lexer);  // Update current_char
                return tok;
            } else {
                fseek(lexer->fp, pos, SEEK_SET);
                return parse_identifier_or_keyword(lexer);
            }
        }
        
        // Identify identifier or keyword (letter/underscore start)
        if (isalpha(lexer->current_char) || lexer->current_char == '_') {
            return parse_identifier_or_keyword(lexer);
        }

        // Identify number (0-9 or 0x start)
        if (isdigit(lexer->current_char)) {
            return parse_number(lexer);
        }

        // Identify character constant ('...')
        if (lexer->current_char == '\'') {
            return parse_char(lexer);
        }



        // Identify operator/separator
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
                // Check if .. (range operator)
                if (fpeek(lexer->fp) == '.') {
                    tok.type = TOKEN_DOTDOT;
                    strcpy(tok.value, "..");
                    next_char(lexer);  // Skip first .
                    next_char(lexer);  // Skip second .
                    return tok;
                } else {
                    // Single . (like . in mem.byte, handled by parser later)
                    tok.type = TOKEN_DOT;
                    strcpy(tok.value, ".");
                    next_char(lexer);
                    return tok;
                }
            default:
                error("Unknown character: %c (line: %d)", lexer->current_char, lexer->line);
        }
    }

    // Reached end of file
    Token eof_tok;
    eof_tok.type = TOKEN_EOF;
    eof_tok.line = lexer->line;
    eof_tok.value[0] = '\0';
    return eof_tok;
}

// Initialize and free functions (supplemented on previous framework)
Lexer* lexer_init(FILE* fp) {
    Lexer* lexer = malloc(sizeof(Lexer));
    if (!lexer) error("Memory allocation failed (lexer_init)");
    lexer->fp = fp;
    lexer->line = 1;
    lexer->current_char = fgetc(fp);  // Pre-read first character
    return lexer;
}

void lexer_free(Lexer* lexer) {
    if (lexer) free(lexer);
}
#ifndef PARSER_H
#define PARSER_H

#include "common/types.h"  // 依赖TokenType
#include "lexer/lexer.h"   // 依赖Lexer和Token

// -------------------------- AST节点类型 --------------------------
// 对应ELFCOST的核心语法单元
typedef enum {
    AST_REG_ASSIGN,    // 寄存器赋值：reg.ax = 0x1234
    AST_MEM_ASSIGN,    // 内存赋值：mem.byte[0xb8000] = 'A'
    AST_CONST_DEF,     // 常量定义：const VIDEO_MEM = 0xb8000
    AST_FUNC_CALL,     // 函数调用：print_char('E', 0, 0)
    AST_FUNC_DEF,      // 函数定义：func print_char(c,x,y) { ... }
    AST_BLOCK,         // 代码块：{ ... }（函数体、if体等）
    AST_EOF            // 结束节点
} AstNodeType;

// -------------------------- 基础AST节点（所有节点的父类） --------------------------
typedef struct AstNode {
    AstNodeType type;          // 节点类型
    struct AstNode* next;      // 链表指针（用于串联多个节点，比如函数体里的多条语句）
    int line;                  // 行号（报错用）
} AstNode;

// -------------------------- 常量表达式（用于存储值，比如0x1234、'A'） --------------------------
// 存储常量的值（支持数字、字符）
typedef struct {
    enum { CONST_NUM, CONST_CHAR } type;  // 常量类型
    union {
        unsigned int num_val;             // 数字值（十进制/十六进制）
        char char_val;                    // 字符值
    } value;
} ConstExpr;

// -------------------------- 寄存器赋值节点 --------------------------
typedef struct {
    AstNode base;               // 继承基础节点
    char reg_name[32];          // 寄存器名：ax、bx等
    ConstExpr value;            // 赋值内容（比如0x1234）
} RegAssignNode;

// -------------------------- 内存赋值节点 --------------------------
typedef struct {
    AstNode base;               // 继承基础节点
    char mem_width[16];         // 内存宽度：byte、word、dword
    ConstExpr addr;             // 内存地址（比如0xb8000）
    ConstExpr value;            // 赋值内容（比如'A'）
} MemAssignNode;

// -------------------------- 常量定义节点 --------------------------
typedef struct {
    AstNode base;               // 继承基础节点
    char const_name[32];        // 常量名：VIDEO_MEM、MBR_SIG等
    ConstExpr value;            // 常量值（比如0xb8000）
} ConstDefNode;

// -------------------------- 函数调用节点 --------------------------
typedef struct {
    AstNode base;               // 继承基础节点
    char func_name[32];         // 函数名：print_char、uart_init等
    ConstExpr* args;            // 函数参数列表（比如['E', 0, 0]）
    int arg_count;              // 参数个数
} FuncCallNode;

// -------------------------- 函数定义节点 --------------------------
typedef struct {
    AstNode base;               // 继承基础节点
    char func_name[32];         // 函数名：print_char等
    char* params;               // 参数列表（比如"c,x,y"，暂时简化存储）
    int param_count;            // 参数个数
    AstNode* body;              // 函数体（代码块，多条语句的链表）
} FuncDefNode;

// -------------------------- 代码块节点（存储多条语句） --------------------------
typedef struct {
    AstNode base;               // 继承基础节点
    AstNode* statements;        // 语句链表（比如函数体里的寄存器赋值、函数调用）
} BlockNode;

// -------------------------- 解析器状态 --------------------------
typedef struct {
    Lexer* lexer;       // 关联的词法分析器（用于获取Token）
    Token current_tok;  // 当前Token（预读一个Token，用于语法判断）
} Parser;

// -------------------------- 解析器核心接口 --------------------------
// 1. 初始化解析器：传入lexer，预读第一个Token
Parser* parser_init(Lexer* lexer);

// 2. 匹配指定Token：如果当前Token是目标类型，消耗并读下一个；否则报错
void parser_match(Parser* parser, TokenType expected_type);

// 3. 解析整个ELFCOST文件，生成AST根节点
AstNode* parser_parse_file(Parser* parser);

// 4. 解析单个语句（比如reg赋值、mem赋值、函数调用）
AstNode* parser_parse_statement(Parser* parser);

// 5. 解析常量表达式（比如0x1234、'A'、VIDEO_MEM）
ConstExpr parser_parse_const_expr(Parser* parser);

// 6. 释放AST（避免内存泄漏）
void ast_free(AstNode* root);

// 7. 释放解析器
void parser_free(Parser* parser);

#endif // PARSER_H
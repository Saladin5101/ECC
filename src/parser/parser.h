#ifndef PARSER_H
#define PARSER_H

#include "common/types.h"  // 依赖TokenType
#include "lexer/lexer.h"   // 依赖Lexer和Token

// -------------------------- AST节点type --------------------------
// 对应ELFCOST的核心语法单元
typedef enum {
    AST_REG_ASSIGN,    // registerassignment：reg.ax = 0x1234
    AST_MEM_ASSIGN,    // memoryassignment：mem.byte[0xb8000] = 'A'
    AST_CONST_DEF,     // constantdefinition：const VIDEO_MEM = 0xb8000
    AST_FUNC_CALL,     // function调用：print_char('E', 0, 0)
    AST_FUNC_DEF,      // functiondefinition：func print_char(c,x,y) { ... }
    AST_BLOCK,         // code block：{ ... }（function体、if体等）
    AST_EOF            // 结束节点
} AstNodeType;

// -------------------------- 基础AST节点（所有节点的父类） --------------------------
typedef struct AstNode {
    AstNodeType type;          // 节点type
    struct AstNode* next;      // 链表指针（用于串联多个节点，比如function体里的多条语句）
    int line;                  // line（报错用）
} AstNode;

// -------------------------- constant表达式（用于存储值，比如0x1234、'A'） --------------------------
// 存储constant的值（supportnumber、字符）
typedef struct {
    enum { CONST_NUM, CONST_CHAR } type;  // constanttype
    union {
        unsigned int num_val;             // number值（十base/十六base）
        char char_val;                    // 字符值
    } value;
} ConstExpr;

// -------------------------- registerassignment节点 --------------------------
typedef struct {
    AstNode base;               // 继承基础节点
    char reg_name[32];          // register名：ax、bx等
    ConstExpr value;            // assignment内容（比如0x1234）
} RegAssignNode;

// -------------------------- memoryassignment节点 --------------------------
typedef struct {
    AstNode base;               // 继承基础节点
    char mem_width[16];         // memory宽度：byte、word、dword
    ConstExpr addr;             // memory地址（比如0xb8000）
    ConstExpr value;            // assignment内容（比如'A'）
} MemAssignNode;

// -------------------------- constantdefinition节点 --------------------------
typedef struct {
    AstNode base;               // 继承基础节点
    char const_name[32];        // constant名：VIDEO_MEM、MBR_SIG等
    ConstExpr value;            // constant值（比如0xb8000）
} ConstDefNode;

// -------------------------- function调用节点 --------------------------
typedef struct {
    AstNode base;               // 继承基础节点
    char func_name[32];         // function名：print_char、uart_init等
    ConstExpr* args;            // functionparameter列表（比如['E', 0, 0]）
    int arg_count;              // parameter个数
} FuncCallNode;

// -------------------------- functiondefinition节点 --------------------------
typedef struct {
    AstNode base;               // 继承基础节点
    char func_name[32];         // function名：print_char等
    char* params;               // parameter列表（比如"c,x,y"，Temporarily简化存储）
    int param_count;            // parameter个数
    AstNode* body;              // function体（code block，多条语句的链表）
} FuncDefNode;

// -------------------------- code block节点（存储多条语句） --------------------------
typedef struct {
    AstNode base;               // 继承基础节点
    AstNode* statements;        // 语句链表（比如function体里的registerassignment、function调用）
} BlockNode;

// -------------------------- 解析器状态 --------------------------
typedef struct {
    Lexer* lexer;       // 关联的lexer（用于获取Token）
    Token current_tok;  // currentToken（预读一个Token，用于语法判断）
} Parser;

// -------------------------- 解析器核心接口 --------------------------
// 1. 初始化解析器：传入lexer，预读第一个Token
Parser* parser_init(Lexer* lexer);

// 2. 匹配specifiedToken：如果currentToken是目标type，消耗并读下一个；否则报错
void parser_match(Parser* parser, TokenType expected_type);

// 3. 解析整个ELFCOST文件，生成AST根节点
AstNode* parser_parse_file(Parser* parser);

// 4. 解析单个语句（比如regassignment、memassignment、function调用）
AstNode* parser_parse_statement(Parser* parser);

// 5. 解析constant表达式（比如0x1234、'A'、VIDEO_MEM）
ConstExpr parser_parse_const_expr(Parser* parser);

// 6. 释放AST（避免memory泄漏）
void ast_free(AstNode* root);

// 7. 释放解析器
void parser_free(Parser* parser);

#endif // PARSER_H
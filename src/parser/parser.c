#include "parser.h"
#include "../common/utils.h"
#include <string.h>
#include <stdlib.h>

// -------------------------- 辅助函数：初始化基础AST节点 --------------------------
static AstNode* ast_node_init(AstNodeType type, int line) {
    AstNode* node = malloc(sizeof(AstNode));
    if (!node) error("内存分配失败（ast_node_init）");
    node->type = type;
    node->next = NULL;
    node->line = line;
    return node;
}

// -------------------------- 1. 解析器初始化 --------------------------
Parser* parser_init(Lexer* lexer) {
    Parser* parser = malloc(sizeof(Parser));
    if (!parser) error("内存分配失败（parser_init）");
    parser->lexer = lexer;
    // 预读第一个Token（语法分析的关键：通过当前Token判断下一步解析逻辑）
    parser->current_tok = lexer_next_token(lexer);
    return parser;
}

// -------------------------- 2. 匹配Token（核心辅助函数） --------------------------
void parser_match(Parser* parser, TokenType expected_type) {
    if (parser->current_tok.type == expected_type) {
        // 匹配成功：消耗当前Token，读下一个
        parser->current_tok = lexer_next_token(parser->lexer);
    } else {
        // 匹配失败：报语法错误（带上行号，方便定位）
        error("语法错误（行号：%d）：预期%s，实际%s（值：%s）",
              parser->current_tok.line,
              token_type_to_str(expected_type),
              token_type_to_str(parser->current_tok.type),
              parser->current_tok.value);
    }
}

// -------------------------- 3. 解析常量表达式（比如0x1234、'A'） --------------------------
// 暂时只支持“直接常量”（后续再加变量引用，比如VIDEO_MEM）
ConstExpr parser_parse_const_expr(Parser* parser) {
    ConstExpr expr;
    Token tok = parser->current_tok;

    switch (tok.type) {
        // 十六进制数字：0x1234
        case TOKEN_NUM_HEX:
            expr.type = CONST_NUM;
            expr.value.num_val = str_to_hex(tok.value);  // 用common里的工具函数转数值
            parser_match(parser, TOKEN_NUM_HEX);
            break;
        // 十进制数字：123
        case TOKEN_NUM_DEC:
            expr.type = CONST_NUM;
            expr.value.num_val = str_to_dec(tok.value);
            parser_match(parser, TOKEN_NUM_DEC);
            break;
        // 字符：'A'
        case TOKEN_CHAR:
            expr.type = CONST_CHAR;
            expr.value.char_val = tok.value[0];  // tok.value存储的是字符本身（比如"A"）
            parser_match(parser, TOKEN_CHAR);
            break;
        // 其他类型（比如标识符，后续扩展变量引用）
        default:
            error("语法错误（行号：%d）：预期常量，实际%s（值：%s）",
                  tok.line, token_type_to_str(tok.type), tok.value);
    }
    return expr;
}

// -------------------------- 4. 解析寄存器赋值语句（reg.ax = 0x1234;） --------------------------
static AstNode* parser_parse_reg_assign(Parser* parser) {
    int line = parser->current_tok.line;
    RegAssignNode* node = NULL;

    // 步骤1：匹配"reg."（当前Token应该是TOKEN_REG）
    parser_match(parser, TOKEN_REG);

    // 步骤2：匹配寄存器名（比如ax，Token类型是TOKEN_ID）
    Token reg_tok = parser->current_tok;
    parser_match(parser, TOKEN_ID);

    // 检查寄存器名是否合法（比如ax、bx等）
    error_with_group(reg_tok.value, "x86_mem_registers");

    // 步骤3：匹配"="
    parser_match(parser, TOKEN_EQUALS);

    // 步骤4：解析赋值的常量表达式（比如0x1234）
    ConstExpr value = parser_parse_const_expr(parser);

    // 步骤5：匹配";"（语句结束）
    parser_match(parser, TOKEN_SEMICOLON);

    // 步骤6：构建寄存器赋值AST节点
    node = malloc(sizeof(RegAssignNode));
    if (!node) error("内存分配失败（parser_parse_reg_assign）");
    node->base = *ast_node_init(AST_REG_ASSIGN, line);  // 初始化基础节点
    strncpy(node->reg_name, reg_tok.value, sizeof(node->reg_name)-1);
    node->value = value;

    return (AstNode*)node;  // 向上转型为基础AstNode
}

// -------------------------- 5. 解析常量定义语句（const VIDEO_MEM = 0xb8000;） --------------------------
static AstNode* parser_parse_const_def(Parser* parser) {
    int line = parser->current_tok.line;
    ConstDefNode* node = NULL;

    // 步骤1：匹配"const"关键字
    parser_match(parser, TOKEN_CONST);

    // 步骤2：匹配常量名（比如VIDEO_MEM，TOKEN_ID）
    Token const_tok = parser->current_tok;
    parser_match(parser, TOKEN_ID);

    // 步骤3：匹配"="
    parser_match(parser, TOKEN_EQUALS);

    // 步骤4：解析常量值（比如0xb8000）
    ConstExpr value = parser_parse_const_expr(parser);

    // 步骤5：匹配";"
    parser_match(parser, TOKEN_SEMICOLON);

    // 步骤6：构建常量定义AST节点
    node = malloc(sizeof(ConstDefNode));
    if (!node) error("内存分配失败（parser_parse_const_def）");
    node->base = *ast_node_init(AST_CONST_DEF, line);
    strncpy(node->const_name, const_tok.value, sizeof(node->const_name)-1);
    node->value = value;

    return (AstNode*)node;
}

// -------------------------- 6. 解析单个语句（根据当前Token判断语句类型） --------------------------
AstNode* parser_parse_statement(Parser* parser) {
    switch (parser->current_tok.type) {
        // 如果当前Token是"reg."，解析寄存器赋值
        case TOKEN_REG:
            return parser_parse_reg_assign(parser);
        // 如果当前Token是"const"，解析常量定义
        case TOKEN_CONST:
            return parser_parse_const_def(parser);
        // 其他语句类型（mem赋值、函数调用等）后续补充
        case TOKEN_MEM:
            error("暂未实现内存赋值解析（行号：%d）", parser->current_tok.line);
            break;
        case TOKEN_ID:  // 可能是函数调用（比如print_char(...)）
            error("暂未实现函数调用解析（行号：%d，标识符：%s）",
                  parser->current_tok.line, parser->current_tok.value);
            break;
        // 结束节点
        case TOKEN_EOF:
            return ast_node_init(AST_EOF, parser->current_tok.line);
        // 未知语句类型
        default:
            error("语法错误（行号：%d）：未知语句类型，Token：%s（值：%s）",
                  parser->current_tok.line,
                  token_type_to_str(parser->current_tok.type),
                  parser->current_tok.value);
    }
    return NULL;
}

// -------------------------- 7. 解析整个文件（串联所有语句） --------------------------
/*AstNode* parser_parse_file(Parser* parser) {
    AstNode* root = ast_node_init(AST_BLOCK, 1);  // 根节点是代码块
    AstNode* current = root->statements;          // 指向当前语句（用于构建链表）

    // 循环解析所有语句，直到遇到EOF
    while (parser->current_tok.type != TOKEN_EOF) {
        AstNode* stmt = parser_parse_statement(parser);
        if (!root->statements) {
            root->statements = stmt;  // 第一个语句
        } else {
            current->next = stmt;     // 后续语句挂到链表后面
        }
        current = stmt;
    }

    return root;
}*/ // 原本的代码，作为参考暂且保留

AstNode* parser_parse_file(Parser* parser) {
    // 根节点是BlockNode，显式分配并初始化
    BlockNode* root_block = safe_malloc(sizeof(BlockNode));
    root_block->base = *ast_node_init(AST_BLOCK, 1);
    root_block->statements = NULL;  // 初始化语句链表

    AstNode* current_stmt = NULL;

    // 循环解析所有语句
    while (parser->current_tok.type != TOKEN_EOF) {
        AstNode* stmt = parser_parse_statement(parser);
        if (!root_block->statements) {
            root_block->statements = stmt;  // 第一个语句
            current_stmt = stmt;
        } else {
            current_stmt->next = stmt;      // 后续语句挂到链表
            current_stmt = stmt;
        }
    }

    return (AstNode*)root_block;  // 转型为基类指针返回
}

// -------------------------- 8. 释放AST（递归释放所有节点，避免内存泄漏） --------------------------
void ast_free(AstNode* root) {
    if (!root) return;

    // 递归释放链表下一个节点
    ast_free(root->next);

    // 根据节点类型释放具体内容（如果有动态分配的字段）
    switch (root->type) {
        case AST_FUNC_CALL: {
            FuncCallNode* call_node = (FuncCallNode*)root;
            if (call_node->args) free(call_node->args);  // 释放参数列表
            break;
        }
        case AST_FUNC_DEF: {
            FuncDefNode* def_node = (FuncDefNode*)root;
            if (def_node->params) free(def_node->params);  // 释放参数名
            ast_free(def_node->body);                      // 递归释放函数体
            break;
        }
        case AST_BLOCK: {
            BlockNode* block_node = (BlockNode*)root;
            ast_free(block_node->statements);  // 释放代码块里的语句
            break;
        }
        // 其他节点（reg/mem赋值、const定义）没有额外动态字段，直接free即可
        default:
            break;
    }

    // 释放当前节点
    free(root);
}

// -------------------------- 9. 释放解析器 --------------------------
void parser_free(Parser* parser) {
    if (parser) free(parser);
}

// -------------------------- 辅助函数：TokenType转字符串（报错用，暂时复制lexer的实现） --------------------------
const char* token_type_to_str(TokenType type) {
    switch (type) {
        case TOKEN_USE: return "TOKEN_USE";
        case TOKEN_REG: return "TOKEN_REG";
        case TOKEN_MEM: return "TOKEN_MEM";
        case TOKEN_CONST: return "TOKEN_CONST";
        case TOKEN_VAR: return "TOKEN_VAR";
        case TOKEN_FUNC: return "TOKEN_FUNC";
        case TOKEN_ID: return "TOKEN_ID";
        case TOKEN_NUM_HEX: return "TOKEN_NUM_HEX";
        case TOKEN_NUM_DEC: return "TOKEN_NUM_DEC";
        case TOKEN_CHAR: return "TOKEN_CHAR";
        case TOKEN_EQUALS: return "TOKEN_EQUALS";
        case TOKEN_SEMICOLON: return "TOKEN_SEMICOLON";
        case TOKEN_EOF: return "TOKEN_EOF";
        default: return "未知Token类型";
    }
}
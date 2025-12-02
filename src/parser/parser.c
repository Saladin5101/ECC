#include "parser.h"
#include "../common/utils.h"
#include <string.h>
#include <stdlib.h>

// -------------------------- Helperfunction：初始化基础AST节点 --------------------------
static AstNode* ast_node_init(AstNodeType type, int line) {
    AstNode* node = malloc(sizeof(AstNode));
    if (!node) error("memoryallocationfailed（ast_node_init）");
    node->type = type;
    node->next = NULL;
    node->line = line;
    return node;
}

// -------------------------- 1. 解析器初始化 --------------------------
Parser* parser_init(Lexer* lexer) {
    Parser* parser = malloc(sizeof(Parser));
    if (!parser) error("memoryallocationfailed（parser_init）");
    parser->lexer = lexer;
    // 预读第一个Token（语法分析的关键：通过currentToken判断下一步解析逻辑）
    parser->current_tok = lexer_next_token(lexer);
    return parser;
}

// -------------------------- 2. 匹配Token（核心Helperfunction） --------------------------
void parser_match(Parser* parser, TokenType expected_type) {
    if (parser->current_tok.type == expected_type) {
        // 匹配successfully：消耗currentToken，读下一个
        parser->current_tok = lexer_next_token(parser->lexer);
    } else {
        // 匹配failed：报Syntax error（带上line，方便定位）
        error("Syntax error（line：%d）：Expected%s，Actual%s（值：%s）",
              parser->current_tok.line,
              token_type_to_str(expected_type),
              token_type_to_str(parser->current_tok.type),
              parser->current_tok.value);
    }
}

// -------------------------- 3. 解析constant表达式（比如0x1234、'A'） --------------------------
// Temporarily只support“直接constant”（后续再加变量引用，比如VIDEO_MEM）
ConstExpr parser_parse_const_expr(Parser* parser) {
    ConstExpr expr;
    Token tok = parser->current_tok;

    switch (tok.type) {
        // 十六basenumber：0x1234
        case TOKEN_NUM_HEX:
            expr.type = CONST_NUM;
            expr.value.num_val = str_to_hex(tok.value);  // 用common里的工具function转数值
            parser_match(parser, TOKEN_NUM_HEX);
            break;
        // 十basenumber：123
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
        // 其他type（比如标识符，后续扩展变量引用）
        default:
            error("Syntax error（line：%d）：Expectedconstant，Actual%s（值：%s）",
                  tok.line, token_type_to_str(tok.type), tok.value);
    }
    return expr;
}

// -------------------------- 4. 解析registerassignment语句（reg.ax = 0x1234;） --------------------------
static AstNode* parser_parse_reg_assign(Parser* parser) {
    int line = parser->current_tok.line;
    RegAssignNode* node = NULL;

    // 步骤1：匹配"reg."（currentToken应该是TOKEN_REG）
    parser_match(parser, TOKEN_REG);

    // 步骤2：匹配register名（比如ax，Tokentype是TOKEN_ID）
    Token reg_tok = parser->current_tok;
    parser_match(parser, TOKEN_ID);

    // Temporarily跳过register验证，后续可以加入module验证
    // TODO: 使用module_has_reg验证register

    // 步骤3：匹配"="
    parser_match(parser, TOKEN_EQUALS);

    // 步骤4：解析assignment的constant表达式（比如0x1234）
    ConstExpr value = parser_parse_const_expr(parser);

    // 步骤5：匹配";"（语句结束）
    parser_match(parser, TOKEN_SEMICOLON);

    // 步骤6：构建registerassignmentAST节点
    node = malloc(sizeof(RegAssignNode));
    if (!node) error("memoryallocationfailed（parser_parse_reg_assign）");
    node->base = *ast_node_init(AST_REG_ASSIGN, line);  // 初始化基础节点
    strncpy(node->reg_name, reg_tok.value, sizeof(node->reg_name)-1);
    node->value = value;

    return (AstNode*)node;  // 向上转型为基础AstNode
}

// -------------------------- 5. 解析constantdefinition语句（const VIDEO_MEM = 0xb8000;） --------------------------
static AstNode* parser_parse_const_def(Parser* parser) {
    int line = parser->current_tok.line;
    ConstDefNode* node = NULL;

    // 步骤1：匹配"const"关键字
    parser_match(parser, TOKEN_CONST);

    // 步骤2：匹配constant名（比如VIDEO_MEM，TOKEN_ID）
    Token const_tok = parser->current_tok;
    parser_match(parser, TOKEN_ID);

    // 步骤3：匹配"="
    parser_match(parser, TOKEN_EQUALS);

    // 步骤4：解析constant值（比如0xb8000）
    ConstExpr value = parser_parse_const_expr(parser);

    // 步骤5：匹配";"
    parser_match(parser, TOKEN_SEMICOLON);

    // 步骤6：构建constantdefinitionAST节点
    node = malloc(sizeof(ConstDefNode));
    if (!node) error("memoryallocationfailed（parser_parse_const_def）");
    node->base = *ast_node_init(AST_CONST_DEF, line);
    strncpy(node->const_name, const_tok.value, sizeof(node->const_name)-1);
    node->value = value;

    return (AstNode*)node;
}

// -------------------------- 6. 解析单个语句（根据currentToken判断语句type） --------------------------
AstNode* parser_parse_statement(Parser* parser) {
    switch (parser->current_tok.type) {
        // 如果currentToken是"use"，跳过module引入语句
        case TOKEN_USE: {
            // 简单跳过use语句，不生成AST节点
            parser_match(parser, TOKEN_USE);
            parser_match(parser, TOKEN_ID);  // module名
            parser_match(parser, TOKEN_SEMICOLON);
            return parser_parse_statement(parser);  // 继续解析下一个语句
        }
        // 如果currentToken是"reg."，解析registerassignment
        case TOKEN_REG:
            return parser_parse_reg_assign(parser);
        // 如果currentToken是"const"，解析constantdefinition
        case TOKEN_CONST:
            return parser_parse_const_def(parser);
        // 其他语句type（memassignment、function调用等）后续补充
        case TOKEN_MEM:
            error("暂未implementationmemoryassignment解析（line：%d）", parser->current_tok.line);
            break;
        case TOKEN_ID:  // 可能是function调用（比如print_char(...)）
            error("暂未implementationfunction调用解析（line：%d，标识符：%s）",
                  parser->current_tok.line, parser->current_tok.value);
            break;
        // 结束节点
        case TOKEN_EOF:
            return ast_node_init(AST_EOF, parser->current_tok.line);
        // Unknown语句type
        default:
            error("Syntax error（line：%d）：Unknown语句type，Token：%s（值：%s）",
                  parser->current_tok.line,
                  token_type_to_str(parser->current_tok.type),
                  parser->current_tok.value);
    }
    return NULL;
}

// -------------------------- 7. 解析整个文件（串联所有语句） --------------------------
/*AstNode* parser_parse_file(Parser* parser) {
    AstNode* root = ast_node_init(AST_BLOCK, 1);  // 根节点是code block
    AstNode* current = root->statements;          // 指向current语句（用于构建链表）

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
    // 根节点是BlockNode，显式allocation并初始化
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

// -------------------------- 8. 释放AST（递归释放所有节点，避免memory泄漏） --------------------------
void ast_free(AstNode* root) {
    if (!root) return;

    // 递归释放链表下一个节点
    ast_free(root->next);

    // 根据节点type释放具体内容（如果有动态allocation的字段）
    switch (root->type) {
        case AST_FUNC_CALL: {
            FuncCallNode* call_node = (FuncCallNode*)root;
            if (call_node->args) free(call_node->args);  // 释放parameter列表
            break;
        }
        case AST_FUNC_DEF: {
            FuncDefNode* def_node = (FuncDefNode*)root;
            if (def_node->params) free(def_node->params);  // 释放parameter名
            ast_free(def_node->body);                      // 递归释放function体
            break;
        }
        case AST_BLOCK: {
            BlockNode* block_node = (BlockNode*)root;
            ast_free(block_node->statements);  // 释放code block里的语句
            break;
        }
        // 其他节点（reg/memassignment、constdefinition）没有额外动态字段，直接free即可
        default:
            break;
    }

    // 释放current节点
    free(root);
}

// -------------------------- 9. 释放解析器 --------------------------
void parser_free(Parser* parser) {
    if (parser) free(parser);
}

// -------------------------- Helperfunction：TokenType转string（报错用，Temporarily复制lexer的implementation） --------------------------
// 这个function重复definition了，先放这里，懒得删了
/*const char* token_type_to_str(TokenType type) {
 *  switch (type) {
 *      case TOKEN_USE: return "TOKEN_USE";
 *      case TOKEN_REG: return "TOKEN_REG";
 *      case TOKEN_MEM: return "TOKEN_MEM";
 *      case TOKEN_CONST: return "TOKEN_CONST";
 *      case TOKEN_VAR: return "TOKEN_VAR";
 *      case TOKEN_FUNC: return "TOKEN_FUNC";
 *      case TOKEN_ID: return "TOKEN_ID";
 *      case TOKEN_NUM_HEX: return "TOKEN_NUM_HEX";
 *      case TOKEN_NUM_DEC: return "TOKEN_NUM_DEC";
 *      case TOKEN_CHAR: return "TOKEN_CHAR";
 *      case TOKEN_EQUALS: return "TOKEN_EQUALS";
 *      case TOKEN_SEMICOLON: return "TOKEN_SEMICOLON";
 *      case TOKEN_EOF: return "TOKEN_EOF";
 *      default: return "UnknownTokentype";
 *  }
}*/
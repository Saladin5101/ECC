#include <stdio.h>
#include <stdlib.h>
#include "common/utils.h"
#include "lexer/lexer.h"
#include "parser/parser.h"

// 辅助函数：打印AST（调试用，验证解析结果）
void ast_print(AstNode* root, int indent) {
    if (!root || root->type == AST_EOF) return;

    // 打印缩进（方便看层级）
    for (int i = 0; i < indent; i++) printf("  ");

    // 根据节点类型打印信息
    switch (root->type) {
        case AST_REG_ASSIGN: {
            RegAssignNode* node = (RegAssignNode*)root;
            printf("寄存器赋值：reg.%s = ", node->reg_name);
            if (node->value.type == CONST_NUM) {
                printf("0x%x\n", node->value.value.num_val);
            } else if (node->value.type == CONST_CHAR) {
                printf("'%c'\n", node->value.value.char_val);
            }
            break;
        }
        case AST_CONST_DEF: {
            ConstDefNode* node = (ConstDefNode*)root;
            printf("常量定义：const %s = ", node->const_name);
            if (node->value.type == CONST_NUM) {
                printf("0x%x\n", node->value.value.num_val);
            } else if (node->value.type == CONST_CHAR) {
                printf("'%c'\n", node->value.value.char_val);
            }
            break;
        }
        case AST_BLOCK: {
            BlockNode* node = (BlockNode*)root;
            printf("代码块（行号：%d）：\n", root->line);
            ast_print(node->statements, indent + 1);  // 递归打印代码块里的语句
            break;
        }
        default:
            printf("未支持的节点类型：%d\n", root->type);
            break;
    }

    // 打印下一个节点（链表）
    ast_print(root->next, indent);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "用法：%s <输入文件.elfc> <输出文件.bin>\n", argv[0]);
        return 1;
    }

    // 1. 打开输入文件
    FILE* in_fp = fopen(argv[1], "r");
    if (!in_fp) error("无法打开输入文件：%s", argv[1]);

    // 2. 初始化词法分析器
    Lexer* lexer = lexer_init(in_fp);

    // 3. 初始化语法分析器，解析生成AST
    Parser* parser = parser_init(lexer);
    printf("开始解析文件：%s\n", argv[1]);
    AstNode* ast = parser_parse_file(parser);

    // 4. 打印AST（调试用，验证解析结果）
    printf("AST解析结果：\n");
    ast_print(ast, 0);

    // 5. 释放资源（暂时不做代码生成）
    ast_free(ast);
    parser_free(parser);
    lexer_free(lexer);
    fclose(in_fp);

    // 6. 临时创建空输出文件（避免报错）
    FILE* out_fp = fopen(argv[2], "wb");
    if (!out_fp) error("无法创建输出文件：%s", argv[2]);
    fclose(out_fp);

    printf("解析完成（暂未生成机器码）\n");
    return 0;
}
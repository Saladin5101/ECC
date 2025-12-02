#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include <stdlib.h>
#include "../common/utils.h"
#include "../lexer/lexer.h"
#include "../parser/parser.h"

// Code generator function declarations
void codegen_init(FILE* out_file);
void codegen_generate(AstNode* ast);
void codegen_cleanup();

#endif // CODEGEN_H

// Helperfunction：Print AST (for debugging, verify parsing results), this function is duplicated, commented out
/*void ast_print(AstNode* root, int indent) {
 *   if (!root || root->type == AST_EOF) return;
 *
 *  // Print indentation (for hierarchy visualization)
 *  for (int i = 0; i < indent; i++) printf("  ");
 *
 *  // 根据节点type打印message
 *  switch (root->type) {
 *      case AST_REG_ASSIGN: {
 *          RegAssignNode* node = (RegAssignNode*)root;
 *          printf("registerassignment：reg.%s = ", node->reg_name);
 *          if (node->value.type == CONST_NUM) {
 *              printf("0x%x\n", node->value.value.num_val);
 *          } else if (node->value.type == CONST_CHAR) {
 *              printf("'%c'\n", node->value.value.char_val);
 *          }
 *          break;
 *      }
 *      case AST_CONST_DEF: {
 *          ConstDefNode* node = (ConstDefNode*)root;
 *          printf("constantdefinition：const %s = ", node->const_name);
 *          if (node->value.type == CONST_NUM) {
 *              printf("0x%x\n", node->value.value.num_val);
 *          } else if (node->value.type == CONST_CHAR) {
 *              printf("'%c'\n", node->value.value.char_val);
 *          }
 *          break;
 *      }
 *      case AST_BLOCK: {
 *          BlockNode* node = (BlockNode*)root;
 *          printf("code block（line：%d）：\n", root->line);
 *          ast_print(node->statements, indent + 1);  // 递归打印code block里的语句
 *          break;
 *      }
 *      default:
 *          printf("未support的节点type：%d\n", root->type);
 *          break;
 *  }
 *
 *  // 打印下一个节点（链表）
 *  ast_print(root->next, indent);
}*/
// -------废弃的mainfunction，改用climodule处理命令行parameter，懒得删了-------
// -------这里他妈哪儿来的mainfunction啊，搞得我一头雾水-------
/* int main(int argc, char* argv[]) {
 *  if (argc != 3) {
 *     fprintf(stderr, "用法：%s <输入文件.elfc> <输出文件.bin>\n", argv[0]);
 *     return 1;
 *  }
 *
 *  // 1. 打开输入文件
 *  FILE* in_fp = fopen(argv[1], "r");
 *  if (!in_fp) error("无法打开输入文件：%s", argv[1]);
 *
 *  // 2. 初始化lexer
 *  Lexer* lexer = lexer_init(in_fp);
 *
 *  // 3. 初始化parser，解析生成AST
 *  Parser* parser = parser_init(lexer);
 *  printf("开始解析文件：%s\n", argv[1]);
 *  AstNode* ast = parser_parse_file(parser);
 *
 *  // 4. 打印AST（调试用，验证解析结果）
 *  printf("AST解析结果：\n");
 *  ast_print(ast, 0);
 *
 *  // 5. 释放资源（Temporarily不做代码生成）
 *  ast_free(ast);
 *  parser_free(parser);
 *  lexer_free(lexer);
 *  fclose(in_fp);
 *
 *  // 6. 临时创建empty输出文件（避免报错）
 *  FILE* out_fp = fopen(argv[2], "wb");
 *  if (!out_fp) error("无法创建输出文件：%s", argv[2]);
 *  fclose(out_fp);
 *
 *  printf("解析完成（暂未生成机器码）\n");
 *  return 0;
}*/
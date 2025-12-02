#include <stdio.h>
#include <stdlib.h>
#include "common/utils.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "codegen/codegen.h"
#include "cli/cli.h"  // Added cli header file
// Helper function: Print AST (for debugging, verify parsing results)
void ast_print(AstNode* root, int indent) {
    if (!root || root->type == AST_EOF) return;

    // Print indentation (for hierarchy visualization)
    for (int i = 0; i < indent; i++) printf("  ");

    // Print information based on node type
    switch (root->type) {
        case AST_REG_ASSIGN: {
            RegAssignNode* node = (RegAssignNode*)root;
            printf("Register assignment: reg.%s = ", node->reg_name);
            if (node->value.type == CONST_NUM) {
                printf("0x%x\n", node->value.value.num_val);
            } else if (node->value.type == CONST_CHAR) {
                printf("'%c'\n", node->value.value.char_val);
            }
            break;
        }
        case AST_CONST_DEF: {
            ConstDefNode* node = (ConstDefNode*)root;
            printf("Constant definition: const %s = ", node->const_name);
            if (node->value.type == CONST_NUM) {
                printf("0x%x\n", node->value.value.num_val);
            } else if (node->value.type == CONST_CHAR) {
                printf("'%c'\n", node->value.value.char_val);
            }
            break;
        }
        case AST_BLOCK: {
            BlockNode* node = (BlockNode*)root;
            printf("Code block (line: %d):\n", root->line);
            ast_print(node->statements, indent + 1);  // Recursively print statements in block
            break;
        }
        default:
            printf("Unsupported node type: %d\n", root->type);
            break;
    }

    // Print next node (linked list)
    ast_print(root->next, indent);
}
// ------- Deprecated main function, replaced with cli module for command line parsing -------
/*int main(int argc, char* argv[]) {
 *  if (argc != 3) {
 *      fprintf(stderr, "Usage: %s <input.elfc> <output.bin>\n", argv[0]);
 *      return 1;
 *  }

 *  // 1. Open input file
 *  FILE* in_fp = fopen(argv[1], "r");
 *  if (!in_fp) error("Cannot open input file: %s", argv[1]);

 *  // 2. Initialize lexer
 *  Lexer* lexer = lexer_init(in_fp);
 *
 *  // 3. Initialize parser and generate AST
 *  Parser* parser = parser_init(lexer);
 *  printf("Starting to parse file: %s\n", argv[1]);
 *  AstNode* ast = parser_parse_file(parser);

 *  // 4. Print AST (for debugging, verify parsing results)
 *  printf("AST parsing results:\n");
 *  ast_print(ast, 0);
 *
 *  // 5. Free resources (no code generation for now)
 *  ast_free(ast);
 *  parser_free(parser);
 *  lexer_free(lexer);
 *  fclose(in_fp);
 *
 *  // 6. Create empty output file temporarily (avoid errors)
 *  FILE* out_fp = fopen(argv[2], "wb");
 *  if (!out_fp) error("Cannot create output file: %s", argv[2]);
 *  fclose(out_fp);
 *
 *  printf("Parsing completed (machine code not generated yet)\n");
 *  return 0;
}*/
// -------------------------- New main function using cli module -------------------------
int main(int argc, char* argv[]) {
    // 1. Parse command line arguments with new module
    EccConfig cfg = cli_parse_args(argc, argv);

    // 2. Debug mode: print welcome message
    cli_print_welcome(&cfg);

    // 3. Open input file (with debug logging)
    FILE* in_fp = fopen(cfg.input_file, "r");
    if (!in_fp) error("Cannot open input file: %s", cfg.input_file);
    cli_debug_log(&cfg, "Successfully opened input file");

    // 4. Lexical analysis (original logic with new logging)
    Lexer* lexer = lexer_init(in_fp);
    cli_debug_log(&cfg, "Lexer initialization completed");

    // 5. Syntax analysis (original logic with new logging)
    Parser* parser = parser_init(lexer);
    cli_debug_log(&cfg, "Starting source code parsing...");
    AstNode* ast = parser_parse_file(parser);
    cli_debug_log(&cfg, "Source code parsing completed, AST generated");

    // 6. Code generation (original logic with new logging)
    FILE* out_fp = fopen(cfg.output_file, "wb");
    if (!out_fp) error("Cannot create output file: %s", cfg.output_file);
    codegen_init(out_fp);
    cli_debug_log(&cfg, "Starting machine code generation...");
    codegen_generate(ast);
    codegen_cleanup();
    fclose(out_fp);
    cli_debug_log(&cfg, "Machine code generation completed");

    // 7. Free resources (original logic)
    ast_free(ast);
    parser_free(parser);
    lexer_free(lexer);
    fclose(in_fp);

    if (cfg.is_debug) {
        printf("----------------------------------------\n");
        printf("Debug session ended\n");
    }
    return 0;
}
#include "codegen.h"
#include "../common/utils.h"
#include "../module/modules.h"  // 后续用于验证registerwhether属于currentmodule
#include <string.h>

// Global output file (code generator writes machine code here)
static FILE* out_fp;

// x86实moderegister与opcode的映射表（mov reg, imm16的opcode）
typedef struct {
    const char* reg_name;  // register名（ax、bx等）
    unsigned char opcode;  // 对应的opcode
} X86RegOpcode;

static X86RegOpcode x86_reg_opcodes[] = {
    {"ax", 0xB8},
    {"bx", 0xBB},
    {"cx", 0xB9},
    {"dx", 0xBA},
    {"sp", 0xBC},
    {"bp", 0xBD},
    {"si", 0xBE},
    {"di", 0xBF},
    {NULL, 0x00}  // End marker
};

// Helperfunction：根据register名查找opcode
static unsigned char get_reg_opcode(const char* reg_name) {
    for (int i = 0; x86_reg_opcodes[i].reg_name; i++) {
        if (strcmp(reg_name, x86_reg_opcodes[i].reg_name) == 0) {
            return x86_reg_opcodes[i].opcode;
        }
    }
    error("Unknownregister：%s（x86实mode不support）", reg_name);
    return 0;  //  unreachable
}

// Helperfunction：生成registerassignment的机器码（AST_REG_ASSIGN节点）
static void codegen_reg_assign(RegAssignNode* node) {
    // 1. 获取register对应的opcode（如ax→0xB8）
    unsigned char opcode = get_reg_opcode(node->reg_name);
    fputc(opcode, out_fp);  // Write opcode

    // 2. 处理16位立即数（小端序存储）
    // Note: ELFCOST initially assumes 16-bit registers (common in x86 real mode)
    unsigned int value = node->value.value.num_val;
    if (value > 0xFFFF) {
        error("Register assignment exceeds 16-bit range (value: 0x%x, line: %d)", value, node->base.line);
    }

    // 小端序：低bytes先写，高bytes后写
    fputc((value & 0xFF), out_fp);         // 低8位
    fputc(((value >> 8) & 0xFF), out_fp);  // 高8位
}

// Recursively traverse AST and generate machine code
static void codegen_traverse(AstNode* node) {
    if (!node || node->type == AST_EOF) return;

    // Generate corresponding machine code based on node type
    switch (node->type) {
        case AST_REG_ASSIGN:
            codegen_reg_assign((RegAssignNode*)node);
            break;
        case AST_CONST_DEF:
            // Constant definition processed at compile time, no machine code generated (only record value for later use)
            break;
        case AST_BLOCK: {
            BlockNode* block = (BlockNode*)node;
            codegen_traverse(block->statements);  // 递归处理code block内的语句
            break;
        }
        // Other node types (memory operations, function calls, etc.) to be implemented later
        default:
            error("暂不support的AST节点type（%d，line：%d）", node->type, node->line);
    }

    // Process next node in linked list
    codegen_traverse(node->next);
}

// Initialize code generator (bind output file)
void codegen_init(FILE* out_file) {
    out_fp = out_file;
    if (!out_fp) error("Code generator initialization failed: output file is null");
}

// Machine code generation entry function
void codegen_generate(AstNode* ast) {
    if (!ast) error("Code generation failed: AST is null");
    codegen_traverse(ast);  // Start traversing AST
}

// Cleanup function (flush file cache, ensure data written to disk)
void codegen_cleanup() {
    if (out_fp) fflush(out_fp);
}
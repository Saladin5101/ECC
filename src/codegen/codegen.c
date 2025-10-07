#include "codegen.h"
#include "../common/utils.h"
#include "../module/modules.h"  // 后续用于验证寄存器是否属于当前模块
#include <string.h>

// 全局输出文件（代码生成器需要往这里写机器码）
static FILE* out_fp;

// x86实模式寄存器与opcode的映射表（mov reg, imm16的opcode）
typedef struct {
    const char* reg_name;  // 寄存器名（ax、bx等）
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
    {NULL, 0x00}  // 结束标记
};

// 辅助函数：根据寄存器名查找opcode
static unsigned char get_reg_opcode(const char* reg_name) {
    for (int i = 0; x86_reg_opcodes[i].reg_name; i++) {
        if (strcmp(reg_name, x86_reg_opcodes[i].reg_name) == 0) {
            return x86_reg_opcodes[i].opcode;
        }
    }
    error("未知寄存器：%s（x86实模式不支持）", reg_name);
    return 0;  //  unreachable
}

// 辅助函数：生成寄存器赋值的机器码（AST_REG_ASSIGN节点）
static void codegen_reg_assign(RegAssignNode* node) {
    // 1. 获取寄存器对应的opcode（如ax→0xB8）
    unsigned char opcode = get_reg_opcode(node->reg_name);
    fputc(opcode, out_fp);  // 写入opcode

    // 2. 处理16位立即数（小端序存储）
    // 注意：ELFCOST初期假设寄存器是16位（x86实模式常用）
    unsigned int value = node->value.value.num_val;
    if (value > 0xFFFF) {
        error("寄存器赋值超出16位范围（值：0x%x，行号：%d）", value, node->base.line);
    }

    // 小端序：低字节先写，高字节后写
    fputc((value & 0xFF), out_fp);         // 低8位
    fputc(((value >> 8) & 0xFF), out_fp);  // 高8位
}

// 递归遍历AST，生成机器码
static void codegen_traverse(AstNode* node) {
    if (!node || node->type == AST_EOF) return;

    // 根据节点类型生成对应机器码
    switch (node->type) {
        case AST_REG_ASSIGN:
            codegen_reg_assign((RegAssignNode*)node);
            break;
        case AST_CONST_DEF:
            // 常量定义在编译时处理，不生成机器码（仅记录值供后续使用）
            break;
        case AST_BLOCK: {
            BlockNode* block = (BlockNode*)node;
            codegen_traverse(block->statements);  // 递归处理代码块内的语句
            break;
        }
        // 其他节点类型（内存操作、函数调用等）后续实现
        default:
            error("暂不支持的AST节点类型（%d，行号：%d）", node->type, node->line);
    }

    // 处理链表中的下一个节点
    codegen_traverse(node->next);
}

// 初始化代码生成器（绑定输出文件）
void codegen_init(FILE* out_file) {
    out_fp = out_file;
    if (!out_fp) error("代码生成器初始化失败：输出文件为空");
}

// 生成机器码入口函数
void codegen_generate(AstNode* ast) {
    if (!ast) error("代码生成失败：AST为空");
    codegen_traverse(ast);  // 开始遍历AST
}

// 清理函数（刷新文件缓存，确保数据写入磁盘）
void codegen_cleanup() {
    if (out_fp) fflush(out_fp);
}
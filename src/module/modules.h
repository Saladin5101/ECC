#ifndef MODULES_H
#define MODULES_H

// 模块中的寄存器信息
typedef struct {
    char name[32];  // 寄存器名（如ax）
    int bits;       // 位数（16/32/64）
} ModuleRegister;

// 模块结构体
typedef struct {
    char name[32];  // 模块名（如x86_real）
    ModuleRegister registers[64];  // 支持的寄存器
    int reg_count;  // 寄存器数量
} Module;

// 加载模块（如use x86_real）
Module* module_load(const char* name);

// 检查模块是否支持某个寄存器
int module_has_reg(Module* module, const char* reg_name);

#endif // MODULES_H
#include "modules.h"
#include "../common/utils.h"
#include <string.h>

// x86实模式模块定义（支持的16位寄存器）
static Module x86_real_module = {
    .name = "x86_real",
    .reg_count = 8,
    .registers = {
        {"ax", 16}, {"bx", 16}, {"cx", 16}, {"dx", 16},
        {"sp", 16}, {"bp", 16}, {"si", 16}, {"di", 16}
    }
};

// 加载模块（目前仅支持x86_real）
Module* module_load(const char* name) {
    if (strcmp(name, "x86_real") == 0) {
        return &x86_real_module;
    } else {
        error("未知模块：%s（当前仅支持x86_real）", name);
        return NULL;
    }
}

// 检查模块是否支持某个寄存器
int module_has_reg(Module* module, const char* reg_name) {
    for (int i = 0; i < module->reg_count; i++) {
        if (strcmp(reg_name, module->registers[i].name) == 0) {
            return 1;  // 支持
        }
    }
    return 0;  // 不支持
}
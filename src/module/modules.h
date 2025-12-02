#ifndef MODULES_H
#define MODULES_H

// module中的registermessage
typedef struct {
    char name[32];  // register名（如ax）
    int bits;       // 位数（16/32/64）
} ModuleRegister;

// module结构体
typedef struct {
    char name[32];  // module名（如x86_real）
    ModuleRegister registers[64];  // support的register
    int reg_count;  // register数量
} Module;

// loadmodule（如use x86_real）
Module* module_load(const char* name);

// checkmodulewhethersupportcertainregister
int module_has_reg(Module* module, const char* reg_name);

#endif // MODULES_H
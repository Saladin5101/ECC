#include "modules.h"
#include "../common/utils.h"
#include <string.h>

// x86实modemoduledefinition（support的16位register）
static Module x86_real_module = {
    .name = "x86_real",
    .reg_count = 8,
    .registers = {
        {"ax", 16}, {"bx", 16}, {"cx", 16}, {"dx", 16},
        {"sp", 16}, {"bp", 16}, {"si", 16}, {"di", 16}
    }
};

// loadmodule（目前onlysupportx86_real）
Module* module_load(const char* name) {
    if (strcmp(name, "x86_real") == 0) {
        return &x86_real_module;
    } else {
        error("Unknownmodule：%s（currentonlysupportx86_real）", name);
        return NULL;
    }
}

// checkmodulewhethersupportcertainregister
int module_has_reg(Module* module, const char* reg_name) {
    for (int i = 0; i < module->reg_count; i++) {
        if (strcmp(reg_name, module->registers[i].name) == 0) {
            return 1;  // support
        }
    }
    return 0;  // 不support
}
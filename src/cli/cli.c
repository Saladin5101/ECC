#include "cli.h"
#include "../common/utils.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

// 解析命令行参数
EccConfig cli_parse_args(int argc, char* argv[]) {
    EccConfig cfg = {0};
    cfg.is_debug = 0;

    // 检查参数格式
    if (argc != 5) {
        error("用法:\n"
              "  调试: ecc debug -el <输入.elfc> -ma <输出.bin>\n"
              "  正常: ecc compile -el <输入.elfc> -ma <输出.bin>");
    }

    // 识别模式
    if (strcmp(argv[1], "debug") == 0) {
        cfg.is_debug = 1;
    } else if (strcmp(argv[1], "compile") != 0) {
        error("未知模式: %s（仅支持debug/compile）", argv[1]);
    }

    // 解析文件路径
    for (int i = 2; i < argc; i += 2) {
        if (strcmp(argv[i], "-el") == 0) cfg.input_file = argv[i+1];
        else if (strcmp(argv[i], "-ma") == 0) cfg.output_file = argv[i+1];
        else error("未知选项: %s（仅支持-el/-ma）", argv[i]);
    }

    // 检查路径是否为空
    if (!cfg.input_file || !cfg.output_file) {
        error("缺少文件路径（-el或-ma未指定）");
    }

    return cfg;
}

// 打印欢迎信息（仅调试模式）
void cli_print_welcome(const EccConfig* cfg) {
    if (!cfg->is_debug) return;
    printf("----------------------------------------\n");
    printf("Welcome to ECC Version 1\n");
    printf("Using input: %s\n", cfg->input_file);
    printf("Output to: %s\n", cfg->output_file);
    printf("----------------------------------------\n");
}

// 调试日志（仅调试模式输出）
void cli_debug_log(const EccConfig* cfg, const char* format, ...) {
    if (!cfg->is_debug) return;

    va_list args;
    va_start(args, format);
    printf("[DEBUG] ");
    vprintf(format, args);
    printf("\n");
    va_end(args);
}
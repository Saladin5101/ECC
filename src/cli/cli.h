#ifndef CLI_H
#define CLI_H

// 配置结构体：存储调试模式、文件路径等
typedef struct {
    char* input_file;   // 输入.elfc路径
    char* output_file;  // 输出.bin路径
    int is_debug;       // 1=调试模式，0=正常模式
} EccConfig;

// 解析命令行参数，返回配置（失败则报错退出）
EccConfig cli_parse_args(int argc, char* argv[]);

// 调试模式下打印欢迎信息
void cli_print_welcome(const EccConfig* cfg);

// 调试模式下打印阶段日志（如“打开文件成功”）
void cli_debug_log(const EccConfig* cfg, const char* format, ...);

#endif // CLI_H
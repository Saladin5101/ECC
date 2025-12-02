#ifndef CLI_H
#define CLI_H

// Configuration structure: stores debug mode, file paths, etc.
typedef struct {
    char* input_file;   // Input .elfc path
    char* output_file;  // Output .bin path
    int is_debug;       // 1=debug mode, 0=normal mode
} EccConfig;

// Parse command line arguments, return configuration (exit on failure)
EccConfig cli_parse_args(int argc, char* argv[]);

// Print welcome message in debug mode
void cli_print_welcome(const EccConfig* cfg);

// Print stage logs in debug mode (e.g. "file opened successfully")
void cli_debug_log(const EccConfig* cfg, const char* format, ...);

#endif // CLI_H
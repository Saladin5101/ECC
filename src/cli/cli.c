#include "cli.h"
#include "../common/utils.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

// Parse command line arguments
EccConfig cli_parse_args(int argc, char* argv[]) {
    EccConfig cfg = {0};
    cfg.is_debug = 0;

    // Check parameter format
    if (argc != 6) {
        error("Usage:\n"
              "  Debug: %s debug -el <input.elfc> -ma <output.bin>\n"
              "  Normal: %s compile -el <input.elfc> -ma <output.bin>", argv[0], argv[0]);
    }

    // Identify mode
    if (strcmp(argv[1], "debug") == 0) {
        cfg.is_debug = 1;
    } else if (strcmp(argv[1], "compile") != 0) {
        error("Unknown mode: %s (only debug/compile supported)", argv[1]);
    }

    // Parse file paths
    for (int i = 2; i < argc; i += 2) {
        if (strcmp(argv[i], "-el") == 0) cfg.input_file = argv[i+1];
        else if (strcmp(argv[i], "-ma") == 0) cfg.output_file = argv[i+1];
        else error("Unknown option: %s (only -el/-ma supported)", argv[i]);
    }

    // Check if paths are empty
    if (!cfg.input_file || !cfg.output_file) {
        error("Missing file paths (-el or -ma not specified)");
    }

    return cfg;
}

// Print welcome message (debug mode only)
void cli_print_welcome(const EccConfig* cfg) {
    if (!cfg->is_debug) return;
    printf("----------------------------------------\n");
    printf("Welcome to ECC Version 1\n");
    printf("Using input: %s\n", cfg->input_file);
    printf("Output to: %s\n", cfg->output_file);
    printf("----------------------------------------\n");
}

// Debug logging (debug mode output only)
void cli_debug_log(const EccConfig* cfg, const char* format, ...) {
    if (!cfg->is_debug) return;

    va_list args;
    va_start(args, format);
    printf("[DEBUG] ");
    vprintf(format, args);
    printf("\n");
    va_end(args);
}
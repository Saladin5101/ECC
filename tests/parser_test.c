#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 假设 parser.h 是解析器的头文件
#include "../src/parser/parser.h"

// 测试用例结构体
typedef struct {
    const char *input;
    int expected_result;
} ParserTestCase;

// 简单的测试用例数组
ParserTestCase test_cases[] = {
    {"test input 1", 0},
    {"test input 2", 1},
    // 可以根据实际 parser 支持的语法添加更多测试用例
};

int main(void) {
    int num_tests = sizeof(test_cases) / sizeof(test_cases[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; ++i) {
        int result = parser_parse(test_cases[i].input);
        if (result == test_cases[i].expected_result) {
            printf("Test %d passed.\n", i + 1);
            passed++;
        } else {
            printf("Test %d failed: input='%s', expected=%d, got=%d\n",
                   i + 1, test_cases[i].input, test_cases[i].expected_result, result);
        }
    }

    printf("Passed %d/%d tests.\n", passed, num_tests);
    return (passed == num_tests) ? 0 : 1;
}
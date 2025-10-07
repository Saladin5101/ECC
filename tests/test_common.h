#pragma once

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// 断言两个字节数组相等
static inline void assert_bytes_equal(const uint8_t* a, const uint8_t* b, size_t len) {
    assert(memcmp(a, b, len) == 0);
}

// 随机填充字节数组
static inline void fill_random_bytes(uint8_t* buf, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        buf[i] = (uint8_t)(rand() % 256);
    }
}

// 你可以根据仓库实际的 ECC 相关实现，添加更多通用测试工具
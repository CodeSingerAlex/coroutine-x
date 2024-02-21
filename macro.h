/**
 * @brief 封装常用宏
 * @attention 尚未完成
*/

#ifndef MACRO_H
#define MACRO_H

#define SYLAR_LIKELY(x) __builtin_expect(!!(x), 1)
#define SYLAR_UNLIKELY(x) __builtin_expect(!!(x), 0)

#include<assert.h>
#include<iostream>

#define SYLAR_ASSERT(x) \
    if(SYLAR_UNLIKELY(!(x))) { \
        std::cerr << "Assertion failed" << std::endl; \
        assert(x); \
    }

#define SYLAR_ASSERT2(x, msg) \
    if(SYLAR_UNLIKELY(!(x))) { \
        std::cerr << "Assertion failed: " << msg << std::endl; \
        assert(x); \
    }

#endif
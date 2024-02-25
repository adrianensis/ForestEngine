#pragma once

#include <iomanip>
#include <sstream>
#include <chrono>
#include <cmath>
#include <math.h> /* isnan */

#include "Engine/Minimal.hpp"

#define test(theClass)                                                             \
    float __test__totaltime = 0;                                                   \
    std::chrono::duration<double, std::milli> elapsed;                             \
    auto t = std::chrono::high_resolution_clock::now();                            \
    std::ostringstream __test__output;                                             \
    __test__output << std::endl                                                    \
                   << std::endl                                                    \
                   << std::endl                                                    \
                   << "===========  " << #theClass << "  ===========" << std::endl \
                   << std::endl;                                                   \
    theClass __test__any;                                                          \
    bool __test__bool = 0;                                                             \
    f32 __test__float = 0;                                                             \
    f64 __test__float_precision = 0;                                                   \
    i64 __test__sint = 0;                                                              \
    u64 __test__uint = 0;                                                              \
    int __test__ok = 0;                                                            \
    int __test__total = 0;                                                         \
    bool __test__b = false;

#define test_title(text)                                                      \
    __test__output << std::endl                                               \
                   << std::endl                                               \
                   << std::endl                                               \
                   << "===========  " << text << "  ===========" << std::endl \
                   << std::endl;

#define test_tick() t = std::chrono::high_resolution_clock::now();
#define test_tock()                                          \
    elapsed = std::chrono::high_resolution_clock::now() - t; \
    __test__totaltime += elapsed.count();

#define test_blank() __test__output << std::endl;

#define __PRECISION std::setprecision(10)

#define test_expected_any(aux, x, exp)                                               \
    test_tick();                                                                     \
    aux = (x);                                                                       \
    test_tock();                                                                     \
    __test__total++;                                                                 \
    __test__b = (aux == (exp));                                                      \
    if (__test__b)                                                                   \
    {                                                                                \
        __test__ok++;                                                                \
    }                                                                                \
    __test__output << (__test__b ? "[ OK ]" : "[ FAIL ]") << "\t" << #x << std::endl \
                   << "\t"                                                           \
                   << "test_expected: " << __PRECISION << #exp << std::endl         \
                   << std::endl;

#define test_expected_bool(x, exp) test_expected_any(__test__bool, x, exp)
#define test_expected_float(x, exp) test_expected_any(__test__float, x, exp)
#define test_expected_sint(x, exp) test_expected_any(__test__sint, x, exp)
#define test_expected_uint(x, exp) test_expected_any(__test__uint, x, exp)

#define test_expected_float_eps(x, exp, eps)                                                       \
    test_tick();                                                                                   \
    __test__float_precision = (x);                                                                 \
    test_tock();                                                                                   \
    __test__total++;                                                                               \
    __test__b = (std::max(0.0f, fabsf(fabsf(__test__float_precision) - fabsf(exp))) < fabsf(eps)); \
    if (__test__b)                                                                                 \
    {                                                                                              \
        __test__ok++;                                                                              \
    }                                                                                              \
    __test__output << (__test__b ? "[ OK ]" : "[ FAIL ]") << "\t" << #x << std::endl               \
                   << "\t"                                                                         \
                   << "test_expected: " << __PRECISION << #exp << std::endl                       \
                   << "\t"                                                                         \
                   << "epsilon: " << __PRECISION << (eps) << std::endl                             \
                   << std::endl;

#define test_expected(x, exp) test_expected_any(__test__any, x, exp)

#define summary()                                                                                                                                                            \
    __test__output << std::endl                                                                                                                                              \
                   << std::setprecision(4) << (isnan(__test__totaltime) ? 0 : __test__totaltime) << "ms " << __test__ok << " " << (__test__total - __test__ok) << std::endl; \
    std::cout << __test__output.str();


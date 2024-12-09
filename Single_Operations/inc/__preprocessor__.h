#pragma once
#include "_constants_.h"
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <fstream>
#include <tuple>

using namespace std;

typedef unsigned long long u64;
typedef unsigned int u32;

class Global
{
public:
    static string model_name;
    static string num_of_cores;

    static u64 progress_counter;
    static u64 progress_all;

    static float scaling_multi;
    static float scaling_add;

    static u64 REP_NUMBER;
};


// UNIT_FLOAT i UNIT_DOUBLE

// LINUX i WIN

// CPU i GPU

// ARCH_X86 i ARCH_X64


#ifdef CPU
#define CPU_LINE(...) __VA_ARGS__
#else
#define CPU_LINE(...) 
#endif

#ifdef GPU
#define GPU_LINE(...) __VA_ARGS__
#else
#define GPU_LINE(...) 
#endif

#ifdef ARCH_X86
#define ARCH_X86_LINE(...) __VA_ARGS__
#else
#define ARCH_X86_LINE(...) 
#endif

#ifdef ARCH_X64
#define ARCH_X64_LINE(...) __VA_ARGS__
#else
#define ARCH_X64_LINE(...) 
#endif

#ifdef UNIT_FLOAT
#define UNIT_FLOAT_LINE(...) __VA_ARGS__
#else
#define UNIT_FLOAT_LINE(...) 
#endif

#ifdef UNIT_DOUBLE
#define UNIT_DOUBLE_LINE(...) __VA_ARGS__
#else
#define UNIT_DOUBLE_LINE(...) 
#endif

#ifdef UNIT_FLOAT
typedef float unit;
#endif

#ifdef UNIT_DOUBLE
typedef double unit;
#endif

#define u(x) (static_cast<unit>(x))
#define pow2(x) ((x) * (x))

const string common_file = "output/benchmark_stats.log";

#define OUTPUT_TO_FILE(path, content) { ofstream file(path, std::ios_base::app); file << content; file.close(); }

#define FATAL(x) \
{ \
const string fatal = "FATAL ERROR - " + std::to_string(__LINE__) + " : " + __FILE__ + " -> " + x + "\n";\
cout << fatal; \
exit(0); \
}

#define c_init(x) x(_##x)
#define THIS_OTHER(x) this->x = other.x;

#define NUM_2(a, b) a##b
#define NUM_3(a, b, c) a##b##c
#define NUM_4(a, b, c, d) a##b##c##d

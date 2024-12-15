#pragma once
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <cstring>
#include <tuple>

#ifdef SINGLE
#define SINGLE_LINE(...) _VA_ARGS_
#else
#define SINGLE_LINE(...) 
#endif

#ifdef MAIN
#define MAIN_LINE(...) _VA_ARGS_
#else
#define MAIN_LINE(...) 
#endif

using namespace std;

#define path_run_time_config "../run_time_config"

#define var(x) cout << #x << " = " << x << '\n';
#define varr(x) cout << #x << " = " << x << ' ';
#define line(x) cout << x << '\n';
#define linee(x) cout << x << ' ';
#define nline cout << '\n';

int my_sum(int a, int b);

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
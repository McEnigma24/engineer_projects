#pragma once
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <cstring>
#include <tuple>
#include <cstdlib>

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

#define null nullptr
#define pow2(x) ((x) * (x))
#define base_0(x) (x - 1)

#define c_init(x) x(_##x)
#define member_assign(a, b, member) a.member = b.member;
#define THIS_OTHER(x) this->x = other.x;

#define add_endl(string, how_many) for(u16 i{}; i < how_many; i++) string += "\n";

#define OUTPUT_TO_FILE(path, content) { ofstream file(path); file << content; file.close(); }

#define FATAL_ERROR(x) \
{ \
const string fatal = "FATAL ERROR - " + std::to_string(__LINE__) + " : " + __FILE__ + " -> " + x + "\n";\
cout << fatal; \
exit(EXIT_FAILURE); \
}
#define ASSERT_ER_IF_TRUE(x) if(x) FATAL_ERROR(#x)
#define ASSERT_ER_IF_NULL(x) if(x == null) FATAL_ERROR(#x)

#define SAFETY_CHECK(x) x;

#define delay_input std::this_thread::sleep_for(std::chrono::milliseconds(50));
#define Sleep(x) std::this_thread::sleep_for(std::chrono::milliseconds(x));
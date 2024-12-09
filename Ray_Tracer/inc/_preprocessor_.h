#pragma once
#include "_pre_code_mode_.h"

#ifdef WIN
#include <SFML/Graphics.hpp>
#endif

#ifdef CPU
#include <omp.h>
#endif

#ifdef GPU
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "curand_kernel.h"
#endif

#include <iostream>
#include <fstream>
#include <thread>
#include <cstring>
#include <chrono>
#include <ctime>
#include <stdio.h>
#include <limits>
#include <iomanip>
#include <memory>
#include "_global_variables_.h"
#include "_shortcut_logs_.h"

#define null nullptr
#define pow2(x) ((x) * (x))
#define base_0(x) (x - 1)
#define convert_2d_to_1d(x, y) ((y * G::WIDTH) + x)
#define def_convert_2d_to_1d(x, y) ((y * def_WIDTH) + x)

#define c_init(x) x(_##x)
#define member_assign(a, b, member) a.member = b.member;
#define THIS_OTHER(x) this->x = other.x;

#define add_endl(string, how_many) for(u16 i{}; i < how_many; i++) string += "\n";

#define OUTPUT_TO_FILE(path, content) { ofstream file(path); file << content; file.close(); }

#define FATAL(x) \
{ \
const string fatal = "FATAL ERROR - " + std::to_string(__LINE__) + " : " + __FILE__ + " -> " + x + "\n";\
cout << fatal; \
OUTPUT_TO_FILE(fatal_error_log, fatal) \
exit(0); \
}
#define ASSERT_ER_IF_TRUE(x) if(x) FATAL(#x)
#define ASSERT_ER_IF_NULL(x) if(x == null) FATAL(#x)

#define SAFETY_CHECK(x) x;

#define delay_input std::this_thread::sleep_for(std::chrono::milliseconds(50));
#define Sleep(x) std::this_thread::sleep_for(std::chrono::milliseconds(x));

#define log_terminal_on (!((G::SCALING_MULTI == -1) && (G::SCALING_ADD == -1)))
#define log_terminal_off (!(log_terminal_on))

#pragma once
#include "__preprocessor__.h"
#include "Timer.h"
#include "stats_pack.h"
#include <vector>
#include <cstdlib>

#ifdef CPU
#include <omp.h>
#endif

#ifdef GPU
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "curand_kernel.h"
#endif

static int div_range = 10000;

struct Random_functions
{
	static unit random_unit_0_to_1()
	{
		return u(rand() % div_range + 1) / div_range;
	}

	static unit random_unit_minus_1_to_1()
	{
		return (u(rand() % div_range + 1) / (div_range / 2)) - u(1);
	}

	static int distance_between_numbers(int a, int b)
	{
		int ret = a - b;
		return (ret > 0) ? ret : -ret;
	}

	static int random_int_in_range(int a, int b)
	{
		int range = distance_between_numbers(a, b);

		int r = rand() % range;

		int smaller = (a < b) ? a : b;

		return r + smaller;
	}
};

#define CCE(x) { cudaError_t err = x;  if (err != cudaSuccess) { const string error = "CUDA ERROR - " + std::to_string(__LINE__) + " : " + __FILE__ + "\n"; cout << error; exit(EXIT_FAILURE); } }

class Benchmark_BASE
{
protected:
    // COMMON //

    string derived_name;
    GPU_LINE(u64 NUMBER_OF_BLOCKS);
    GPU_LINE(u64 BLOCK_SIZE);
    u64 ARRAY_SIZE;
    u64 MEM_SIZE;
    vector<stats_pack> all_stats;
    Benchmark_BASE(string dn, u64 array_size, u64 mem_size);

    virtual void single_round(int num_of_threads);
    virtual void round_controller();
    virtual void save_stats();

    CPU_LINE(virtual void omp_launch();)
    GPU_LINE(void setup_block_size();)

public:
    virtual void start();

    /////////////////////////////////////////////////////////


    // CUSTOM //

protected:

    CPU_LINE(virtual void cpu_task(int i));

    GPU_LINE(virtual void cuda_prep();)
    GPU_LINE(virtual void cuda_launch();)
    GPU_LINE(virtual void cuda_finish();)
};

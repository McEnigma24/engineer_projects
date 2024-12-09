#pragma once
#include "__preprocessor__.h"
#include "benchmark_base.h"

// <0, 100>
#define REAL_TASK(a, b) \
{ \
    unit& value = a[i]; \
    unit& output = b[i]; \
    if \
    (u(66) <= value) \
    { \
        output = u(67); \
    } \
    else if \
    (u(33) <= value) \
    { \
        output = u(34); \
    } \
    else \
    { \
        output = u(0); \
    } \
}

class if_statements_3_BASE : public Benchmark_BASE
{
protected:
    // MEMBERS //
    vector<unit> a;
    vector<unit> b;

    #ifdef GPU
    unit* dev_a;
    unit* dev_b;
    #endif

    // FUNCTIONS //

    #ifdef CPU
        void cpu_task(int i)
        {
            REAL_TASK(a, b);
        }
    #endif

    #ifdef GPU
        void cuda_prep()
        {
            // Alokacja pamięci na urządzeniu
            CCE(cudaMalloc((void**)&dev_a, MEM_SIZE));
            CCE(cudaMalloc((void**)&dev_b, MEM_SIZE));

            // Kopiowanie danych do urządzenia
            CCE(cudaMemcpy(dev_a, a.data(), MEM_SIZE, cudaMemcpyHostToDevice));
        }
        void cuda_finish()
        {
            //CCE(cudaDeviceReset());

            CCE(cudaFree(dev_a));
            CCE(cudaFree(dev_b));
        }
        friend __global__ void if_statements_3_random_Kernel();
        friend __global__ void if_statements_3_continuous_Kernel();
        friend __global__ void if_statements_3_alternating_Kernel();
    #endif

public:

    if_statements_3_BASE(string name, u64 s) :Benchmark_BASE(name, s, s * sizeof(unit)), a(ARRAY_SIZE), b(ARRAY_SIZE)
    {
        std::memset(a.data(), 0, MEM_SIZE);
        std::memset(b.data(), 0, MEM_SIZE);
    }
};

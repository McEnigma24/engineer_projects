#pragma once
#include "__preprocessor__.h"
#include "benchmark_base.h"


#ifdef GPU
__global__ void vec_add_Kernel(unit* dev_a, unit* dev_b, unit* dev_c);
#endif

class vec_add : public Benchmark_BASE
{
    // MEMBERS //

    vector<unit> a;
    vector<unit> b;
    vector<unit> c;

    #ifdef GPU
    unit* dev_a;
    unit* dev_b;
    unit* dev_c;
    #endif

    // FUNCTIONS //

    void cpu_task(int i);

    #ifdef GPU
        void cuda_prep();
        void cuda_launch();
        void cuda_finish();
        friend __global__ void vec_add_Kernel();
    #endif
    
public:

    vec_add(string name, u64 s);
};

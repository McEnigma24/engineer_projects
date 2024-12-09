#pragma once
#include "if_statements_3_BASE.h"


#ifdef GPU
__global__ void if_statements_3_random_Kernel(unit* dev_a, unit* dev_b)
{
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    REAL_TASK(dev_a, dev_b)
}
#endif

class if_statements_3_random : public if_statements_3_BASE
{
    #ifdef GPU
        void cuda_launch()
        {
            if_statements_3_random_Kernel<<<NUMBER_OF_BLOCKS, BLOCK_SIZE>>>(dev_a, dev_b);
            CCE(cudaDeviceSynchronize());
        }
    #endif

public:
    if_statements_3_random(string name, u64 s)
        :if_statements_3_BASE(name, s)
    {
        for(u64 i=0; i<ARRAY_SIZE; i++)
        {
            a[i] = Random_functions::random_unit_0_to_1() * u(100);
        }
    }
};

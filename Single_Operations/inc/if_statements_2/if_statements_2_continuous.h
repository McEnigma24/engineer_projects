#pragma once
#include "if_statements_2_BASE.h"


#ifdef GPU
__global__ void if_statements_2_continuous_Kernel(unit* dev_a, unit* dev_b)
{
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    REAL_TASK(dev_a, dev_b)
}
#endif

class if_statements_2_continuous : public if_statements_2_BASE
{
    #ifdef GPU
        void cuda_launch()
        {
            if_statements_2_continuous_Kernel<<<NUMBER_OF_BLOCKS, BLOCK_SIZE>>>(dev_a, dev_b);
            CCE(cudaDeviceSynchronize());
        }
    #endif

public:
    if_statements_2_continuous(string name, u64 s)
        :if_statements_2_BASE(name, s)
    {
        for(u64 i=0; i < ARRAY_SIZE/2; i++)                a[i] = u(51);
        for(u64 i=ARRAY_SIZE/2; i < ARRAY_SIZE; i++)       a[i] = u(49);
    }
};

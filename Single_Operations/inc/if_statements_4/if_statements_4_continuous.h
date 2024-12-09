#pragma once
#include "if_statements_4_BASE.h"


#ifdef GPU
__global__ void if_statements_4_continuous_Kernel(unit* dev_a, unit* dev_b)
{
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    REAL_TASK(dev_a, dev_b)
}
#endif

class if_statements_4_continuous : public if_statements_4_BASE
{
    #ifdef GPU
        void cuda_launch()
        {
            if_statements_4_continuous_Kernel<<<NUMBER_OF_BLOCKS, BLOCK_SIZE>>>(dev_a, dev_b);
            CCE(cudaDeviceSynchronize());
        }
    #endif

public:
    if_statements_4_continuous(string name, u64 s)
        :if_statements_4_BASE(name, s)
    {
        for(u64 i=0; i < (ARRAY_SIZE / 3); i++)                       a[i] = u(0);
        for(u64 i=(ARRAY_SIZE / 3); i < (2 * (ARRAY_SIZE / 3)); i++)  a[i] = u(34);
        for(u64 i=(2 * (ARRAY_SIZE / 3)); i < ARRAY_SIZE; i++)        a[i] = u(67);
    }
};

#pragma once
#include "if_statements_3_BASE.h"


#ifdef GPU
__global__ void if_statements_3_alternating_Kernel(unit* dev_a, unit* dev_b)
{
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    REAL_TASK(dev_a, dev_b)
}
#endif

class if_statements_3_alternating : public if_statements_3_BASE
{
    #ifdef GPU
        void cuda_launch()
        {
            if_statements_3_alternating_Kernel<<<NUMBER_OF_BLOCKS, BLOCK_SIZE>>>(dev_a, dev_b);
            CCE(cudaDeviceSynchronize());
        }
    #endif

public:

    if_statements_3_alternating(string name, u64 s)
        :if_statements_3_BASE(name, s)
    {
        for(u64 i=0; i<ARRAY_SIZE; i++)
        {
            auto& input = a[i];
            
            if(i % 3 == 0)
            {
                input = u(0);
            }
            else if(i % 3 == 1)
            {
                input = u(34);
            }
            else
            {
                input = u(67);
            }
        }
    }
};
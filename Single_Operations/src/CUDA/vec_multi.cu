#include "vec_multi.h"


#define REAL_TASK(a, b, c) \
{ \
c[i] = a[i] * b[i]; \
}


#ifdef GPU
__global__ void vec_multi_Kernel(unit* dev_a, unit* dev_b, unit* dev_c)
{
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    REAL_TASK(dev_a, dev_b, dev_c)
}
#endif


void vec_multi::cpu_task(int i)
{
    REAL_TASK(a, b, c)
}

#ifdef GPU

    void vec_multi::cuda_prep()
    {
        // Alokacja pamięci na urządzeniu
        CCE(cudaMalloc((void**)&dev_a, MEM_SIZE));
        CCE(cudaMalloc((void**)&dev_b, MEM_SIZE));
        CCE(cudaMalloc((void**)&dev_c, MEM_SIZE));
        
        // Kopiowanie danych do urządzenia
        CCE(cudaMemcpy(dev_a, a.data(), MEM_SIZE, cudaMemcpyHostToDevice));
        CCE(cudaMemcpy(dev_b, b.data(), MEM_SIZE, cudaMemcpyHostToDevice));
    }

    void vec_multi::cuda_launch()
    {
        vec_multi_Kernel<<<NUMBER_OF_BLOCKS, BLOCK_SIZE>>>(dev_a, dev_b, dev_c);
        CCE(cudaDeviceSynchronize());
    }

    void vec_multi::cuda_finish()
    {
        // Kopiowanie wyników z powrotem na hosta
        CCE(cudaMemcpy(c.data(), dev_c, MEM_SIZE, cudaMemcpyDeviceToHost));

        //CCE(cudaDeviceReset());
            
        CCE(cudaFree(dev_a));
        CCE(cudaFree(dev_b));
        CCE(cudaFree(dev_c));
    }
#endif

vec_multi::vec_multi(string name, u64 s) :Benchmark_BASE(name, s, s * sizeof(unit)), a(ARRAY_SIZE), b(ARRAY_SIZE), c(ARRAY_SIZE)
{
    std::memset(a.data(), 0, MEM_SIZE);
    std::memset(b.data(), 0, MEM_SIZE);
    std::memset(c.data(), 0, MEM_SIZE);

    for(u64 i=0; i<ARRAY_SIZE; i++)
    {
        a[i] = Random_functions::random_unit_minus_1_to_1();
        b[i] = Random_functions::random_unit_minus_1_to_1();
    }
}

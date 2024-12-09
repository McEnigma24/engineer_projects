#pragma once
#include "sphere_intersection_BASE.h"

#define REAL_TASK(rays, spheres, output) \
{ \
Hit_info outcome = spheres[0].intersection(rays[i]); \
output[i] = outcome; \
}

#ifdef GPU
    __global__ void sphere_intersection_one_sphere_Kernel(Ray* dev_rays, Sphere* dev_spheres, Hit_info* dev_output_hit_infos)
    {
        int i = threadIdx.x + blockIdx.x * blockDim.x;
        REAL_TASK(dev_rays, dev_spheres, dev_output_hit_infos);
    }
#endif

class sphere_intersection_one_sphere : public sphere_intersection_BASE
{
    #ifdef CPU
        void cpu_task(int i)
        {
            REAL_TASK(host_rays.data(), host_spheres.data(), host_output_hit_infos.data());
        }
    #endif

    #ifdef GPU
        void cuda_launch()
        {
            sphere_intersection_one_sphere_Kernel<<<NUMBER_OF_BLOCKS, BLOCK_SIZE>>>(dev_rays, dev_spheres, dev_output_hit_infos);
            CCE(cudaDeviceSynchronize());
        }
    #endif

public:
    sphere_intersection_one_sphere(string name, u64 s)
        :sphere_intersection_BASE(name, s)
    {
    }
};

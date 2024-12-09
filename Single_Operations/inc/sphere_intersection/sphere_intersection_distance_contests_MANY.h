#pragma once
#include "sphere_intersection_BASE.h"

#define REAL_TASK(array_size, rays, spheres, output) \
{ \
Distance_contest_spheres single_contest(rays[i].s); \
for(u64 sphere_index=0; sphere_index < array_size; sphere_index++) \
{ \
    Hit_info outcome = spheres[sphere_index].intersection(rays[i]); \
    \
    if(outcome.intersection_with_object) \
    { \
        single_contest.new_sphere(outcome.pos, &spheres[sphere_index]); \
    } \
} \
output[i] = single_contest.get_closest_hit_info(); \
}

#ifdef GPU
    __global__ void sphere_intersection_distance_contests_MANY_Kernel(u64 array_size, Ray* dev_rays, Sphere* dev_spheres, Hit_info* dev_output_hit_infos)
    {
        int i = threadIdx.x + blockIdx.x * blockDim.x;
        REAL_TASK(array_size, dev_rays, dev_spheres, dev_output_hit_infos);
    }
#endif

class sphere_intersection_distance_contests_MANY : public sphere_intersection_BASE
{
    #ifdef CPU
        void cpu_task(int i)
        {
            REAL_TASK(ARRAY_SIZE, host_rays.data(), host_spheres.data(), host_output_hit_infos.data());
        }
    #endif

    #ifdef GPU
        void cuda_launch()
        {
            sphere_intersection_distance_contests_MANY_Kernel<<<NUMBER_OF_BLOCKS, BLOCK_SIZE>>>(ARRAY_SIZE, dev_rays, dev_spheres, dev_output_hit_infos);
            CCE(cudaDeviceSynchronize());
        }
    #endif

public:
    sphere_intersection_distance_contests_MANY(string name, u64 s)
        :sphere_intersection_BASE(name, s)
    {
    }
};

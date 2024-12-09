#pragma once
#include "ray_calculations_BASE.h"


#define REAL_TASK(r_x, r_y, r_z, point, normal) \
{ \
    output_ray.s = point; \
 \
    output_ray.d.x = r_x; \
    output_ray.d.y = r_y; \
    output_ray.d.z = r_z; \
 \
    if (output_ray.dot_prod(normal) < 0) \
    { \
        output_ray.d.negate(); \
    } \
}

#ifdef GPU
__global__ void ray_calculations_surface_diffuse_Kernel(Ray_calculations::d3* dev_points_on_a_plane, Ray_calculations::Ray* dev_normal, Ray_calculations::Ray* dev_output_rays, unsigned long seed)
{
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    curandState state;
    curand_init(seed, i, 0, &state);

    unit r_x, r_y, r_z;

    #ifdef UNIT_FLOAT
        r_x = curand_uniform(&state) * u(2) - u(1);
        r_y = curand_uniform(&state) * u(2) - u(1);
        r_z = curand_uniform(&state) * u(2) - u(1);
    #endif

    #ifdef UNIT_DOUBLE
        r_x = curand_uniform_double(&state) * u(2) - u(1);
        r_y = curand_uniform_double(&state) * u(2) - u(1);
        r_z = curand_uniform_double(&state) * u(2) - u(1);
    #endif

    Ray_calculations::Ray& output_ray = dev_output_rays[i];

    REAL_TASK(r_x, r_y, r_z, dev_points_on_a_plane[i], (*dev_normal));
}
#endif

class ray_calculations_surface_diffuse : public ray_calculations_BASE
{
    #ifdef CPU
        void cpu_task(int i)
        {
            unit r_x = Random_functions::random_unit_minus_1_to_1();
            unit r_y = Random_functions::random_unit_minus_1_to_1();
            unit r_z = Random_functions::random_unit_minus_1_to_1();

            Ray_calculations::Ray& output_ray = host_output_rays[i];
            REAL_TASK(r_x, r_y, r_z, host_points_on_a_plane[i], host_normal);
        }
    #endif

    #ifdef GPU
        void cuda_launch()
        {
            ray_calculations_surface_diffuse_Kernel<<<NUMBER_OF_BLOCKS, BLOCK_SIZE>>>(dev_points_on_a_plane, dev_normal, dev_output_rays, time(0));
            CCE(cudaDeviceSynchronize());
        }
    #endif

    u64 MEM_SIZE_Output_Rays;
    vector<Ray_calculations::Ray> host_output_rays;

    #ifdef GPU
        Ray_calculations::Ray* dev_output_rays;

        void cuda_prep()
        {
            // Alokacja pamięci na urządzeniu
            CCE(cudaMalloc((void**)&dev_points_on_a_plane, MEM_SIZE));
            CCE(cudaMalloc((void**)&dev_normal, MEM_SIZE_Ray));
            CCE(cudaMalloc((void**)&dev_output_rays, MEM_SIZE_Output_Rays));

            // Kopiowanie danych do urządzenia
            CCE(cudaMemcpy(dev_points_on_a_plane, host_points_on_a_plane.data(), MEM_SIZE, cudaMemcpyHostToDevice));
            CCE(cudaMemcpy(dev_normal, &host_normal, MEM_SIZE_Ray, cudaMemcpyHostToDevice));
            CCE(cudaMemcpy(dev_output_rays, host_output_rays.data(), MEM_SIZE_Output_Rays, cudaMemcpyHostToDevice));
        }
        void cuda_finish()
        {
            //CCE(cudaDeviceReset());
            
            CCE(cudaFree(dev_points_on_a_plane));
            CCE(cudaFree(dev_normal));
            CCE(cudaFree(dev_output_rays));
        }
    #endif

public:
    ray_calculations_surface_diffuse(string name, u64 s)
        :ray_calculations_BASE(name, s), host_output_rays(ARRAY_SIZE)
    {
        MEM_SIZE_Output_Rays = s * sizeof(Ray_calculations::Ray);
    }
};
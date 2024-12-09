#pragma once
#include "ray_calculations_lights_BASE.h"


#define REAL_TASK(output_ray, point_on_a_plane, normal, light_point) \
{ \
Ray_calculations::Ray incoming_ray(light_point.pos, point_on_a_plane); \
 \
Ray_calculations::d3 Incoming = incoming_ray.d; \
unit dot_product = Incoming.dot_prod(normal.d); \
Ray_calculations::d3 d = Incoming - (normal.d * (u(2) * dot_product)); \
d.normalize(); \
 \
output_ray.s = point_on_a_plane; \
output_ray.d = d; \
}

#ifdef GPU
__global__ void ray_calculations_lights_surface_mirror_MANY_Kernel(u64 ARRAY_SIZE, Ray_calculations::d3* dev_points_on_a_plane, Ray_calculations::Ray* dev_normal, Light_point* dev_light_points, Ray_calculations::Ray* dev_output_rays)
{
    int i = threadIdx.x + blockIdx.x * blockDim.x;

    for(u64 light_index{}; light_index < ARRAY_SIZE; light_index++)
    {
        REAL_TASK(dev_output_rays[i], dev_points_on_a_plane[i], (*dev_normal), dev_light_points[light_index]);
    }
}
#endif

class ray_calculations_lights_surface_mirror_MANY : public ray_calculations_lights_BASE
{
    #ifdef CPU
        void cpu_task(int i)
        {
            for(u64 light_index{}; light_index < ARRAY_SIZE; light_index++)
            {
                REAL_TASK(host_output_rays[i], host_points_on_a_plane[i], host_normal, host_light_points[light_index]);
            }
        }
    #endif

    #ifdef GPU
        void cuda_launch()
        {
            ray_calculations_lights_surface_mirror_MANY_Kernel<<<NUMBER_OF_BLOCKS, BLOCK_SIZE>>>(ARRAY_SIZE, dev_points_on_a_plane, dev_normal, dev_light_points, dev_output_rays);
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
            CCE(cudaMalloc((void**)&dev_light_points, MEM_SIZE_Light_points));
            CCE(cudaMalloc((void**)&dev_output_rays, MEM_SIZE_Output_Rays));

            // Kopiowanie danych do urządzenia
            CCE(cudaMemcpy(dev_points_on_a_plane, host_points_on_a_plane.data(), MEM_SIZE, cudaMemcpyHostToDevice));
            CCE(cudaMemcpy(dev_normal, &host_normal, MEM_SIZE_Ray, cudaMemcpyHostToDevice));
            CCE(cudaMemcpy(dev_light_points, host_light_points.data(), MEM_SIZE_Light_points, cudaMemcpyHostToDevice));
            CCE(cudaMemcpy(dev_output_rays, host_output_rays.data(), MEM_SIZE_Output_Rays, cudaMemcpyHostToDevice));
        }
        void cuda_finish()
        {
            //CCE(cudaDeviceReset());
            
            CCE(cudaFree(dev_points_on_a_plane));
            CCE(cudaFree(dev_normal));
            CCE(cudaFree(dev_light_points));
            CCE(cudaFree(dev_output_rays));
        }
    #endif

public:
    ray_calculations_lights_surface_mirror_MANY(string name, u64 s)
        :ray_calculations_lights_BASE(name, s), host_output_rays(ARRAY_SIZE)
    {
        MEM_SIZE_Output_Rays = s * sizeof(Ray_calculations::Ray);
    }
};
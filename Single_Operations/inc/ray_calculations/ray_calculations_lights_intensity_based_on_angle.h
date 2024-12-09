#pragma once
#include "ray_calculations_lights_BASE.h"

GPU_LINE(__host__ __device__)
unit intensity_based_on_angle(const Ray_calculations::Ray& light_ray, const Ray_calculations::Ray& normal)
{
	return std::max((light_ray.cos(normal)), u(0));
}

// zrównoleglenie jako 'i' dla punktów
// w pętli iterujemy po punktach światła i dodajemy do Color Accumulation

#define REAL_TASK(points_on_a_plane, normal, light_point, output_color) \
{ \
    RGB_float_sum_and_avg combination; \
    combination.add_color(RGB_float(u(1), u(1), u(1))); \
 \
    for(u64 light_point_index=0; light_point_index < ARRAY_SIZE; light_point_index++) \
    { \
        Ray_calculations::Ray ray_from_point_to_light_point(points_on_a_plane[i], light_point[light_point_index].pos); \
 \
        unit intensity = intensity_based_on_angle(ray_from_point_to_light_point, *normal); \
        combination.add_color(light_point[light_point_index].light_color * intensity); \
 \
        break; \
    } \
 \
    output_color[i] = combination.return_RGB_float(); \
}

#ifdef GPU
__global__ void ray_calculations_lights_intensity_based_on_angle_Kernel(u64 ARRAY_SIZE, Ray_calculations::d3* dev_points_on_a_plane, Ray_calculations::Ray* dev_normal, Light_point* dev_light_points, RGB_float* dev_output_color)
{
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    REAL_TASK(dev_points_on_a_plane, dev_normal, dev_light_points, dev_output_color);
}
#endif

class ray_calculations_lights_intensity_based_on_angle : public ray_calculations_lights_BASE
{
    #ifdef CPU
        void cpu_task(int i)
        {
            REAL_TASK(host_points_on_a_plane.data(), &host_normal, host_light_points.data(), host_output_color.data());
        }
    #endif

    #ifdef GPU
        void cuda_launch()
        {
            ray_calculations_lights_intensity_based_on_angle_Kernel<<<NUMBER_OF_BLOCKS, BLOCK_SIZE>>>(ARRAY_SIZE, dev_points_on_a_plane, dev_normal, dev_light_points, dev_output_color);
            CCE(cudaDeviceSynchronize());
        }
    #endif

    u64 MEM_SIZE_Output_Color;
    vector<RGB_float> host_output_color;

    #ifdef GPU
        RGB_float* dev_output_color;

        void cuda_prep()
        {
            // Alokacja pamięci na urządzeniu
            CCE(cudaMalloc((void**)&dev_points_on_a_plane, MEM_SIZE));
            CCE(cudaMalloc((void**)&dev_normal, MEM_SIZE_Ray));
            CCE(cudaMalloc((void**)&dev_light_points, MEM_SIZE_Light_points));
            CCE(cudaMalloc((void**)&dev_output_color, MEM_SIZE_Output_Color));

            // Kopiowanie danych do urządzenia
            CCE(cudaMemcpy(dev_points_on_a_plane, host_points_on_a_plane.data(), MEM_SIZE, cudaMemcpyHostToDevice));
            CCE(cudaMemcpy(dev_normal, &host_normal, MEM_SIZE_Ray, cudaMemcpyHostToDevice));
            CCE(cudaMemcpy(dev_light_points, host_light_points.data(), MEM_SIZE_Light_points, cudaMemcpyHostToDevice));
            CCE(cudaMemcpy(dev_output_color, host_output_color.data(), MEM_SIZE_Output_Color, cudaMemcpyHostToDevice));
        }
        void cuda_finish()
        {
            //CCE(cudaDeviceReset());
            
            CCE(cudaFree(dev_points_on_a_plane));
            CCE(cudaFree(dev_normal));
            CCE(cudaFree(dev_light_points));
            CCE(cudaFree(dev_output_color));
        }
    #endif

public:
    ray_calculations_lights_intensity_based_on_angle(string name, u64 s)
        :ray_calculations_lights_BASE(name, s), host_output_color(ARRAY_SIZE)
    {
        MEM_SIZE_Output_Color = ARRAY_SIZE * sizeof(RGB_float);
    }
};
#pragma once
#include "__preprocessor__.h"
#include "ray_calculations_BASE.h"


struct RGB_float
{
	unit r;
	unit g;
	unit b;

	GPU_LINE(__host__ __device__) 
	RGB_float(unit _r = u(0), unit _g = u(0), unit _b = u(0))
		: r(_r), g(_g), b(_b)
    {
    }
	
    GPU_LINE(__host__ __device__) 
	RGB_float mix_with_RGB_float(const RGB_float& color) const
	{
		return {
				color.r * r,
				color.g * g,
				color.b * b
		};
	}

	GPU_LINE(__host__ __device__) 
	RGB_float operator*(unit multi)
	{
		return { 
				(r * multi),
				(g * multi),
				(b * multi)
		};
	}
};

RGB_float random_color()
{
	return
	{
		Random_functions::random_unit_0_to_1(),
		Random_functions::random_unit_0_to_1(),
		Random_functions::random_unit_0_to_1()
	};
}

class RGB_float_sum_and_avg
{
	u64 counter;
	unit r;
	unit g;
	unit b;

	GPU_LINE(__host__ __device__) 
	void make_average()
	{
		if (counter <= 1) return;

		r /= counter;
		g /= counter;
		b /= counter;
	}

public:
	GPU_LINE(__host__ __device__) 
	RGB_float_sum_and_avg()
		:counter(0), r(u(0)), g(u(0)), b(u(0))
	{
	}

	GPU_LINE(__host__ __device__) 
	u64 check_counter() const
	{
		return counter;
	}

	GPU_LINE(__host__ __device__) 
	void add_color(const RGB_float& adding_color)
	{
		r += adding_color.r;
		g += adding_color.g;
		b += adding_color.b;

		counter++;
	}

	GPU_LINE(__host__ __device__) 
	RGB_float_sum_and_avg& operator+=(const RGB_float& other)
	{
		this->add_color(other);
		return *this;
	}

	GPU_LINE(__host__ __device__) 
	RGB_float_sum_and_avg& operator=(const RGB_float_sum_and_avg& other)
	{
		THIS_OTHER(counter);
		THIS_OTHER(r);
		THIS_OTHER(g);
		THIS_OTHER(b);

		return *this;
	}

	GPU_LINE(__host__ __device__) 
	RGB_float return_RGB_float()
	{
		make_average();

		return { r, g, b };
	}
};

struct Light_point
{
    Ray_calculations::d3 pos;
    RGB_float light_color;

    Light_point(const Ray_calculations::d3& _pos = Ray_calculations::d3(Ray_calculations::random_position(), 10, Ray_calculations::random_position()), const RGB_float& _light_color = random_color())
        :c_init(pos), c_init(light_color)
    {
    }
};

class ray_calculations_lights_BASE : public ray_calculations_BASE
{
protected:
	vector<Light_point> host_light_points;
	u64 MEM_SIZE_Light_points;

	#ifdef GPU
		Light_point* dev_light_points;
	#endif

public:
	ray_calculations_lights_BASE(string name, u64 s)
		:ray_calculations_BASE(name, s), host_light_points(ARRAY_SIZE)
	{
		MEM_SIZE_Light_points = ARRAY_SIZE * sizeof(Light_point);
	}
};

#pragma once
#include "__preprocessor__.h"
#include "benchmark_base.h"
#include <math.h>

struct Floating_functions
{
    GPU_LINE(__host__ __device__)
    inline static bool comparison_equal_to_zero(unit val)
    {
        return (abs(val) < def_EPSILON);
    }

    GPU_LINE(__host__ __device__)
    inline static bool comparison_bigger_than_zero(unit val)
    {
        return (def_EPSILON < val);
    }

    GPU_LINE(__host__ __device__)
    inline static bool comparison_smaller_than_zero(unit val)
    {
        return (val < -def_EPSILON);
    }

    GPU_LINE(__host__ __device__)
    inline static bool comparison_equal(unit a, unit b)
    {
        return comparison_equal_to_zero(a - b);
    }

    GPU_LINE(__host__ __device__)
    inline static bool comparison_bigger___true_if_a_bigger_than_b(unit a, unit b)
    {
        return comparison_bigger_than_zero(a - b);
    }

    GPU_LINE(__host__ __device__)
    inline static bool comparison_smaller___true_if_a_smaller_than_b(unit a, unit b)
    {
        return comparison_smaller_than_zero(a - b);
    }
};

struct d3
{
	unit x;
	unit y;
	unit z;

public:

	GPU_LINE(__host__ __device__)
	d3(const unit& xx = -1, const unit& yy = -1, const unit& zz = -1) :x(xx), y(yy), z(zz) {}
	
	GPU_LINE(__host__ __device__)
	void normalize()
	{
		unit distance = d3::distance(*this);

		x /= distance;
		y /= distance;
		z /= distance;
	}
	
	GPU_LINE(__host__ __device__)
	void negate()
	{
		x = -x;
		y = -y;
		z = -z;
	}

	GPU_LINE(__host__ __device__)
	unit dot_prod(const d3& other)
	{
		return 
			(this->x * other.x) +
			(this->y * other.y) +
			(this->z * other.z);
	}
	
	GPU_LINE(__host__ __device__)
	d3 operator-(const d3& other)
	{
		return d3(
			(this->x - other.x),
			(this->y - other.y),
			(this->z - other.z)
		);
	}
	
	GPU_LINE(__host__ __device__)
	d3 operator*(const unit& mult)
	{
		return d3(
			(this->x * mult),
			(this->y * mult),
			(this->z * mult)
		);
	}

	GPU_LINE(__host__ __device__)
	d3& operator=(const d3& other)
	{
            THIS_OTHER(x);
            THIS_OTHER(y);
            THIS_OTHER(z);

		return *this;
	}

    GPU_LINE(__host__ __device__)
    static unit distance(const d3& not_normalized_vector)
    {
        unit sum = (not_normalized_vector.x * not_normalized_vector.x) +
            (not_normalized_vector.y * not_normalized_vector.y) +
            (not_normalized_vector.z * not_normalized_vector.z);

        return sqrt(sum);
    }

    GPU_LINE(__host__ __device__)
    static unit distance_between(const d3& obj1, const d3& obj2)
    {
        return distance(d3(obj1.x - obj2.x, obj1.y - obj2.y, obj1.z - obj2.z));
    }
};

namespace Sphere_intersection
{
    d3 random_position()
    {
        return
        {
            100 * Random_functions::random_unit_minus_1_to_1(),
            100 * Random_functions::random_unit_minus_1_to_1(),
            100 * Random_functions::random_unit_minus_1_to_1()
        };
    }
    d3 random_direction()
    {
        return
        {
            Random_functions::random_unit_minus_1_to_1(),
            Random_functions::random_unit_minus_1_to_1(),
            Random_functions::random_unit_minus_1_to_1()
        };
    }
    unit random_radious()
    {
        return 1 + (9 * Random_functions::random_unit_0_to_1());
    }
}

struct Ray
{
	d3 s; // starting position
	d3 d; // direction
	unit distance;
	bool uninitialized_values;

public:

    GPU_LINE(__host__ __device__)
    Ray(d3 _s = Sphere_intersection::random_position(), d3 _d = Sphere_intersection::random_direction())
        :c_init(s), c_init(d)
    {
        distance = d3::distance(d);
    }

    GPU_LINE(__host__ __device__)
    unit dot_prod(const Ray& other) const
    {
        return (
                    (this->d.x * other.d.x) +
                    (this->d.y * other.d.y) +
                    (this->d.z * other.d.z)
            );
    }

    GPU_LINE(__host__ __device__)
    unit dot_prod(const Ray& r1, const Ray& r2)
    {
        return r1.dot_prod(r2);
    }

    GPU_LINE(__host__ __device__)
    unit angle(const Ray& other) const
    {
        unit cos_x = (this->dot_prod(other));

        UNIT_FLOAT_LINE(unit ret = acosf(cos_x));
        UNIT_DOUBLE_LINE(unit ret = acosl(cos_x));

        return ret;
    }

    GPU_LINE(__host__ __device__)
    unit angle(const Ray& r1, const Ray& r2)
    {
        return r1.angle(r2);
    }

    GPU_LINE(__host__ __device__)
    unit cos(const Ray& other) const
    {
        unit ret = (this->dot_prod(other));
        
        return ret;
    }

    GPU_LINE(__host__ __device__)
    unit cos(const Ray& r1, const Ray& r2)
    {
        return r1.cos(r2);
    }

    GPU_LINE(__host__ __device__)
    void modify_after_construction(const d3& starting_pos, const d3& ending_pos)
    {
        uninitialized_values = false;

        s = starting_pos;
    }

    GPU_LINE(__host__ __device__)
    void modify_after_construction(const d3& starting_pos, const unit& d_x, const unit& d_y, const unit& d_z)
    {
        uninitialized_values = false;

        s = starting_pos;

        d.x = d_x;
        d.y = d_y;
        d.z = d_z;

        distance = d3::distance(d);
    }

    GPU_LINE(__host__ __device__)
    void modify_after_construction_random_in_normal_hemisphere(const d3& starting_pos, const Ray& normal)
    {
        uninitialized_values = false;

        s = starting_pos;
        
        d.x = Random_functions::random_unit_minus_1_to_1();
        d.y = Random_functions::random_unit_minus_1_to_1();
        d.z = Random_functions::random_unit_minus_1_to_1();

        distance = d3::distance(d);

        d.normalize();

        if (dot_prod(*this, normal) < 0)
        {
            d.negate();
        }
    }

    GPU_LINE(__host__ __device__)
    void modify_after_constructionp_perfect_mirror(const d3& starting_pos, const d3& hit_ray_direction, const Ray& normal)
    {
        uninitialized_values = false;

        s = starting_pos;

        d3 Incoming = hit_ray_direction;
        d3 Normal = normal.d;

        unit dot_product = Incoming.dot_prod(Normal);
        d = Incoming - (Normal * (u(2) * dot_product));


        distance = d3::distance(d); d.normalize();
    }
};

struct Hit_info
{
    bool intersection_with_object;
    d3 pos;

    Hit_info(bool _intersection_with_object = false, d3 _pos = {u(0), u(0), u(0)})
        :c_init(intersection_with_object), c_init(pos)
    {
    }
};

struct Sphere
{
	d3 center_pos;
	unit radious;
	
public:
	
	GPU_LINE(__host__ __device__)   Sphere(d3 c_pos = Sphere_intersection::random_position(), unit rad = Sphere_intersection::random_radious())
        :center_pos(c_pos), radious(rad)
    {
    }
	GPU_LINE(__host__ __device__)   Hit_info intersection(const Ray& ray)    
	{
		unit a, b, c, delta;

		a =
			pow2(ray.d.x) +
			pow2(ray.d.y) +
			pow2(ray.d.z)
			;

		b =
			2 *
				(
				ray.d.x * (ray.s.x - center_pos.x) +
				ray.d.y * (ray.s.y - center_pos.y) +
				ray.d.z * (ray.s.z - center_pos.z)
				);

		c =
			pow2(center_pos.x) +
			pow2(center_pos.y) +
			pow2(center_pos.z) +

			pow2(ray.s.x) +
			pow2(ray.s.y) +
			pow2(ray.s.z)
			- pow2(radious)

			-2 * (
						(center_pos.x * ray.s.x) +
						(center_pos.y * ray.s.y) +
						(center_pos.z * ray.s.z)
					);;

		delta = (b * b) - 4 * (a * c);

		// no hit
		if (Floating_functions::comparison_smaller_than_zero(delta))
			return { false };		

		unit t = ((-b - sqrt(delta)) /
			(2 * a));

		if (t < 0.1)
		{
			t = ((-b + sqrt(delta)) /
				(2 * a));
		}

		if (t < 0.01)
		{
			return { false };
		}
        
        return
        {
                true,
                    d3(
                            (ray.s.x + (t * ray.d.x)),
                            (ray.s.y + (t * ray.d.y)),
                            (ray.s.z + (t * ray.d.z))
                    )
        };
	}
};

struct closest_sphere_hit
{
    d3 intersection_pos;
	Sphere* sphere;

    closest_sphere_hit()
    {
        memset(this, 0, sizeof(*this));
    }
};

class Distance_contest_spheres
{
	d3 reference_pos;

	u32 counter;
	unit current_closest_distance;
    closest_sphere_hit closest;

public:

    GPU_LINE(__host__ __device__)
	Distance_contest_spheres(const d3& _reference_pos)
        :c_init(reference_pos), counter(0), current_closest_distance(u(0)), closest() {}

    // ju¿ wczeœniej jest sprawdzane czy mamy trafienie czy nie

    GPU_LINE(__host__ __device__)
    void new_sphere(const d3& new_intersection_pos, Sphere* sphere)
    {
        if (counter == 0)
        {
            counter++;
            closest.intersection_pos = new_intersection_pos;
            closest.sphere = sphere;
            current_closest_distance = d3::distance_between(reference_pos, closest.intersection_pos);
        }
        else
        {
            unit distance_to_new_sphere = d3::distance_between(reference_pos, new_intersection_pos);

            if (distance_to_new_sphere < current_closest_distance)
            {
                counter++;
                closest.intersection_pos = new_intersection_pos;
                closest.sphere = sphere;
                current_closest_distance = distance_to_new_sphere;
            }
        }
    }

    GPU_LINE(__host__ __device__)
    closest_sphere_hit get_closest()
    {
        return closest;
    }

    GPU_LINE(__host__ __device__)
    Hit_info get_closest_hit_info()
    {
        return {counter, closest.intersection_pos};
    }
};



class sphere_intersection_BASE : public Benchmark_BASE
{
protected:
    // MEMBERS //
    vector<Ray> host_rays;
    vector<Sphere> host_spheres;
    vector<Hit_info> host_output_hit_infos;

    #ifdef GPU
    Ray* dev_rays;
    Sphere* dev_spheres;
    Hit_info* dev_output_hit_infos;
    #endif

    u64 MEM_SIZE_Ray;
    u64 MEM_SIZE_Sphere;
    u64 MEM_SIZE_Hit_info;

    // FUNCTIONS //

    #ifdef GPU
        void cuda_prep()
        {
            // Alokacja pamięci na urządzeniu
            CCE(cudaMalloc((void**)&dev_rays, MEM_SIZE_Ray));
            CCE(cudaMalloc((void**)&dev_spheres, MEM_SIZE_Sphere));
            CCE(cudaMalloc((void**)&dev_output_hit_infos, MEM_SIZE_Hit_info));

            // Kopiowanie danych do urządzenia
            CCE(cudaMemcpy(dev_rays, host_rays.data(), MEM_SIZE_Ray, cudaMemcpyHostToDevice));
            CCE(cudaMemcpy(dev_spheres, host_spheres.data(), MEM_SIZE_Sphere, cudaMemcpyHostToDevice));
            CCE(cudaMemcpy(dev_output_hit_infos, host_output_hit_infos.data(), MEM_SIZE_Hit_info, cudaMemcpyHostToDevice));
        }
        void cuda_finish()
        {
            //CCE(cudaDeviceReset());
            
            CCE(cudaFree(dev_rays));
            CCE(cudaFree(dev_spheres));
            CCE(cudaFree(dev_output_hit_infos));
        }
        friend __global__ void sphere_intersection_one_sphere_Kernel();
        friend __global__ void sphere_intersection_one_distance_contest_Kernel(); // zrobić pętlę co się wykona tylko raz
        friend __global__ void sphere_intersection_distance_contests_MANY_Kernel(); // ta sama pętla TUTAJ
    #endif

public:

    sphere_intersection_BASE(string name, u64 s) :Benchmark_BASE(name, s, -1),
                                host_rays(ARRAY_SIZE), host_spheres(ARRAY_SIZE), host_output_hit_infos(ARRAY_SIZE)
    {
        MEM_SIZE_Ray = s * sizeof(Ray);
        MEM_SIZE_Sphere = s * sizeof(Sphere);
        MEM_SIZE_Hit_info = s * sizeof(Hit_info);

        // DEFAULT CONSTRUCTOR did it's thing and randomised
    }
};


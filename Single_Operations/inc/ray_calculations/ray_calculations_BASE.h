#pragma once
#include "__preprocessor__.h"
#include "benchmark_base.h"


namespace Ray_calculations
{
    unit random_position()
    {
        return  10 * Random_functions::random_unit_minus_1_to_1();
    }

    struct d3
    {
        unit x;
        unit y;
        unit z;

    public:

        GPU_LINE(__host__ __device__)
        d3(const unit& xx = Ray_calculations::random_position(), const unit& yy = 0, const unit& zz = Ray_calculations::random_position()) :x(xx), y(yy), z(zz) {}
        
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
        Ray_calculations::d3& operator=(const Ray_calculations::d3& other)
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

    d3 random_direction()
    {
        return
        {
            Random_functions::random_unit_minus_1_to_1(),
            Random_functions::random_unit_minus_1_to_1(),
            Random_functions::random_unit_minus_1_to_1()
        };
    }

    struct Ray
    {
        Ray_calculations::d3 s; // starting position
        Ray_calculations::d3 d; // direction
        unit distance;
        bool uninitialized_values;

    public:

        GPU_LINE(__host__ __device__)
        Ray(Ray_calculations::d3 _s = random_position(), Ray_calculations::d3 _d = random_direction())
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
}

class ray_calculations_BASE : public Benchmark_BASE
{
protected:
    // MEMBERS //
    vector<Ray_calculations::d3> host_points_on_a_plane; // MEM_SIZE
    Ray_calculations::Ray host_normal;

    #ifdef GPU
        Ray_calculations::d3* dev_points_on_a_plane;
        Ray_calculations::Ray* dev_normal;
    #endif

    u64 MEM_SIZE_Ray;

    #ifdef GPU
        friend __global__ void ray_calculations_surface_diffuse_Kernel();
        friend __global__ void light_calculations_surface_mirror_Kernel();
        friend __global__ void light_calculations_intensity_based_on_angle_Kernel();
    #endif

public:

    ray_calculations_BASE(string name, u64 s) :Benchmark_BASE(name, s, s * sizeof(Ray_calculations::d3)),
                                host_points_on_a_plane(ARRAY_SIZE),
                                host_normal(Ray_calculations::d3(0, 0, 0), Ray_calculations::d3(0, 1, 0))
    {
        MEM_SIZE_Ray = sizeof(Ray_calculations::Ray);

        // d3 DEFAULT CONSTRUCTOR randomised on init
    }
};
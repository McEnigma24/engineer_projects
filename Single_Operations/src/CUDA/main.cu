#include "__preprocessor__.h"
#include "benchmark_base.h"

#include "vec_add.h"
#include "vec_multi.h"

#include "if_statements_2_random.h"
#include "if_statements_2_alternating.h"
#include "if_statements_2_continuous.h"

#include "if_statements_3_random.h"
#include "if_statements_3_alternating.h"
#include "if_statements_3_continuous.h"

#include "if_statements_4_random.h"
#include "if_statements_4_alternating.h"
#include "if_statements_4_continuous.h"

#include "sphere_intersection_one_sphere.h"
#include "sphere_intersection_distance_contests_MANY.h"

#include "ray_calculations_surface_diffuse.h"
#include "ray_calculations_surface_diffuse_MANY.h"

#include "ray_calculations_lights_surface_mirror.h"
#include "ray_calculations_lights_surface_mirror_MANY.h"

#include "ray_calculations_lights_intensity_based_on_angle.h"
#include "ray_calculations_lights_intensity_based_on_angle_MANY.h"


class Job_Benchmarker
{
    vector<Benchmark_BASE*> jobs;

public:
    Job_Benchmarker() { }
    ~Job_Benchmarker()
    {
        Global::progress_all = jobs.size() * Global::REP_NUMBER;
        CPU_LINE(Global::progress_all *= def_MAX_NUM_OF_THREADS);
        for(auto& j : jobs)
        {    
            j->start();
        }
        
        OUTPUT_TO_FILE(common_file, "\n");
    }

    void add_job(Benchmark_BASE* job)
    {
        jobs.emplace_back(job);
    }
};

string Global::model_name = "default";
string Global::num_of_cores = "default";
u64 Global::progress_counter = 0;
u64 Global::progress_all = 0;
float Global::scaling_multi = -1;
float Global::scaling_add = -1;

const string path_rep_num = "../_run_time_config_/rep_number.txt";
u64 Global::REP_NUMBER = 0;

void good_to_go(int argc, char* argv[])
{
    if(argc >= 4)
    {
        CPU_LINE(Global::model_name = argv[1]);
        GPU_LINE(Global::model_name = argv[2]);
    
        CPU_LINE(Global::num_of_cores = argv[3]);
        GPU_LINE(Global::num_of_cores = "0");

        if(argc == 6)
        {
            Global::scaling_multi = atof(argv[4]);
            Global::scaling_add = atof(argv[5]);
        }
    }
    else
    {
        cout << "NOT ENOUGHT PARAMS fead into main function\n";
        exit(0);
    }

    #ifdef GPU
    if(Global::model_name == "nvidia_gpu_not_present")
    {
        cout << "Nvidia GPU not present. Exiting...\n";
        exit(0);
    }
    #endif

    ifstream FILE(path_rep_num);
    if (!FILE.is_open()) {
        std::cerr << "Nie mozna otworzyc pliku: " << path_rep_num << std::endl;
        exit(0);
    }
    FILE >> Global::REP_NUMBER;

    if(Global::REP_NUMBER == 0) { cout << "REP_NUMBER == 0\n"; exit(0); }
}

#define job(name, ...) new name(#name, __VA_ARGS__)

vector<u64> create_job_sizes(const u64 incrementer, const u64 limit, u64 size)
{
    vector<u64> ret;
    for(;;)
    {
        if(size <= limit)
        {
            ret.push_back(size);
            size += incrementer;
        }
        else break;
    }
    return ret;
}

// Simple Operations - O(n)
void create_simple_jobs(Job_Benchmarker& bencher, const u64 incrementer, const u64 limit, u64 size)
{
    vector<u64> sizes = create_job_sizes(incrementer, limit, size);

    for(auto& s : sizes)
    {
        bencher.add_job(job(vec_add, s));
        bencher.add_job(job(vec_multi, s));
        bencher.add_job(job(if_statements_2_random, s));
        bencher.add_job(job(if_statements_2_alternating, s));
        bencher.add_job(job(if_statements_2_continuous, s));
        bencher.add_job(job(if_statements_3_random, s));
        bencher.add_job(job(if_statements_3_alternating, s));
        bencher.add_job(job(if_statements_3_continuous, s));
        bencher.add_job(job(if_statements_4_random, s));
        bencher.add_job(job(if_statements_4_alternating, s));
        bencher.add_job(job(if_statements_4_continuous, s));
        bencher.add_job(job(sphere_intersection_one_sphere, s));
        bencher.add_job(job(ray_calculations_surface_diffuse, s));
        bencher.add_job(job(ray_calculations_lights_surface_mirror, s));
        bencher.add_job(job(ray_calculations_lights_intensity_based_on_angle, s));
    }
}

// Complex Operations - O(n^2)
void create_complex_jobs(Job_Benchmarker& bencher, const u64 incrementer, const u64 limit, u64 size)
{
    vector<u64> sizes = create_job_sizes(incrementer, limit, size);

    for(auto& s : sizes)
    {
        bencher.add_job(job(sphere_intersection_distance_contests_MANY, s));
        bencher.add_job(job(ray_calculations_surface_diffuse_MANY, s));
        bencher.add_job(job(ray_calculations_lights_surface_mirror_MANY, s));
        bencher.add_job(job(ray_calculations_lights_intensity_based_on_angle_MANY, s));
    }
}

int main(int argc, char* argv[])
{
    good_to_go(argc, argv);

    Job_Benchmarker bencher;

    #ifdef NORMAL_TASK
    create_simple_jobs(bencher,
                                // inc
                                1000,

                                // limit
                                NUM_2(50, 000),

                                // starting size
                                1000
                        );

    create_complex_jobs(bencher,
                                // inc
                                10,

                                // limit
                                600,

                                // starting size
                                50
                        );
    #endif

    #ifdef BIG_TASK

    create_simple_jobs(bencher,
                                // inc
                                NUM_2(5, 000),

                                // limit
                                UNIT_FLOAT_LINE(NUM_2(500, 000),)
                                UNIT_DOUBLE_LINE(NUM_2(375, 000),)

                                // starting size
                                NUM_2(75, 000)
                        );

    create_complex_jobs(bencher,
                               // inc
                                NUM_2(5, 000),

                                // limit
                                NUM_2(75, 000),

                                // starting size
                                NUM_2(5, 000)
                        );
    #endif

    cout << "Jobs Scheduled. Starting execution...\n";
    
    return 0;
}


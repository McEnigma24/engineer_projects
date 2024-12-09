#include "benchmark_base.h"
#include <sstream>
#include "_SINGLE_.h"
#include <iomanip>

// COMMON //

#define PROGRESS_LOG

#ifdef PROGRESS_LOG
#define OLD_LOG_LINE(...)  
#define PROGRESS_LOG_LINE(...) __VA_ARGS__
#else
#define OLD_LOG_LINE(...) __VA_ARGS__
#define PROGRESS_LOG_LINE(...) 
#endif

#ifdef CPU
void Benchmark_BASE::omp_launch()
{
    #pragma omp parallel for schedule(static)
    for(int i=0; i<ARRAY_SIZE; i++)
    {
        cpu_task(i);
    }
}
#endif

Benchmark_BASE::Benchmark_BASE(string dn, u64 array_size, u64 mem_size)
    :derived_name(dn), ARRAY_SIZE(array_size), MEM_SIZE(mem_size),
    CPU_LINE(all_stats(def_MAX_NUM_OF_THREADS + 1))
    GPU_LINE(all_stats(1))
{
    GPU_LINE(setup_block_size());
}

#ifdef GPU
void Benchmark_BASE::setup_block_size()
{
    // <<<NUMBER_OF_BLOCKS, BLOCK_SIZE>>>

    // BLOCK_SIZE = ARRAY_SIZE; // no difference
    // NUMBER_OF_BLOCKS = 1;

    // BLOCK_SIZE = 1; // może warto spróbować          // JEST duża różnica i jest turbo WOLNO
    // NUMBER_OF_BLOCKS = ARRAY_SIZE;

    BLOCK_SIZE = def_BLOCK_SIZE;
    NUMBER_OF_BLOCKS = ARRAY_SIZE / BLOCK_SIZE + 1; // jest +1 żeby pokrywało wszystko -> przez to musi być warunek
}
#endif


float calculate_progress()
{
    float ret = (((float) Global::progress_counter) / ((float) Global::progress_all));

    if(!((Global::scaling_multi == -1) && (Global::scaling_add == -1)))
    {
        ret = ret * Global::scaling_multi + Global::scaling_add;
    }
    else
    {
        // DEFAULT //
        ret = ret * 1.0f + 0.0f;
    }

    return ret;
}

string get_current_local_time()
{
    ostringstream oss;

    if(Global::REP_NUMBER == 1)
    {
        auto now = std::chrono::system_clock::now();
        std::time_t czas = std::chrono::system_clock::to_time_t(now);

        // Korzystamy z std::localtime w systemach Linux/Unix
        std::tm* aktualnyCzas = std::localtime(&czas);  // Zwraca wskaźnik na strukturę tm

        oss << std::put_time(aktualnyCzas, "%H:%M:%S") << std::endl;
    }
    else
    {
        // Pobranie aktualnego czasu lokalnego
        auto now = std::chrono::system_clock::now();
        std::time_t current_time = std::chrono::system_clock::to_time_t(now);
        std::tm* current_tm = std::localtime(&current_time);

        // Odczyt daty z pliku
        std::ifstream file("../_run_time_config_/estimated_finish_date.txt");
        if (!file.is_open()) {
            std::cerr << "Nie można otworzyć pliku: " << "../_run_time_config_/estimated_finish_date.txt" << std::endl;
            exit(0);
        }

        int year, month, day, hour, minute, second; // 2024 11 15 10 48 02
        file >> year >> month >> day >> hour >> minute >> second;
        file.close();

        // cout << year << " " << month << " " << day << " " << hour << " " << minute << " " << second << "   ";

        // Stworzenie obiektu tm dla daty z pliku
        std::tm target_tm = {};
        target_tm.tm_year = year - 1900;  // tm_year jest liczbą lat od 1900
        target_tm.tm_mon = month - 1;    // tm_mon jest liczbą miesięcy od 0 (styczeń)
        target_tm.tm_mday = day;
        target_tm.tm_hour = hour;
        target_tm.tm_min = minute;
        target_tm.tm_sec = second;

        // Konwersja na std::time_t
        std::time_t target_time = std::mktime(&target_tm);
        if (target_time == -1) {
            std::cerr << "Nieprawidłowa data w pliku." << std::endl;
            return "error";
        }

        // Obliczenie różnicy czasu
        //auto diff = std::difftime(current_time, target_time);
        auto diff = std::difftime(target_time, current_time);
        if (diff < 0) {
            // std::cout << "Podana data już minęła." << std::endl;
            return "very close to finishing...\n";
        }

        // Przekształcenie różnicy na lata, miesiące, dni, godziny, minuty i sekundy
        auto diff_seconds = static_cast<int>(diff);
        int days = diff_seconds / (24 * 3600);
        diff_seconds %= (24 * 3600);
        int hours = diff_seconds / 3600;
        diff_seconds %= 3600;
        int minutes = diff_seconds / 60;
        int seconds = diff_seconds % 60;

        // Wyświetlenie wyniku

        if(days != 0) oss << days << "d ";
        if(hours != 0) oss << hours << "h ";
        if(minutes != 0) oss << minutes << "m ";
        if(seconds != 0) oss << seconds << "s";
        oss << "\n";
    }

	return oss.str();
}

void Benchmark_BASE::single_round(int num_of_threads)
{
    OLD_LOG_LINE(cout << "single round " << num_of_threads << "\n");

    PROGRESS_LOG_LINE(printf("[%.0f%]", (100.0f * calculate_progress())));
    PROGRESS_LOG_LINE(cout << "       " << get_current_local_time();)
    GPU_LINE(cuda_prep());
    GPU_LINE(CCE(cudaDeviceSynchronize()));
    CPU_LINE(omp_set_num_threads(num_of_threads););

    Timer t;

    t.start();
    {
        CPU_LINE(omp_launch());
        GPU_LINE(cuda_launch());
    }
    t.end();
    PROGRESS_LOG_LINE(Global::progress_counter++);

    all_stats[num_of_threads].push(t.get_all_in_nano());

    GPU_LINE(cuda_finish());
    //GPU_LINE(cudaDeviceReset());
}

void Benchmark_BASE::round_controller()
{
    OLD_LOG_LINE(cout << "round controller\n");
    for(int rep = 0; rep < Global::REP_NUMBER; rep++)
    {
        OLD_LOG_LINE(cout << rep + 1 << endl);
        
        CPU_LINE(for(int i=1; i <= def_MAX_NUM_OF_THREADS; i++) { single_round(i); });

        GPU_LINE(single_round(0));
    }
}

/////////////////////////////////////////////////////////////////////////////

#include "_PARSER_Format_Buffer_Common_IMP_.h"

/////////////////////////////////////////////////////////////////////////////

void Benchmark_BASE::save_stats()
{
    #ifdef CPU
    stringstream stream;
    stream << fixed << std::setprecision(2) << (float)omp_get_num_procs() / (float)stoi(Global::num_of_cores);
    #endif

    tupl variables =
    {
        0.0,
        0,
        0,
        0,

        CPU_LINE("CPU")
        GPU_LINE("GPU")
        ,

        Global::model_name,

        ARCH_X86_LINE("x86")
        ARCH_X64_LINE("x64")
        GPU_LINE("CUDA")
        ,

        UNIT_FLOAT_LINE("float")
        UNIT_DOUBLE_LINE("double")
        ,

        Global::num_of_cores,

        CPU_LINE(to_string(omp_get_num_procs()))
        GPU_LINE(to_string(0))
        ,

        CPU_LINE(stream.str())
        GPU_LINE(to_string(0))
        ,

        derived_name,
        to_string(ARRAY_SIZE),
        
        CPU_LINE(to_string(0))
        GPU_LINE(to_string(0))
        ,
        
        CPU_LINE("measuring")
        GPU_LINE("BEST")
    };

    #ifdef CPU
        tupl best_time_variables;
        vector<u64> best_data;
        u64 best_time_so_far = -1; // biggest uint64_t

        for(int i=1; i <= def_MAX_NUM_OF_THREADS; i++)
        {
            auto [min, avg, max, rel_dev] = all_stats[i].get();

            GET(variables, 0) = rel_dev;

            GET(variables, NUM_OF_THREADS + 4) = to_string(i);
            GET(variables, 1) = min;
            GET(variables, 2) = avg;
            GET(variables, 3) = max;

            if(avg < best_time_so_far)
            {
                best_data = all_stats[i].get_data();
                best_time_so_far = avg;
                best_time_variables = variables;
            }

            string output = Format_Buffer::input_variables_return_log_line(variables);
            insert_raw_datapoints(output, all_stats[i].get_data());
            output += "\n";
            OUTPUT_TO_FILE(common_file, output);
        }
        GET(best_time_variables, RATING + 4) = "BEST";
        string output = Format_Buffer::input_variables_return_log_line(best_time_variables);
        insert_raw_datapoints(output, best_data);
        output += "\n";
        OUTPUT_TO_FILE(common_file, output);
    #endif

    #ifdef GPU
        auto [min, avg, max, rel_dev] = all_stats[0].get();

        GET(variables, 0) = rel_dev;

        GET(variables, 1) = min;
        GET(variables, 2) = avg;
        GET(variables, 3) = max;

        string output = Format_Buffer::input_variables_return_log_line(variables);
        insert_raw_datapoints(output, all_stats[0].get_data());
        output += "\n";
        OUTPUT_TO_FILE(common_file, output);
    #endif
}

void Benchmark_BASE::start()
{
    OLD_LOG_LINE(cout << "starting -> " << derived_name << " ( " << ARRAY_SIZE << " )" << endl);

    round_controller();
    save_stats();
}

/////////////////////////////////////////////////////////

// CUSTOM for any derived //

CPU_LINE(inline void Benchmark_BASE::cpu_task(int i) { FATAL("virtual function not implemented"); })

GPU_LINE(void Benchmark_BASE::cuda_prep() { FATAL("virtual function not implemented"); })
GPU_LINE(void Benchmark_BASE::cuda_launch() { FATAL("virtual function not implemented"); })
GPU_LINE(void Benchmark_BASE::cuda_finish() { FATAL("virtual function not implemented"); })

// + KONSTRUKTOR gdzie przyjmuje jakieś dane i robi ich prep + ustawiał ARRAY_SIZE
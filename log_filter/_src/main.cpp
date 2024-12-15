#include "__preprocessor__.h"
#include "stats_pack.h"
#include <filesystem>
#include <vector>

#ifdef SINGLE
#include "_SINGLE_.h"
#endif

#ifdef MAIN
#include "_MAIN_.h"
#endif

/////////////////////////////////////////////////////////////////////////////

#include "_PARSER_Format_Buffer_Common_IMP_.h"

/////////////////////////////////////////////////////////////////////////////

#ifdef MAIN
    const string dir_output = "output/Ray_Tracer";
    const string dir_input = "input/Ray_Tracer";
#endif
#ifdef SINGLE
    const string dir_output = "output/Single_Operations";
    const string dir_input = "input/Single_Operations";
#endif

vector<string> get_all_file_lines(const string& file_path)
{
    vector<string> ret;

    ifstream is(file_path); string line;
    while(getline(is, line))
    {
        ret.push_back(line);
    }

    return ret;
}

string raw_data_points_STR(const string& whole_line)
{
    string raw_data_points;
    size_t pos = whole_line.find('{');
    if (pos != std::string::npos)
    {
        raw_data_points = whole_line.substr(pos + 1);
        raw_data_points = raw_data_points.substr(0, raw_data_points.size() - 2);
    }

    return raw_data_points;
}

vector<u64> get_all_raw_data_points(const string& raw_data_points_STR)
{
    vector<u64> ret;
    if(raw_data_points_STR.size() == 0) return ret;

    istringstream iss(raw_data_points_STR.substr(1));
    string s;
    while(getline(iss, s, ' '))
    {
        std::stringstream sstr(s);
        u64 val;
        sstr >> val;

        ret.push_back(val);
    }

    return ret;
}

string data_points_to_string(const vector<u64>& all_raw_data_points)
{
    string ret;
    // ret += to_string(all_raw_data_points.size() - 1);
    ret += "{ ";
    for(auto& data_point : all_raw_data_points)
    {
        ret += to_string(data_point) + " ";
    }
    ret += "}";
    return ret;
}

void adding_unique_strings(tupl& all_tuples)
{
    auto& [adding_read] = all_tuples;

    read_processor += "_proc";
    read_model += "_mod";
    read_arch += "_arch";
    read_unit += "_unit";
    read_physical_cores += "_p_cores";
    read_logical_cores += "_l_cores";
    read_core_proportion += "_prop_cores";

    read_num_of_threads += "_th";
    read_rating += "_r";

    #ifdef MAIN
        read_scene += "_scene";
        read_lights += "_lights";
        read_spheres += "_spheres";
        read_bounces += "_bounces";
    #endif
    #ifdef SINGLE
        read_task += "_task";
        read_size += "_size";
    #endif
}

double deviation_of_single_point(const u64 _x, const u64 _avg)
{
    double x = _x;
    double avg = _avg;

    double difference = x - avg;
    difference = pow2(difference);
    difference = sqrt(difference);

    return 100 * (difference / avg);
}

vector<u64> single_line_recal(tupl& all_tuples, const vector<u64>& all_raw_data_points)
{
    auto& [adding_read] = all_tuples;
    stats_pack filtered_stats;

    for(auto& x : all_raw_data_points)
    {
        if(not(100.0 < deviation_of_single_point(x, read_VALUE_avg)))
        {
            filtered_stats.push(x);
        }
    }

    auto [min, avg, max, rel_dev] = filtered_stats.get();

    read_VALUE_min = min;
    read_VALUE_avg = avg;
    read_VALUE_max = max;
    read_VALUE_rel_dev = rel_dev;

    return filtered_stats.get_data();
}

static u64 best_time_so_far;
static string best_cpu_thread;

string modyfied_line(tupl& all_tuples, const string& whole_line)
{
    auto& [adding_read] = all_tuples;
    vector<u64> all_raw_data_points = get_all_raw_data_points(raw_data_points_STR(whole_line));

    // line mods
    adding_unique_strings(all_tuples);
    vector<u64> filtered_data_points = single_line_recal(all_tuples, all_raw_data_points);

    // group stats mods
    string ret = Format_Buffer::input_variables_return_log_line(all_tuples) + " " + data_points_to_string(filtered_data_points);
    {
        #define assing_current_values \
        { \
          best_time_so_far = read_VALUE_avg; \
          read_rating = "BEST_r"; \
          best_cpu_thread = Format_Buffer::input_variables_return_log_line(all_tuples) + " " + data_points_to_string(filtered_data_points); \
        }

        if(read_processor == "CPU_proc")
        {
            if(read_rating != "BEST_r")
            {
                if(read_num_of_threads == "1_th")
                {
                    assing_current_values;
                }
                else
                {
                    if(read_VALUE_avg < best_time_so_far)
                    {
                        assing_current_values;
                    }
                }
            }
            else // LINE WITH BEST in IT -> needs to be changed with the saved one
            {
                return best_cpu_thread;
            }
        }
    }

    return ret;
}

vector<string> create_output_lines(const vector<string>& input)
{
    vector<string> ret;

    for(auto& whole_line : input)
    {
        if(whole_line == "") { ret.push_back(""); continue; }
        auto all_tuples = Format_Buffer::input_log_line_output_variables(whole_line);
        auto& [adding_read] = all_tuples;
        
        ret.push_back(modyfied_line(all_tuples, whole_line));
    }

    return ret;
}

string get_just_the_file_name(const string& full_path)
{
    size_t index = full_path.find_last_of("/");

    return full_path.substr(index + 1);
}

void save_file(const string& name, const vector<string>& lines)
{
    ofstream os(name);

    for(auto& line : lines)
    {
        os << line << endl;
    }

    os.close();
}

int main(int argc, char* argv[])
{
    std::filesystem::create_directory(dir_output);

    for (const auto& log_file_path : std::filesystem::directory_iterator(dir_input))
    {
        if (!log_file_path.is_regular_file()) continue;
        string log_file = log_file_path.path().string();

        vector<string> original_file_lines = get_all_file_lines(log_file);
        vector<string> modified_output = create_output_lines(original_file_lines);

        string path = dir_output + string("/") + get_just_the_file_name(log_file);
        save_file(path, modified_output);
    }

    return 0;
}

#include "__preprocessor__.h"
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

vector<string> create_output_lines(const vector<string>& input)
{
    vector<string> ret;

    for(auto& line : input)
    {
        auto all_tuples = Format_Buffer::input_log_line_output_variables(line);
        auto& [adding_read] = all_tuples;

        read_VALUE_rel_dev = 6.9;
        read_VALUE_min = 1;
        read_VALUE_avg = 2;
        read_VALUE_max = 3;

        // dodać to raw data points { }

        ret.push_back(Format_Buffer::input_variables_return_log_line(all_tuples));
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

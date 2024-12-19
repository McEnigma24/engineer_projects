#include "__preprocessor__.h"

// SPECIFIC IMP //

#ifdef SINGLE
#include "_SINGLE_.h"
const string dir_serialized_maps = 
AVG_LINE(string("serialized/avg/single/"))
DEV_LINE(string("serialized/dev/single/"))
+
ORIGINAL_LINE(string("original/"))
FILTERED_LINE(string("filtered/"))
;
#endif

#ifdef MAIN
#include "_MAIN_.h"
const string dir_serialized_maps = 
AVG_LINE(string("serialized/avg/main/"))
DEV_LINE(string("serialized/dev/main/"))
+
ORIGINAL_LINE(string("original/"))
FILTERED_LINE(string("filtered/"))
;
#endif

/////////////////////////////////////////////////////////////////////////////

#include "_PARSER_Format_Buffer_Common_IMP_.h"

#define INVALID_VALUE std::numeric_limits<double>::quiet_NaN()

/////////////////////////////////////////////////////////////////////////////


std::chrono::steady_clock::time_point last_time = std::chrono::steady_clock::now();

// Funkcja do uzyskania aktualnego czasu lokalnego w formacie HH:MM:SS
std::string get_current_local_time() {
    auto now = std::chrono::system_clock::now();
    std::time_t czas = std::chrono::system_clock::to_time_t(now);
    std::tm* aktualnyCzas = std::localtime(&czas);

    std::ostringstream oss;
    oss << std::put_time(aktualnyCzas, "%H:%M:%S");
    return oss.str();
}

// Funkcja do obliczania różnicy czasowej w formacie h, m, s, ms
std::string calculate_time_difference() {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time);
    last_time = now; // Aktualizuj czas ostatniego pomiaru

    // Konwersja na jednostki czasu
    long long milliseconds = duration.count();
    long long hours = milliseconds / (1000 * 60 * 60);
    milliseconds %= (1000 * 60 * 60);
    long long minutes = milliseconds / (1000 * 60);
    milliseconds %= (1000 * 60);
    long long seconds = milliseconds / 1000;
    milliseconds %= 1000;

    // Formatowanie wyniku
    std::ostringstream oss;
    if (hours > 0) oss << hours << "h ";
    if (minutes > 0 || hours > 0) oss << minutes << "m ";
    if (seconds > 0 || minutes > 0 || hours > 0) oss << seconds << "s ";
    oss << milliseconds << "ms";
    return oss.str();
}

#define time_stamp(x) std::cout << "\nTIME_STAMP: " << x << " " << get_current_local_time() << " (+ " << calculate_time_difference() << ")\n";
// #define time_stamp(x) 

void serialize_binary(const std::unordered_map<std::string, double>& map, const std::string& filename)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file)
    {
        throw std::ios_base::failure("Failed to open file for writing");
    }

    size_t size = map.size();
    file.write(reinterpret_cast<const char*>(&size), sizeof(size));

    for (const auto& [key, value] : map)
    {
        size_t key_size = key.size();
        file.write(reinterpret_cast<const char*>(&key_size), sizeof(key_size));
        file.write(key.data(), key_size);

        file.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }

    time_stamp("serialized " << filename)
}
std::unordered_map<std::string, double> deserialize_binary(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        throw std::ios_base::failure("Failed to open file for reading");
    }

    size_t size;
    file.read(reinterpret_cast<char*>(&size), sizeof(size));

    std::unordered_map<std::string, double> map;
    for (size_t i = 0; i < size; ++i)
    {
        size_t key_size;
        file.read(reinterpret_cast<char*>(&key_size), sizeof(key_size));

        std::string key(key_size, '\0');
        file.read(&key[0], key_size);

        double value;
        file.read(reinterpret_cast<char*>(&value), sizeof(value));

        map[key] = value;
    }
    return map;
}



struct Multi_Tree_Node
{
    Multi_Tree_Node* parent;
    vector<Multi_Tree_Node*> children;
    string id;
    double value;

    Multi_Tree_Node(Multi_Tree_Node* p, string i, double v)
        :parent(p)
        ,id(i)
        ,value(v)
    {
    }
};

class Tree
{
    Multi_Tree_Node* top;
    
    void add_one_value_IMP(Multi_Tree_Node* current_node, const vector<string>& all_params_in_order, u64 param_index, const double& value)
    {
        bool last_one = (param_index == all_params_in_order.size() - 1);
        const string current_id = all_params_in_order[param_index];

        if(!last_one)
        {
            for(auto& child : current_node->children)
            {
                if(child->id == current_id)
                {
                    add_one_value_IMP(child, all_params_in_order, param_index + 1, value);
                    return;
                }
            }

            Multi_Tree_Node* new_node = new Multi_Tree_Node(current_node, current_id, INVALID_VALUE); // node pośredni
            current_node->children.push_back(new_node); // dodanie do dzieci parenta
            add_one_value_IMP(new_node, all_params_in_order, param_index + 1, value); // przechodzimy dalej puki nie skończą się parametry
        }
        else
        {
            Multi_Tree_Node* new_node = new Multi_Tree_Node(current_node, current_id, value); // ostatni -> faktyczna wartość
            current_node->children.push_back(new_node); // dodanie do dzieci parenta
        }
    }
    double get_value_IMP(Multi_Tree_Node* current_node, const vector<string>& all_params_in_order, u64 param_index)
    {
        bool last_one = (param_index == all_params_in_order.size());

        if(last_one)
        {
            return current_node->value;
        }
        else
        {
            const string current_id = all_params_in_order[param_index];

            for(auto& child : current_node->children)
            {
                if(child->id == current_id)
                {
                    return get_value_IMP(child, all_params_in_order, param_index + 1);
                }
            }
        }

        return INVALID_VALUE;
    }

public:

    Tree()
    {
        top = new Multi_Tree_Node(nullptr, "", INVALID_VALUE);
    }
    static vector<string> params_to_strings(tupl all_params)
    {
        auto& [adding_read] = all_params;

        vector<string> all_params_in_order;
        all_params_in_order.push_back(read_processor);
        all_params_in_order.push_back(read_model);
        all_params_in_order.push_back(read_arch);
        all_params_in_order.push_back(read_unit);
        all_params_in_order.push_back(read_physical_cores);
        all_params_in_order.push_back(read_logical_cores);
        all_params_in_order.push_back(read_core_proportion);

        #ifdef MAIN
            all_params_in_order.push_back(read_scene);
            all_params_in_order.push_back(read_lights);
            all_params_in_order.push_back(read_spheres);
            all_params_in_order.push_back(read_bounces);
        #endif
        #ifdef SINGLE
            all_params_in_order.push_back(read_task);
            all_params_in_order.push_back(read_size);
        #endif

        all_params_in_order.push_back(read_num_of_threads);
        all_params_in_order.push_back(read_rating);

        return all_params_in_order;
    }
    static vector<string> params_to_strings_not_all_indexes(tupl all_params, const vector<category_index>& ind_TO_SKIP)
    {
        vector<string> all = params_to_strings(all_params);
        vector<string> ret;

        for(auto& index : ind_TO_SKIP)
        {
            all[index] = "";
        }

        for(auto& param : all)
        {
            if(param != "")
                ret.push_back(param);
        }

        return ret;
    }

    double get_value(const vector<string>& all_params_in_order)
    {
        return get_value_IMP(top, all_params_in_order, 0);
    }
    void add_value(const vector<string>& all_params_in_order, const double value)
    {
        add_one_value_IMP(top, all_params_in_order, 0, 
                                                        value
                                                        // AVG_LINE(read_VALUE_avg / NUM_3(1,000,000))
                                                        // DEV_LINE(read_VALUE_rel_dev)
        );
    }

    double get_value(tupl all_params)
    {
        vector<string> all_params_in_order = params_to_strings(all_params);

        return get_value_IMP(top, all_params_in_order, 0);
    }
    void add_value(tupl all_params)
    {
        vector<string> all_params_in_order = params_to_strings(all_params);
        auto& [adding_read] = all_params;

        add_one_value_IMP(top, all_params_in_order, 0,
                                                        AVG_LINE(read_VALUE_avg / NUM_3(1,000,000))
                                                        DEV_LINE(read_VALUE_rel_dev)
        );
    }
};


class All_Category_Combinations
{
    string input_dir;
    string output_file;

    ofstream FILE;
    vector<string> all_lines;
    vector<vector<string>> categories;
    unordered_map<string, double> hash_map;    

    u64 in_which_category_can_we_find_this_value(const string& value)
    {
        int i = 0;
        for(auto& single_category : categories)
        {
            for(auto& single_value : single_category)
            {
                if(value == single_value) return i;
            }

            i++;
        }

        cout << "FATAL ERROR\n";
        exit(0);

        return -1;
    }
    u64 count_string_lengh_after_concatination(const string& x, const vector<string>& parametry_linii, const vector<string>& parametry_wykresu)
    {
        u64 ret = x.size();
        for(auto& l : parametry_linii)      ret += l.size();
        for(auto& w : parametry_wykresu)    ret += w.size();

        return ret;
    }
    void concatinate_string(string& ret, const string& x, const vector<string>& parametry_linii, const vector<string>& parametry_wykresu)
    {
        ret += x;
        for(auto& l : parametry_linii)      ret += l;
        for(auto& w : parametry_wykresu)    ret += w;
    }
    string in_order_param_concatinator(const string& x, const vector<string>& parametry_linii, const vector<string>& parametry_wykresu)
    {
        string ret; ret.reserve(count_string_lengh_after_concatination(x, parametry_linii, parametry_wykresu));
        concatinate_string(ret, x, parametry_linii, parametry_wykresu);

        return ret;
    }
    // HASH //
    string create_index_combination_to_string(const category_index& ind_X, const vector<category_index>& ind_LINEs, const vector<category_index>& ind_CHARTs)
    {
        string ret;

        ret += to_string(ind_X);
        ret+="-";

        for(auto& line : ind_LINEs) ret += to_string(line);
        ret+="-";

        for(auto& chart : ind_CHARTs) ret += to_string(chart);
        ret+="-";

        return ret;
    }

    // HASH //
    void create_hash_table_for_this_set_of_params(const string& hash_table_name, const vector<string>& X_line, const vector<vector<string>>& LINE_combinations, const vector<vector<string>>& GROUP_CHART_combinations, const vector<category_index>& ind_TO_SKIP)
    {
        hash_map.clear();
        const string file_path = dir_serialized_maps + hash_table_name + ".bin";

        ifstream FILE(file_path);
        std::ifstream test_file(file_path, std::ios::binary);
        if(FILE.is_open() && test_file)
        {
            hash_map = deserialize_binary(file_path);
        }
        else
        {
            // tworzymy drzewo
            Tree tree;

            time_stamp("TREE creation - starting...");
            for(auto& line : all_lines)
            {
                if(line == "") continue;

                tupl line_params = Format_Buffer::input_log_line_output_variables(line);

                vector<string> params_in_order_and_filtered = Tree::params_to_strings_not_all_indexes(line_params, ind_TO_SKIP);

                auto& [adding_read] = line_params;
                tree.add_value(params_in_order_and_filtered,
                                                                AVG_LINE((double)read_VALUE_avg / NUM_3(1,000,000))
                                                                DEV_LINE(read_VALUE_rel_dev)
                );
            }
            time_stamp("TREE creation - DONE");

            time_stamp("hash table creation - starting... " << hash_table_name);

            hash_map.reserve(GROUP_CHART_combinations.size() * LINE_combinations.size() * X_line.size());

            for(auto& parametry_wykresu : GROUP_CHART_combinations)
            {
                // time_stamp("hash table creation - new group" << hash_table_name);
                for(auto& parametry_linii : LINE_combinations)
                {
                    // time_stamp("hash table creation - new line combinations" << hash_table_name);
                    for(auto& x : X_line)
                    {
                        string str = in_order_param_concatinator(x, parametry_linii, parametry_wykresu);

                        double value = 
                                        // iterative_value_for_line_with(x, parametry_linii, parametry_wykresu);
                                        tree_value_for_line_with(tree, x, parametry_linii, parametry_wykresu);

                        if(isnan(value)) continue;
                        hash_map[str] = value;
                    }
                }
            }
            time_stamp("hash table creation - DONE " << hash_table_name);

            serialize_binary(hash_map, file_path);
        }
    }
    // HASH //
    double hash_value_for_line_with(const string& x, const vector<string>& parametry_linii, const vector<string>& parametry_wykresu)
    {
        if(hash_map.find(in_order_param_concatinator(x, parametry_linii, parametry_wykresu)) != hash_map.end())
        {
            return hash_map.at(in_order_param_concatinator(x, parametry_linii, parametry_wykresu));
        }

        return INVALID_VALUE;
    }

    // TREE //
    vector<string> sort_already_filtered_strings(const vector<string>& all_params_that_need_to_be_present)
    {
        vector<string> all(NUM_OF_CATEGORIES, "");

        for(auto& param : all_params_that_need_to_be_present)
        {
            u64 index = in_which_category_can_we_find_this_value(param);
            all[index] = param;
        }

        vector<string> ret;
        ret.reserve(all_params_that_need_to_be_present.size());

        for(auto& param : all)
        {
            if(param != "")
                ret.push_back(param);
        }

        return ret;
    }
    // TREE //
    double tree_value_for_line_with(Tree& tree, const string& x, const vector<string>& parametry_linii, const vector<string>& parametry_wykresu)
    {
        vector<string> all_params_that_need_to_be_present;
        all_params_that_need_to_be_present.push_back(x);
        all_params_that_need_to_be_present.insert(all_params_that_need_to_be_present.end(), parametry_linii.begin(), parametry_linii.end());
        all_params_that_need_to_be_present.insert(all_params_that_need_to_be_present.end(), parametry_wykresu.begin(), parametry_wykresu.end());

        vector<string> sorted_params = sort_already_filtered_strings(all_params_that_need_to_be_present);

        return tree.get_value(sorted_params);
    }
    // PARALLEL //
    double iterative_value_for_line_with(const string& x, const vector<string>& parametry_linii, const vector<string>& parametry_wykresu)
    {
        vector<string> all_params_that_need_to_be_present;
        all_params_that_need_to_be_present.push_back(x);
        all_params_that_need_to_be_present.insert(all_params_that_need_to_be_present.end(), parametry_linii.begin(), parametry_linii.end());
        all_params_that_need_to_be_present.insert(all_params_that_need_to_be_present.end(), parametry_wykresu.begin(), parametry_wykresu.end());

        double ret = INVALID_VALUE;
        bool stop = false;

        #define assing_to_ret \
        ret = \
        AVG_LINE((double)read_VALUE_avg / 1000000) \
        DEV_LINE(read_VALUE_rel_dev) \
        ;

        omp_set_num_threads(12);

        // time_stamp("iterative search " << all_lines.size());

        #pragma omp parallel for schedule(static) shared(ret, stop)
        for(u64 i = 0; i < all_lines.size(); i++)
        {
            if(stop) continue;
            auto tuple_output = Format_Buffer::input_log_line_output_variables(all_lines[i]);

            if(is_every_param_present(all_params_that_need_to_be_present, tuple_output))
            {
                #pragma omp critical
                {
                    if(!stop)
                    {
                        stop = true;

                        if(i == 0)
                        {
                            auto[adding_read] = Format_Buffer::input_log_line_output_variables(all_lines[i]);

                            assing_to_ret;
                        }
                        else
                        {
                            long long x;
                            for(x=i; x >= 0; x--)
                            {
                                tuple_output = Format_Buffer::input_log_line_output_variables(all_lines[x]);

                                if(!is_every_param_present(all_params_that_need_to_be_present, tuple_output))
                                {
                                    // x--;
                                    break;
                                }
                            }

                            auto[adding_read] = Format_Buffer::input_log_line_output_variables(all_lines[x + 1]);

                            assing_to_ret;
                        }
                    }
                }
            }
        }
        return ret;
    }

private:
    void get_all_lines()
    {
        for (const auto& log_file_path : std::filesystem::directory_iterator(input_dir))
        {
            if (!log_file_path.is_regular_file()) continue;
            string log_file = log_file_path.path().string();
            if (filesystem::path(log_file).extension() == ".tar") continue;

            ifstream is(log_file); string line;
            while(getline(is, line))
            {
                all_lines.push_back(line);
            }
        }
    }
    void filling_categories()
    {
        categories.resize(NUM_OF_CATEGORIES);

        for(auto& line : all_lines)
        {
            auto[adding_read] = Format_Buffer::input_log_line_output_variables(line);
            
            category_assignment;
        }
    }
    void recur(vector<vector<string>>& input_categories, vector<vector<string>>& output_all_comb, vector<string>& work_in_progress_comb_tab, int last_category_index)
    {
        // TIME TO SAVE
        if(last_category_index == -1)
        {
            output_all_comb.push_back(work_in_progress_comb_tab);

            work_in_progress_comb_tab.resize(input_categories.size());
            return;
        }


        for(int i=0; i < input_categories[last_category_index].size(); i++)
        {
            work_in_progress_comb_tab[last_category_index] = input_categories[last_category_index][i];

            recur(input_categories, output_all_comb, work_in_progress_comb_tab, last_category_index - 1);
        }
    }
    
    vector<string> get_one_category_from_indexes(category_index index)
    {
        return categories[index];
    }
    vector<vector<string>> get_categories_from_indexes(vector<category_index> indexes)
    {
        vector<vector<string>> ret;

        for(auto& i : indexes)
        {
            ret.push_back(categories[i]);
        }

        return ret;
    }
    bool is_every_param_present(const vector<string>& all_params_that_need_to_be_present, const tupl& params)
    {
        auto[adding_read] = params;

        int present_params_counter = 0;
        for(auto& param : all_params_that_need_to_be_present)
        {
            if(condition)
            {
                present_params_counter++;
            }
        }

        return (present_params_counter == all_params_that_need_to_be_present.size());
    }
    bool is_present_in_category(int index, string input)
    {
        return utils::contains(input, categories[index]);
    }
    void add_unique(int index, string input)
    {
        if(!is_present_in_category(index, input))
        {
            categories[index].push_back(input);
        }
    }
    vector<vector<string>> combination_only_for_selected_categories(vector<category_index> indexes)
    {
        vector<vector<string>> input_categories = get_categories_from_indexes(indexes);

        vector<vector<string>> output_all_comb;
        vector<string> work_in_progress_comb_tab;

        work_in_progress_comb_tab.resize(input_categories.size());
        recur(input_categories, output_all_comb, work_in_progress_comb_tab, input_categories.size() - 1);

        return output_all_comb;
    }

    string apply_number_separation_if_needed(const string& line)
    {
        return is_number(line) ? create_number_separation(line) : line;
    }
    bool is_number(const string& s)
    {
        return !s.empty() && find_if(s.begin(), 
            s.end(), [](unsigned char c) { return !isdigit(c); }) == s.end();
    }
    string create_number_separation(const string& line)
    {
        string local_line;

        int counter = -1;
        for (auto it = line.rbegin(); it != line.rend(); it++)
        {
            counter++;
            
            if(counter != 0 && counter % 3 == 0)
                local_line += "'";

            local_line += *it;
        }

        reverse(local_line.begin(), local_line.end());

        return local_line;
    }
    void put_into_file_one_after_another_separation_by_coma(const vector<string>& list)
    {
        for(auto& l : list)
        {
            if (&l != &list.front()) FILE << ",";
                
            FILE << apply_number_separation_if_needed(l);
        }
    }
    void show_outcome(const vector<string>& line)
    {
        for(auto& single_entry : line)
        {
            cout << single_entry << " ";
        }
        cout << endl;
    }
    void show_outcome(const vector<vector<string>>& output_all_comb)
    {
        for(auto& combination : output_all_comb)
        {
            show_outcome(combination);
        }
    }
    void show_found_categories()
    {
        cout << "FOUND categories\n";
        for(auto& category : categories)
        {
            for(auto& single_entry : category)
            {
                if (&single_entry != &category.front())
                {
                    cout << " - ";
                }
                cout << single_entry;
            }
            cout << endl;
        }
    }
    void show_all_gathered_lines()
    {
        for(auto& line : all_lines)
        {
            cout << line << endl;
        }
    }

    bool combination_possible_in_this_group(const vector<string>& X_line, const vector<vector<string>>& LINE_combinations, const vector<string>& chart_params)
    {
        for(auto& line_params : LINE_combinations)
        {
            if(combination_possible_with_at_least_one_X(X_line, line_params, chart_params))
                return true;
        }

        return false;
    }
    bool combination_possible_with_at_least_one_X(const vector<string>& X_line, const vector<string>& line_params, const vector<string>& chart_params)
    {
        int counter = 0;
        for(auto& x : X_line)
        {
            if(combination_has_value(x, line_params, chart_params))
                counter++;
        }

        return counter >= 2;
    }
    bool combination_possible_with_at_least_one_LINE(const string& x, const vector<vector<string>>& LINE_combinations, const vector<string>& chart_params)
    {
        for(auto& list_of_line_params : LINE_combinations)
        {
            if(combination_has_value(x, list_of_line_params, chart_params))
            {
                return true;
            }
        }
        return false;
    }
    bool combination_has_value(const string x, const vector<string>& line_params, const vector<string>& chart_params)
    {
        return ! isnan(hash_value_for_line_with(x, line_params, chart_params));
    }
    bool check_for_repeating_categories(const category_index& ind_X, const vector<category_index>& ind_LINEs, const vector<category_index>& ind_CHARTs)
    {
        if(utils::contains(ind_X, ind_LINEs)) return true;
        if(utils::contains(ind_X, ind_CHARTs)) return true;

        for(auto& single_line_index : ind_LINEs)
        {
            if(utils::contains(single_line_index, ind_CHARTs)) return true;
        }

        return false;
    }
    void fill_unused_indexes(const category_index& ind_X, const vector<category_index>& ind_LINEs, vector<category_index>& ind_CHARTs)
    {
        vector<bool> used_tab(NUM_OF_CATEGORIES, false);
        used_tab[ind_X] = true; for(auto& ind_line : ind_LINEs) used_tab[ind_line] = true; for(auto& ind_chart : ind_CHARTs) used_tab[ind_chart] = true;

        for(int i=0; i < used_tab.size(); i++)
        {
            if(used_tab[i] == false)
            {
                ind_CHARTs.push_back(static_cast<category_index>(i));
            }
        }
    }

public:

    All_Category_Combinations(string _input_dir, string _output_file)
    :input_dir(_input_dir), output_file(_output_file), FILE(output_file)
    {
        get_all_lines();
        filling_categories();

        show_found_categories();
        //show_all_gathered_lines();
    }
    ~All_Category_Combinations()
    {
        FILE.close();
        time_stamp("parser - starting...");

        #ifdef ORIGINAL
            #ifdef MAIN
                AVG_LINE(system("python3 src/charter.py demo_charts_main_avg_og Main_avg_og.csv");)
                DEV_LINE(system("python3 src/charter.py demo_charts_main_dev_og Main_dev_og.csv");)
            #endif
            #ifdef SINGLE
                AVG_LINE(system("python3 src/charter.py demo_charts_single_avg_og SingleOp_avg_og.csv");)
                DEV_LINE(system("python3 src/charter.py demo_charts_single_dev_og SingleOp_dev_og.csv");)
            #endif
        #endif

         #ifdef FILTERED
            #ifdef MAIN
                AVG_LINE(system("python3 src/charter.py demo_charts_main_avg_filt Main_avg_filt.csv");)
                DEV_LINE(system("python3 src/charter.py demo_charts_main_dev_filt Main_dev_filt.csv");)
            #endif
            #ifdef SINGLE
                AVG_LINE(system("python3 src/charter.py demo_charts_single_avg_filt SingleOp_avg_filt.csv");)
                DEV_LINE(system("python3 src/charter.py demo_charts_single_dev_filt SingleOp_dev_filt.csv");)
            #endif
        #endif

        time_stamp("parser - DONE");
    }

    void start(category_index ind_X, vector<category_index> ind_LINEs, vector<category_index> ind_CHARTs, bool fill_unused_indexes_param = false, vector<category_index> ind_TO_SKIP = {})
    {
        if(check_for_repeating_categories(ind_X, ind_LINEs, ind_CHARTs))
        {
            cout << "\n\nERROR - SAME indexes are used in the configuration - " << __LINE__ << endl;
            exit(0);
        }
        if(fill_unused_indexes_param) { fill_unused_indexes(ind_X, ind_LINEs, ind_CHARTs); }
        if(ind_TO_SKIP.size() != 0)
        {
            for(auto& ind_to_skip : ind_TO_SKIP)
            {
                if(ind_to_skip == ind_X)
                {
                    cout << "\n\nERROR - unable to skip category that's set to X - " << __LINE__ << endl;
                    exit(0);
                }

                utils::remove_by_value(ind_to_skip, ind_LINEs);
                utils::remove_by_value(ind_to_skip, ind_CHARTs);
            }
        }

        vector<string> X_line = get_one_category_from_indexes(ind_X);
        vector<vector<string>> LINE_combinations = combination_only_for_selected_categories(ind_LINEs);
        vector<vector<string>> GROUP_CHART_combinations = combination_only_for_selected_categories(ind_CHARTs);

        string hash_table_name = create_index_combination_to_string(ind_X, ind_LINEs, ind_CHARTs);

        // HASH //
        create_hash_table_for_this_set_of_params(hash_table_name, X_line, LINE_combinations, GROUP_CHART_combinations, ind_TO_SKIP);

        for(auto& list_of_chart_params : GROUP_CHART_combinations)
        {
            if(!combination_possible_in_this_group(X_line, LINE_combinations, list_of_chart_params)) { continue; }
            // if(utils::contains<string>("measuring_r", list_of_chart_params)) { continue; } // tak, żeby zrobiły tylko dla BEST

            // po kolei wyświetlam każdy parametr CHART
            FILE << "GROUP;";
            put_into_file_one_after_another_separation_by_coma(list_of_chart_params);
            FILE << "\n";

            // po kolei wyświetlam każdy parametr LINE
            FILE << ";"; // NIC
            for(auto& list_of_line_params : LINE_combinations)
            {
                // pomija niemożliwe konfiguracje -> wyświetlanie
                if(!combination_possible_with_at_least_one_X(X_line, list_of_line_params, list_of_chart_params)) { continue; }

                put_into_file_one_after_another_separation_by_coma(list_of_line_params);
                FILE << ";";
            }
            FILE << "\n";


            // writing VALUES //

            for(auto& x : X_line)
            {
                if(combination_possible_with_at_least_one_LINE(x, LINE_combinations, list_of_chart_params))
                {
                    FILE << apply_number_separation_if_needed(x) << ";";
                }
                else continue;

                for(auto& list_of_line_params : LINE_combinations)
                {
                    // pomija niemożliwe konfiguracje -> wartość
                    if(!combination_has_value(x, list_of_line_params, list_of_chart_params))
                    {
                        if(combination_possible_with_at_least_one_X(X_line, list_of_line_params, list_of_chart_params)) FILE << "0;";
                        continue;
                    }

                    FILE << setprecision(2) << hash_value_for_line_with(x, list_of_line_params, list_of_chart_params) << ";";
                }
                FILE << "\n";
            }
            FILE << "\n\n";
        }
    }
};

/////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
    // true -> dodaje nieprzypisane parametry do CHART
    // {} -> te parametry pomijamy

    time_stamp("Main starting..")

    #define PACKED


    #ifdef MAIN

        #ifdef PACKED
            All_Category_Combinations comb(
            ORIGINAL_LINE("input/original/Ray_Tracer")
            FILTERED_LINE("input/filtered/Ray_Tracer")
            ,
            string(
            AVG_LINE("output/Main_avg")
            DEV_LINE("output/Main_dev")
            )
            +
            string(
            ORIGINAL_LINE("_og")
            FILTERED_LINE("_filt")
            )
            +
            string(".csv")
            );
        #else
            All_Category_Combinations comb("../Ray_Tracer/output", "output/Main.csv");
        #endif
        
        // { PROCESSOR, MODEL, ARCH, UNIT, PHYSICAL_CORES, LOGICAL_CORES, CORE_PROPORTION, SCENE, LIGHTS, SPHERES, BOUNCES, NUM_OF_THREADS, RATING };

        // BEST vs FIRST //
        comb.start(SCENE, {MODEL, ARCH, UNIT}, {RATING}, true, {LIGHTS, SPHERES, BOUNCES, PHYSICAL_CORES, LOGICAL_CORES, CORE_PROPORTION, NUM_OF_THREADS}); // przez rating, są unikatowe
        comb.start(SCENE, {MODEL, ARCH}, {UNIT, RATING}, true, {LIGHTS, SPHERES, BOUNCES, PHYSICAL_CORES, LOGICAL_CORES, CORE_PROPORTION, NUM_OF_THREADS});
        
        // WYKRESY PER THREAD //
        comb.start(NUM_OF_THREADS, {SCENE, UNIT, ARCH}, {MODEL, RATING}, true, {LIGHTS, SPHERES, BOUNCES, PHYSICAL_CORES, LOGICAL_CORES, CORE_PROPORTION}); // tylko tu jest konflikt

        // Z POMIJANIEM //
        comb.start(SCENE, {UNIT}, {RATING}, true, {LIGHTS, SPHERES, BOUNCES, PHYSICAL_CORES, LOGICAL_CORES, CORE_PROPORTION, NUM_OF_THREADS});

        // każdy model po koleji
        comb.start(MODEL, {SCENE}, {PROCESSOR, UNIT, RATING}, true, {LIGHTS, SPHERES, BOUNCES, PHYSICAL_CORES, LOGICAL_CORES, CORE_PROPORTION, NUM_OF_THREADS});
        comb.start(MODEL, {UNIT}, {SCENE, RATING}, true, {LIGHTS, SPHERES, BOUNCES, PHYSICAL_CORES, LOGICAL_CORES, CORE_PROPORTION, NUM_OF_THREADS});
    #endif

    #ifdef SINGLE

        #ifdef PACKED
            All_Category_Combinations comb(
            ORIGINAL_LINE("input/original/Single_Operations")
            FILTERED_LINE("input/filtered/Single_Operations")
            ,
            string(
            AVG_LINE("output/SingleOp_avg")
            DEV_LINE("output/SingleOp_dev")
            )
            +
            string(
            ORIGINAL_LINE("_og")
            FILTERED_LINE("_filt")
            )
            +
            string(".csv")
            );
        #else
            All_Category_Combinations comb("../Single_Operations/output", "output/SingleOp.csv");
        #endif

        // { PROCESSOR, MODEL, ARCH, UNIT, PHYSICAL_CORES, LOGICAL_CORES, CORE_PROPORTION, TASK, SIZE, NUM_OF_THREADS, RATING };

        comb.start(SIZE, {MODEL, ARCH, UNIT}, {TASK, RATING}, true, {NUM_OF_THREADS, PHYSICAL_CORES, LOGICAL_CORES, CORE_PROPORTION});

        comb.start(TASK, {SIZE, UNIT}, {RATING, MODEL, ARCH}, true, {NUM_OF_THREADS, PHYSICAL_CORES, LOGICAL_CORES, CORE_PROPORTION});
        comb.start(TASK, {SIZE, MODEL, ARCH}, {RATING, UNIT}, true, {NUM_OF_THREADS, PHYSICAL_CORES, LOGICAL_CORES, CORE_PROPORTION});


        // bez SIZE -> to za dużo grup // !!!!!!!!!!!!!!!

        // comb.start(NUM_OF_THREADS, {TASK, MODEL, ARCH}, {PROCESSOR}, true, {RATING, PHYSICAL_CORES, LOGICAL_CORES, CORE_PROPORTION});

        // comb.start(NUM_OF_THREADS, {UNIT, MODEL, ARCH, PROCESSOR}, {TASK}, true, {RATING, PHYSICAL_CORES, LOGICAL_CORES, CORE_PROPORTION});
        // comb.start(NUM_OF_THREADS, {MODEL, ARCH}, {TASK, PROCESSOR, UNIT}, true, {RATING, PHYSICAL_CORES, LOGICAL_CORES, CORE_PROPORTION});

        // // każdy model po koleji
        // comb.start(MODEL, {TASK}, {PROCESSOR, UNIT, RATING}, true, {PHYSICAL_CORES, LOGICAL_CORES, CORE_PROPORTION, NUM_OF_THREADS});
        // comb.start(MODEL, {UNIT}, {TASK, RATING}, true, {PHYSICAL_CORES, LOGICAL_CORES, CORE_PROPORTION, NUM_OF_THREADS});

    #endif

    time_stamp("Main END")
}

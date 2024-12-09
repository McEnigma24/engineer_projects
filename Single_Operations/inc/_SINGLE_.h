#pragma once

#include "_PARSER_Format_Buffer_Common_.h"

#define NUM_OF_CATEGORIES 11
//              1    2    3    4
//        PROCESSOR MODEL ARCH UNIT
//                                                                              8   9                          10
//                                                                            TASK SIZE                  NUM_OF_THREADS
//                                                5    6    7                                                            11
//                                    PHYSICAL_CORES LOGICAL_CORES CORE_PROPORTION                                     RATING
#define format "%s - %s - %s - %s -> Core Details %s - %s - %s -> Task Details %s - %s -> Execution Details th %s / 18 - %s - <%llu, %llu, %llu> Odchylenie standardowe: %lf",

enum category_index { PROCESSOR, MODEL, ARCH, UNIT, PHYSICAL_CORES, LOGICAL_CORES, CORE_PROPORTION, TASK, SIZE, NUM_OF_THREADS, RATING };



#define GET(t, index) get<index>(t)

#define tupl tuple<double, u64, u64, u64, \
string, \
string, \
string, \
string, \
string, \
string, \
string, \
string, \
string, \
string, \
string \
>

#define add_to_all(a, b) \
\
add(a, processor, b), \
add(a, model, b), \
add(a, arch, b), \
add(a, unit, b), \
add(a, physical_cores, b), \
add(a, logical_cores, b), \
add(a, core_proportion, b), \
add(a, task, b), \
add(a, size, b), \
add(a, num_of_threads, b), \
add(a, rating, b) 

#define adding_buffer_init \
\
add_array(processor); \
add_array(model); \
add_array(arch); \
add_array(unit); \
add_array(physical_cores); \
add_array(logical_cores); \
add_array(core_proportion); \
add_array(task); \
add_array(size); \
add_array(num_of_threads); \
add_array(rating);

#define category_assignment \
add_unique(PROCESSOR, add(read_, processor,)); \
add_unique(MODEL, add(read_, model,)); \
add_unique(ARCH, add(read_, arch,)); \
add_unique(UNIT, add(read_, unit,)); \
add_unique(PHYSICAL_CORES, add(read_, physical_cores,)); \
add_unique(LOGICAL_CORES, add(read_, logical_cores,)); \
add_unique(CORE_PROPORTION, add(read_, core_proportion,)); \
add_unique(TASK, add(read_, task,)); \
add_unique(SIZE, add(read_, size,)); \
add_unique(NUM_OF_THREADS, add(read_, num_of_threads,)); \
add_unique(RATING, add(read_, rating,));

#define input_tupl_variables \
get<4>(variables).c_str(), \
get<5>(variables).c_str(), \
get<6>(variables).c_str(), \
get<7>(variables).c_str(), \
get<8>(variables).c_str(), \
get<9>(variables).c_str(), \
get<10>(variables).c_str(), \
get<11>(variables).c_str(), \
get<12>(variables).c_str(), \
get<13>(variables).c_str(), \
get<14>(variables).c_str()

#define condition \
(param == add(read_, processor,)) || \
(param == add(read_, model,)) || \
(param == add(read_, arch,))  || \
(param == add(read_, unit,))  || \
(param == add(read_, physical_cores,))  || \
(param == add(read_, logical_cores,))  || \
(param == add(read_, core_proportion,))  || \
(param == add(read_, task,))  || \
(param == add(read_, size,))  || \
(param == add(read_, num_of_threads,)) || \
(param == add(read_, rating,))

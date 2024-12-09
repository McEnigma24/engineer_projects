#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <tuple>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <omp.h>

template <typename T>
bool contains(const T& value, const std::vector<T>& vec)
{
    return std::find(vec.begin(), vec.end(), value) != vec.end();
}

template <typename T>
void remove_by_value(const T& value, std::vector<T>& vec)
{
    auto it = std::find(vec.begin(), vec.end(), value);

    if (it != vec.end())
    {
        vec.erase(it);
    }
}

using namespace std;

#define var(x) cout << #x << " = " << x << '\n';
#define varr(x) cout << #x << " = " << x << ' ';
#define line(x) cout << x << '\n';
#define linee(x) cout << x << ' ';
#define nline cout << '\n';

#define string_line(s) cout << s << " size: " << s.size() << endl;


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
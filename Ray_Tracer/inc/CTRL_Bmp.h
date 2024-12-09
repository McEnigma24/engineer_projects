#pragma once
#include "_preprocessor_.h"
#include "RT_RGB.h"

struct BMP_static
{
    static void save(const string& file_name, const vector<RGB>& my_pixel);
};
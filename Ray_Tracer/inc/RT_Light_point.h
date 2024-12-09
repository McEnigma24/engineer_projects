#pragma once
#include "_preprocessor_.h"
#include "RT_Dimensions.h"
#include "RT_RGB.h"
#include "RT_RGB_float.h"


struct Light_point
{
	d3 pos;
	RGB_float light_color;

public:
	Light_point(d3 p, RGB col)
		:pos(p), light_color(col) {}
};
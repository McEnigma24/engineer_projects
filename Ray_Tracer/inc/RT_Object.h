#pragma once
#include "_preprocessor_.h"
#include "RT_RGB_float.h"


struct Object
{
	unit transparent_param;
	unit reflective_param;
	bool ligth_source;
	RGB_float color;
	Surface_type surface_type;

public:
	Object(unit transparent, unit reflective, bool light, RGB_float col, Surface_type s_type)
		:transparent_param(transparent), reflective_param(reflective), ligth_source(light), color(col), surface_type(s_type) {}
};
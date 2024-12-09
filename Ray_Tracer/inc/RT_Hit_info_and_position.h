#pragma once
#include "_preprocessor_.h"
#include "RT_Dimensions.h"


struct Hit_info_and_position
{
	bool intersecting_with_object;
	d3 intersection_pos;

	Hit_info_and_position(bool b = false, const d3& int_pos = -1)
		:intersecting_with_object(b), intersection_pos(int_pos)
	{}
};

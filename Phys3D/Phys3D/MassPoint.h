#pragma once
#include "../hoLib/num/vector.h"

namespace ho
{
	namespace num
	{
		class vec3d;
	}
}

using namespace ho::num;

namespace Phys3D
{
	struct MASSPOINT //質点
	{
		MASSPOINT(const vec3d &vecPos, const double &Mass)
			: vecPos(vecPos), Mass(Mass)
		{}

		vec3d vecPos; //位置
		double Mass; //質量
	};
}

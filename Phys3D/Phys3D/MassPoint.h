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
	struct MASSPOINT //Ž¿“_
	{
		MASSPOINT(const vec3d &vecPos, const double &Mass)
			: vecPos(vecPos), Mass(Mass)
		{}

		vec3d vecPos; //ˆÊ’u
		double Mass; //Ž¿—Ê
	};
}

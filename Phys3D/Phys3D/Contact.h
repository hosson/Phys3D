//接触情報構造体
#pragma once
#include <memory>
#include "../hoLib/num/vector.h"
#include <vector>

using namespace ho::num;

namespace Phys3D
{
	class Body;
	class Solver;

	struct PAIR
	{
		PAIR() {}
		PAIR(const std::weak_ptr<Body> &wpBodyObj1, const std::weak_ptr<Body> &wpBodyObj2)
		{
			this->wpBodyObj[0] = wpBodyObj1;
			this->wpBodyObj[1] = wpBodyObj2;
		}
		std::weak_ptr<Body> wpBodyObj[2];

		bool operator ==(const PAIR &obj) const { return this->wpBodyObj[0].lock() == obj.wpBodyObj[0].lock() && this->wpBodyObj[1].lock() == obj.wpBodyObj[1].lock(); }
	};

	struct CONTACT
	{
		CONTACT(const PAIR &Pair)
			: Pair(Pair)
		{}

		PAIR Pair;
		vec3d vecWorldPos; //接触した位置（ワールド座標）
		vec3d vecN; //接触時の法線ベクトル（物体1の内側へ向く方向）
		double SinkDistance; //めり込んでいる距離
		std::vector<std::shared_ptr<Solver>> vectorspSolverObj; //各ソルバーへのポインタ
	};
}
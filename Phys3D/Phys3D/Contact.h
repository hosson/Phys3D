//�ڐG���\����
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
		vec3d vecWorldPos; //�ڐG�����ʒu�i���[���h���W�j
		vec3d vecN; //�ڐG���̖@���x�N�g���i����1�̓����֌��������j
		double SinkDistance; //�߂荞��ł��鋗��
		std::vector<std::shared_ptr<Solver>> vectorspSolverObj; //�e�\���o�[�ւ̃|�C���^
	};
}
//�ڐG���N���X
#pragma once
#include "../hoLib/num/vector.h"
#include <vector>
#include <memory>
#include "Contact.h"
using namespace ho::num;

namespace Phys3D
{
	class Body;
	class Collision;

	class Solver
	{
	public:
		Solver(const CONTACT &Contact, const std::weak_ptr<Collision> &wpCollisionObj); //�R���X�g���N�^
		~Solver(); //�f�X�g���N�^

		void Continue(const CONTACT &Contact); //�ڐG�p���̂��߁A�ڐG�����X�V

		void PreSolve(); //�S�������̎��O����
		void SolveVelocity(); //���x�����ɂ��čS�����������s
		void SolvePosition(); //�ʒu�����ɂ��čS�����������s

		//�A�N�Z�b�T
		const CONTACT &getContact() const { return Contact; }
	private:
		const std::weak_ptr<Collision> wpCollisionObj;
		CONTACT Contact; //�ڐG���
		double TargetRelVelNormal; //�@�������̖ڕW���Α��x
		double TotalForceNormal; //�@�������̗͂̍��v
		double TotalForceTangent; //�ڐ������̗͂̍��v
		double constant_a_normal; //�@�������̒萔a
		double e; //�����W��
		double mu; //���C�W��

#ifdef _DEBUG
		std::vector<double> vectorTotalForceNormal; //�@�������̗͂̔�������
#endif
		double Get_constant_a(const vec3d &vecN); //�萔a���v�Z
	};
}

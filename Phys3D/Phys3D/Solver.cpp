#include "Solver.h"
#include "Body.h"
#include "Collision.h"
#include "../hoLib/num/numerical.hpp"
using namespace ho::num;

namespace Phys3D
{
	//�R���X�g���N�^
	Solver::Solver(const CONTACT &Contact, const std::weak_ptr<Collision> &wpCollisionObj)
		: wpCollisionObj(wpCollisionObj), Contact(Contact), TotalForceNormal(0), TotalForceTangent(0)
	{
		std::shared_ptr<Body> spBodyObj[2] = { Contact.Pair.wpBodyObj[0].lock(), Contact.Pair.wpBodyObj[1].lock() };

		this->e = sqrt(spBodyObj[0]->gete() * spBodyObj[1]->gete()); //2���̊Ԃ̔����W�����v�Z
		this->mu = sqrt(spBodyObj[0]->getmu() * spBodyObj[1]->getmu()); //2���̊Ԃ̖��C�W�����v�Z

		vec3d vecRelVel = Body::GetRelativeVelocity(Contact.Pair.wpBodyObj[0], Contact.Pair.wpBodyObj[1], Contact.vecWorldPos); //�ڐG�ʒu�ɂ����镨�̊Ԃ̑��Α��x
		double RelVelNormal = vec3d::GetDot(vecRelVel, Contact.vecN); //���Α��x�̖@��������������ςɂ��v�Z

		//�@�������̖ڕW���Α��x��ݒ�
		if (RelVelNormal < -0.5) //0.5[m/s]�ȏ�̑����ŋ߂Â��Ă���ꍇ
			this->TargetRelVelNormal = -RelVelNormal * this->e; //�Փˌ�̖ڕW���Α��x�͏ՓˑO�̋߂Â����x�ɔ����W�����|�����������������̂ƂȂ�
		else
			this->TargetRelVelNormal = 0;
	}

	//�f�X�g���N�^
	Solver::~Solver()
	{}

	//�ڐG�p���̂��߁A�ڐG�����X�V
	void Solver::Continue(const CONTACT &Contact)
	{
		this->Contact = Contact;

		vec3d vecRelVel = Body::GetRelativeVelocity(Contact.Pair.wpBodyObj[0], Contact.Pair.wpBodyObj[1], Contact.vecWorldPos); //�ڐG�ʒu�ɂ����镨�̊Ԃ̑��Α��x
		double RelVelNormal = vec3d::GetDot(vecRelVel, Contact.vecN); //���Α��x�̖@��������������ςɂ��v�Z

		//�@�������̖ڕW���Α��x��ݒ�
		if (RelVelNormal < -0.5) //0.5[m/s]�ȏ�̑����ŋ߂Â��Ă���ꍇ
			this->TargetRelVelNormal = -RelVelNormal * this->e; //�Փˌ�̖ڕW���Α��x�͏ՓˑO�̋߂Â����x�ɔ����W�����|�����������������̂ƂȂ�
		else
			this->TargetRelVelNormal = 0;

		return;
	}

	//�S�������̎��O����
	void Solver::PreSolve()
	{
		const std::shared_ptr<Body> spBodyObj1 = Contact.Pair.wpBodyObj[0].lock();
		const std::shared_ptr<Body> spBodyObj2 = Contact.Pair.wpBodyObj[1].lock();

		this->constant_a_normal = Get_constant_a(Contact.vecN); //�@�������̒萔a���v�Z

		//���̂ɑO��Ō�ɋ��܂����͐ς�������
		//this->TotalForceNormal = 0;
		spBodyObj1->ApplyForce(this->Contact.vecN * this->TotalForceNormal, this->Contact.vecWorldPos - spBodyObj1->getvecPos()); //�@�������̗͂�����1�ɉ�����
		spBodyObj2->ApplyForce(-this->Contact.vecN * this->TotalForceNormal, this->Contact.vecWorldPos - spBodyObj2->getvecPos()); //����2�ɂ͔���p�̗͂�������

		//���C�͂�0�ɂ���
		this->TotalForceTangent = 0;

		return;
	}

	//���x�����ɂ��čS�����������s
	void Solver::SolveVelocity()
	{
		const std::shared_ptr<Body> spBodyObj1 = Contact.Pair.wpBodyObj[0].lock();
		const std::shared_ptr<Body> spBodyObj2 = Contact.Pair.wpBodyObj[1].lock();

		vec3d vecRelVel = Body::GetRelativeVelocity(Contact.Pair.wpBodyObj[0], Contact.Pair.wpBodyObj[1], Contact.vecWorldPos); //�ڐG�ʒu�ɂ����镨�̊Ԃ̑��Α��x
		double RelVelNormal = vec3d::GetDot(vecRelVel, Contact.vecN); //���Α��x�̖@��������������ςɂ��v�Z

		const double SOR_omega = 1.0; //SOR�@�ɂ������W����

		//�@������
		{
			//�@�������̗͐ς��v�Z
			double ForceNormal = (this->TargetRelVelNormal - RelVelNormal) * this->constant_a_normal; //((���z���Α��xN - ���݂̑��Α��xN) * �K������N)��͐�N�Ƃ���
			ForceNormal *= SOR_omega; //SOR�@�ɂ�����
			ForceNormal = ho::num::Max(this->TotalForceNormal + ForceNormal, 0.0) - this->TotalForceNormal;
			this->TotalForceNormal += ForceNormal;
#ifdef _DEBUG
			this->vectorTotalForceNormal.push_back(this->TotalForceNormal);
#endif

			//���̂ɗ͐ς�������
			spBodyObj1->ApplyForce(Contact.vecN * ForceNormal, Contact.vecWorldPos - spBodyObj1->getvecPos()); //�@�������̗͂�����1�ɉ�����
			spBodyObj2->ApplyForce(-Contact.vecN * ForceNormal, Contact.vecWorldPos - spBodyObj2->getvecPos()); //����2�ɂ͔���p�̗͂�������
		}

		//�ڐ�����
		{
			vec3d vecRelVelTanjent = vecRelVel - Contact.vecN * RelVelNormal; //���Α��x�̐ڐ������������A[���Α��x-�@����������]�Ōv�Z
			double constant_a_tanjent = Get_constant_a(vecRelVelTanjent.GetNormalize()); //�ڐ������̒萔a���v�Z

			//(-���݂̑��Α��xT * �K������T)�Ŋ��S�ɂ����ł������͐�T
			//���������C�͂ɂ͏��������̂�(�͐�N * ���C�W��)�ŃN�����v
			double ForceTanjent = -vecRelVelTanjent.GetPower() * constant_a_tanjent;
			ForceTanjent = ho::num::Clamp(this->TotalForceTangent + ForceTanjent, -this->TotalForceNormal * mu, this->TotalForceNormal * mu) - this->TotalForceTangent;
			this->TotalForceTangent += ForceTanjent;

			//���̂ɖ��C�ɂ��͐ς�������
			spBodyObj1->ApplyForce(vecRelVelTanjent.GetNormalize() * ForceTanjent, Contact.vecWorldPos - spBodyObj1->getvecPos()); //�ڐ������̗͂�����1�ɉ�����
			spBodyObj2->ApplyForce(-vecRelVelTanjent.GetNormalize() * ForceTanjent, Contact.vecWorldPos - spBodyObj2->getvecPos()); //����2�ɂ͔���p�̗͂�������
		}
		
		return;
	}

	//�ʒu�����ɂ��čS�����������s
	void Solver::SolvePosition()
	{
		const std::shared_ptr<Body> spBodyObj1 = Contact.Pair.wpBodyObj[0].lock();
		const std::shared_ptr<Body> spBodyObj2 = Contact.Pair.wpBodyObj[1].lock();

		const double constant_a = this->Get_constant_a(Contact.vecN); //�萔a���v�Z

		//�߂荞�ݏC���ɕK�v�ȗ͂��v�Z
		double Force = (Contact.SinkDistance - 0.0001) * constant_a * 0.4; //�Ō�̌W����0.5�ȉ��ɂ���
		if (Force < 0)
			Force = 0;

		//���̂Ɉʒu�C���̗͂�������
		spBodyObj1->ApplyCorrect(Contact.vecN * Force, Contact.vecWorldPos - spBodyObj1->getvecPos()); //�@�������̗͂�����1�ɉ�����
		spBodyObj2->ApplyCorrect(Contact.vecN * -Force, Contact.vecWorldPos - spBodyObj2->getvecPos()); //�@�������̗͂�����2�ɉ�����

		this->wpCollisionObj.lock()->CollisionJudge(this->Contact); //�ʒu�C�������̂ŐڐG�����X�V

		return;
	}

	//�萔a���v�Z
	double Solver::Get_constant_a(const vec3d &vecN) 
	{
		const std::shared_ptr<Body> spBodyObj1 = Contact.Pair.wpBodyObj[0].lock();
		const std::shared_ptr<Body> spBodyObj2 = Contact.Pair.wpBodyObj[1].lock();
		
		//����1��2���猩�����Έʒu���v�Z
		const vec3d vecRelativePos1 = Contact.vecWorldPos - spBodyObj1->getvecPos();
		const vec3d vecRelativePos2 = Contact.vecWorldPos - spBodyObj2->getvecPos();

		// 1/... �ɂȂ��Ă���̂͋t�������Ƃɖ߂����߁B�t�����g���Ɩ�����̎��ʂ��\���\�B
		return 1.0 / (spBodyObj1->getMassInv() + spBodyObj2->getMassInv() +
			//�������[�����g�E���Έʒu�E�@�����畨�̂̊p���x�ɗ^����e�����v�Z
			vec3d::GetDot(vecN, vec3d::GetCross(spBodyObj1->getmtrxInertiaNowInv() * vec3d::GetCross(vecRelativePos1, vecN), vecRelativePos1)) +
			vec3d::GetDot(vecN, vec3d::GetCross(spBodyObj2->getmtrxInertiaNowInv() * vec3d::GetCross(vecRelativePos2, vecN), vecRelativePos2))
			);
	}
}
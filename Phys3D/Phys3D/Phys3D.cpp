#include "Phys3D.h"
#include "World.h"
#include "BodyCreator.h"
#include "../hoLib/num/vector.h"
#include "Collision.h"

using namespace ho::num;

namespace Phys3D
{
	//�R���X�g���N�^
	Phys3D::Phys3D(const double &dt, const int SolveNum)
	: dt(dt), SolveNum(SolveNum)
	{
		this->spBodyCreatorObj = std::shared_ptr<BodyCreator>(new BodyCreator()); //���̐����I�u�W�F�N�g�𐶐�
		this->spCollisionObj = std::make_shared<Collision>(); //�Փˌ��o�I�u�W�F�N�g�𐶐�
		this->spWorldObj = ho::sp_init<World>(new World(this->sp_this, dt, vec3d(0, -9.8, 0))); //���E�N���X�I�u�W�F�N�g���쐬
		this->WarmStart = true;
	}

	//�f�X�g���N�^
	Phys3D::~Phys3D() 
	{}

	//1�t���[�����̏���
	void Phys3D::Update()
	{
		this->spWorldObj->Update(); //���E�N���X�I�u�W�F�N�g��1�t���[�����̏���

		return;
	}

	//���̂𐶐�����
	std::shared_ptr<Body> Phys3D::CreateBody(const vec3d &vecPos, const vec3d &vecLinearVelocity, const Quaternion &qtAngle, const vec3d &vecL, const std::vector<MASSPOINT> &vectorMassPoint, const std::vector<vec3d> &vectorvecVertex, const std::vector<int> &vecVtIndex, const double &e, const double &mu, const bool &Fixed)
	{
		std::shared_ptr<Body> spBodyObj = this->spBodyCreatorObj->CreateBody(vecPos, vecLinearVelocity, qtAngle, vecL, vectorMassPoint, vectorvecVertex, vecVtIndex, e, mu, Fixed);
		this->spWorldObj->AddspBodyObj(spBodyObj);

		return spBodyObj;
	}

	//���̂𐶐�����
	std::shared_ptr<Body> Phys3D::CreateBody(const vec3d &vecPos, const vec3d &vecLinearVelocity, const Quaternion &qtAngle, const vec3d &vecL, const double &Density, const std::vector<vec3d> &vectorvecVertex, const std::vector<int> &vecVtIndex, const double &e, const double &mu, const bool &Fixed)
	{
		std::shared_ptr<Body> spBodyObj = this->spBodyCreatorObj->CreateBody(vecPos, vecLinearVelocity, qtAngle, vecL, Density, vectorvecVertex, vecVtIndex, e, mu, Fixed);
		this->spWorldObj->AddspBodyObj(spBodyObj);

		return spBodyObj;
	}
}

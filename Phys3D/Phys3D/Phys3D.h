//�������Z�G���W�� Phys3D �I�u�W�F�N�g
#pragma once
#include "../hoLib/num/vector.h"
#include "../hoLib/num/Quaternion.h"
#include <memory>
#include "MassPoint.h"
#include "../hoLib/sp_init.hpp"

using namespace ho::num;

namespace Phys3D
{
	class World;
	class BodyCreator;
	class Body;
	class Collision;

	class Phys3D : public ho::sp_base<Phys3D>
	{
	public:
		Phys3D(const double &dt = 1.0 / 60.0, const int SolveNum = 10); //�R���X�g���N�^
		~Phys3D(); //�f�X�g���N�^

		void Update(); //1�t���[�����̏���
		std::shared_ptr<Body> CreateBody(const vec3d &vecPos, const vec3d &vecLinearVelocity, const Quaternion &qtAngle, const vec3d &vecL, const std::vector<MASSPOINT> &vectorMassPoint, const std::vector<vec3d> &vectorvecVertex, const std::vector<int> &vecVtIndex, const double &e, const double &mu, const bool &Fixed); //���̂𐶐�����
		std::shared_ptr<Body> CreateBody(const vec3d &vecPos, const vec3d &vecLinearVelocity, const Quaternion &qtAngle, const vec3d &vecL, const double &Density, const std::vector<vec3d> &vectorvecVertex, const std::vector<int> &vecVtIndex, const double &e, const double &mu, const bool &Fixed); //���̂𐶐�����

		//�A�N�Z�b�T
		const std::shared_ptr<Collision> &getspCollisionObj() const { return spCollisionObj; }
		const int getSolveNum() const { return SolveNum; }
	private:
		double dt; //���ݎ���t [s] �i60FPS�Ȃ�1/60������j
		int SolveNum; //�S���������s����
		bool WarmStart; //WarmStart���g�����ǂ���
		std::shared_ptr<BodyCreator> spBodyCreatorObj; //���̐����I�u�W�F�N�g
		std::shared_ptr<Collision> spCollisionObj; //�Փˌ��o�I�u�W�F�N�g
		std::shared_ptr<World> spWorldObj; //���E�I�u�W�F�N�g
	};
}

/*�������Z�G���W���𗘗p����v���O����������́A���̃N���X�̃I�u�W�F�N�g���ŏ���
1������������B�g��Ȃ��Ȃ����I�����ɂ͔j������B

���E�ɍ��̂�ǉ�����ɂ́APhys3D::CreateBody() ���Ăяo���B
Phys3D::CreateBody() �̖߂�l�̃X�}�[�g�|�C���^���G���W���O���ŕێ�����B
�G���W���O���ŃX�}�[�g�|�C���^����������ƁA�G���W�������ł������I�ɕ��̂��폜�����B
*/
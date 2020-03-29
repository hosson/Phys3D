//���̐����N���X
#pragma once
#include <memory>
#include "../hoLib/num/vector.h"
#include "../hoLib/num/Quaternion.h"
#include "../hoLib/num/Matrix.h"
#include <vector>
#include "MassPoint.h"

using namespace ho::num;

namespace Phys3D
{
	class Body;

	class BodyCreator
	{
	public:
		BodyCreator() {} //�R���X�g���N�^
		~BodyCreator() {} //�f�X�g���N�^

		std::shared_ptr<Body> CreateBody(vec3d vecPos, const vec3d &vecLinearVelocity, const Quaternion &qtAngle, const vec3d &vecOmega, std::vector<MASSPOINT> vectorMassPoint, std::vector<vec3d> vectorvecVertex, const std::vector<int> &vectorVtFaceIndex, const double &e, const double &mu, const bool &Fixed); //���̂𐶐�����
		std::shared_ptr<Body> CreateBody(vec3d vecPos, const vec3d &vecLinearVelocity, const Quaternion &qtAngle, const vec3d &vecOmega, const double &Density, std::vector<vec3d> vectorvecVertex, const std::vector<int> &vectorVtFaceIndex, const double &e, const double &mu, const bool &Fixed); //���̂𐶐�����
	private:
		Matrix GetmtrxInertia(const std::vector<MASSPOINT> &vectorMassPoint, const bool &Fixed); //���_�f�[�^���犵�����[�����g�e���\�����v�Z����
		Matrix GetmtrxInertia(const double &Density, const std::vector<vec3d> &vectorvecVertex, const std::vector<int> &vectorVtFaceIndex, const bool &Fixed); //���x�ƃ��b�V���`�󂩂犵�����[�����g�e���\�����v�Z����
		std::vector<int> GetvectorVtEdgeIndex(const std::vector<int> &vectorVtFaceIndex); //�ʂ̒��_�C���f�b�N�X����ӂ̒��_�C���f�b�N�X�𒊏o
	};
}

/*Body �I�u�W�F�N�g�𐶐����邽�߂̃N���X�B
Body �I�u�W�F�N�g�������̃p�����[�^�ɂ͗l�X�ȃp�^�[�������邽�߁A
���̍����z�����ABody �N���X�̃R���X�g���N�^��1�ɂ܂Ƃ߂����������B
*/
//���̃N���X
#pragma once
#include <list>
#include <vector>
#include "../hoLib/num/vector.h"
#include "../hoLib/num/Quaternion.h"
#include "../hoLib/num/Matrix.h"
#include <memory>

using namespace ho::num;

namespace Phys3D
{
	class World;
	class Mesh;
	class Contact;

	class Body
	{
	public:
		Body(const vec3d &vecPos, const vec3d &vecLinearVelocity, const Quaternion &qtAngle, const vec3d &vecL, const double &Mass, const Matrix &mtrxInertia, const std::shared_ptr<Mesh> &spMeshObj, const double &e, const double &mu, const bool &Fixed); //�R���X�g���N�^
		~Body(); //�f�X�g���N�^

		Matrix GetCoordTransMatrix() const; //�O���t�B�b�N�X�p�̍��W�ϊ��s����擾����

		//void GetAllvecVtWorld(std::vector<vec3d> *pvectorvecVtWorld); //���ׂĂ̒��_���W�����[���h���W�Ŏ擾
		void Move(const double &dt); //�ړ�
		void ApplyGravity(const vec3d &vecGravity); //�d�͂�t������
		void ApplyForce(const vec3d &vecForce, const vec3d &vecLocalPos); //�͐ς�������
		void ApplyCorrect(const vec3d &vecForce, const vec3d &vecLocalPos); //�ʒu�𓮂����͂�������
		void GetlistVtFaceiiFromVtEdgeii(std::list<int> *plistVtFaceii, int VtEdgeii); //�����ӂ��܂܂��ʂ����ׂė񋓂���i�ӂ̒��_�C���f�b�N�X�z��̃C���f�b�N�X�̕ӂɂ��܂܂��ʂ̒��_�C���f�b�N�X�z�񒆂̃C���f�b�N�X��񋓁j
		//void GetvectorvecFaceNRotation(std::vector<vec3d> *pvectorvecFaceN); //���ׂĂ̖ʂ̌��݂̎p���ł̖@���x�N�g�����擾����
		void CalcmtrxInertiaNowInv(); //���ݎ����i���ݎp���j�ł̊����e���\���̋t�s����v�Z����
		void UpdateMeshVertexWorldPos(); //���b�V���̃��[���h���W���X�V����
		void AddpContactObj(Contact *pContactObj); //�ڐG�I�u�W�F�N�g��ǉ�
		bool ErasepContactObj(Contact *pContactObj); //�ڐG�I�u�W�F�N�g���폜

		static vec3d GetRelativeVelocity(const std::weak_ptr<Body> &wpBodyObj1, const std::weak_ptr<Body> &wpBodyObj2, const vec3d &vecWorldPos); //���[���h���W��̈�_�ł̍��̊Ԃ̑��Α��x���v�Z

		//�A�N�Z�b�T
		void setwpWorldObj(const std::weak_ptr<World> &wp) { this->wpWorldObj = wp; }
		const vec3d &getvecPos() const { return vecPos; }
		const vec3d &getvecLinearVelocity() const { return vecLinearVelocity; }
		const Quaternion &getqtAngle() const { return qtAngle; }
		const vec3d &getvecL() const { return vecL; }
		const vec3d &getvecOmega() const { return vecOmega; }
		const double &getMass() const { return Mass; }
		const double &getMassInv() const { return MassInv; }
		const Matrix &getmtrxInertia() const { return mtrxInertia; }
		const Matrix &getmtrxInertiaInv() const { return mtrxInertiaInv; }
		const Matrix &getmtrxInertiaNowInv() const { return mtrxInertiaNowInv; }
		const double &getBoundingRadius() const { return BoundingRadius; }
		const bool &getFixed() const { return Fixed; }
		double CalcInverseInertia(vec3d *pvecAxis);
		std::vector<vec3d> *GetpvectorvecVt() { return &vectorvecVt; }
		std::vector<int> *GetpvectorVtFaceIndex() { return &vectorVtFaceIndex; }
		std::vector<int> *GetpvectorVtEdgeIndex() { return &vectorVtEdgeIndex; }
		int GetFaces() { return Faces; }
		const std::shared_ptr<Mesh> &getspMeshObj() const { return spMeshObj; }
		const double &gete() const { return  e; }
		const double &getmu() const { return mu; }
		//bool GetSleep() { return Sleep; }
	private:
		std::weak_ptr<World> wpWorldObj;
		vec3d vecPos; //���[���h���W��̈ʒu
		vec3d vecLinearVelocity; //���s�ړ����x
		Quaternion qtAngle; //��]�ʒu
		vec3d vecL; //�p�^����
		vec3d vecOmega; //�p���x
		double Mass; //�d��[Kg]
		double MassInv; //�d�ʂ̋t��
		Matrix mtrxInertia; //�������[�����g�e���\��
		Matrix mtrxInertiaInv; //�������[�����g�e���\���̋t�s��
		Matrix mtrxInertiaNowInv; //���ݎ����i���ݎp���j�ł̊����e���\���̋t�s��
		std::shared_ptr<Mesh> spMeshObj; //���b�V���I�u�W�F�N�g
		std::vector<vec3d> vectorvecVt; //���_���W�z��
		std::vector<int> vectorVtFaceIndex; //�ʂ̒��_�C���f�b�N�X�z��
		std::vector<int> vectorVtEdgeIndex; //�ӂ̒��_�C���f�b�N�X�z��
		int Faces; //�ʂ̐�
		//std::vector<vec3d> vectorvecFaceN; //�e�ʂ̖@���x�N�g���̔z��
		double e; //�����W��
		double mu; //���C�W��
		bool Fixed; //�Œ蕨���ǂ���
		double BoundingRadius; //�Փ˔���p�̍Œ����a
		std::list<Contact *> listpContactObj; //�ւ���Ă���ڐG�I�u�W�F�N�g�̃��X�g
		int LastUpdateFrame; //���b�V���̃��[���h���_���W���Ō�ɍX�V�����t���[����
	};

}
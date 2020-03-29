//�Փ˔���N���X
#pragma once
#include <list>
#include <vector>
#include "../hoLib/num/vector.h"
#include "Collision_base.h"
#include "Contact.h"

using namespace ho::num;

namespace Phys3D
{

	class Mesh;
	class Face;
	class Edge;
	class Vertex;
	class Body;

	class Collision
	{
	public:
		struct CrossPoint //�ʂƕӂƂ̌�_���
		{
			vec3d vecPos; //�������W
			Face *pFaceObj;
			Edge *pEdgeObj;
			Vertex *pVertexObj;
		};
		struct TriangleCollision //3�p�`�̖ʂǂ����̏Փ˔���̌���
		{
			CrossPoint CrossPt[2]; //��_���
			Face *pFaceObj[2]; //�Փ˔��肳�ꂽ2�̖�
		};
		struct CommonPoint //�ڐG��_�ƒ��_�I�u�W�F�N�g�̋��ʂ̓_���
		{
			Vertex *pVertexObj; //���_�I�u�W�F�N�g
			CrossPoint CrossPoint; //�ڐG��_
		};
	public:
		Collision(); //�R���X�g���N�^
		~Collision(); //�f�X�g���N�^

		void DetectContact(std::list<CONTACT> &listContact, const std::list<std::shared_ptr<Body>> &listspBodyObj); //�S�Ă̕��̂���ڐG�����擾
		bool CollisionJudge(CONTACT &Contact); //���̓��m�̏Փ˔���
	private:
		void GetPair(std::list<PAIR> &listPair, const std::list<std::shared_ptr<Body>> &listspBodyObj); //�ڐG�\���̂���y�A���擾����
		bool GetSinkMesh(std::vector<std::shared_ptr<Mesh>> &vectorspMeshObj, const std::weak_ptr<Mesh> &wpMeshObj1, const std::weak_ptr<Mesh> &wpMeshObj2); //2�̃��b�V�����Փ˔��肵�Փ˕����̃��b�V���𓾂�
		void GetSinkMeshFace(const std::shared_ptr<Mesh> &spMeshObj, const std::shared_ptr<Face> &spFaceObj, std::list<vec3d> &listvecCrossPoint); //�Փː����ŕ������A�߂荞�ݕ����̃��b�V�����\������ʂ𓾂�
		void CreateOutLine(std::list<vec3d> &listvecCrossPoint, const vec3d &vecFaceN); //���_���X�g����O���̕ӂ��\�����钸�_�������c���A�c����폜����
		void DividePolygonIntoTriangles(const std::shared_ptr<Mesh> &spMeshObj, const std::list<vec3d> &listvecCrossPointj); //���p�`�𕡐��̎O�p�`�̖ʂɕ���
		vec3d CalcvecNFromSinkMesh(std::vector<std::shared_ptr<Mesh>> &vectorspMeshObj); //�ڐG���b�V�����@���x�N�g�����v�Z����
		vec3d CalcHitPos(std::vector<std::shared_ptr<Mesh>> &vectorspMeshObj); //�ڐG���b�V�����ڐG�ʒu���v�Z����
		double CalcSinkDistance(const std::vector<std::shared_ptr<Mesh>> &vectorspMeshObj, const vec3d &vecHitPos, const vec3d &vecN); //�ڐG���b�V���ƏՓˈʒu�ƏՓ˖@�����A�߂荞�݋������v�Z����
		void GetSubPos(std::vector<vec3d> &vectorvecSubPos, const std::vector<std::shared_ptr<Mesh>> &vectorspMeshObj, const vec3d &vecHitPos, const vec3d &vecM); //�T�u�\���o�[�p�̐ڐG�ʒu���擾
	};
}

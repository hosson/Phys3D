#pragma once
#include <list>
#include <vector>
#include "../hoLib/num/vector.h"
#include "../hoLib/num/Matrix.h"
#include "../hoLib/num/Quaternion.h"
#include <memory>
#include <boost/optional.hpp>
#include "../hoLib/sp_init.hpp"

namespace Phys3D
{
	using namespace ::ho::num;

	class Vertex;
	class Edge;
	class Face;

	//���b�V���N���X
	class Mesh : public ho::sp_base<Mesh>
	{
	public:
		Mesh(); //�R���X�g���N�^
		Mesh(const std::vector<vec3d> &vectorvecVt, const std::vector<int> &vectorVtEdgeIndex, const std::vector<int> &vectorVtFaceIndex); //�R���X�g���N�^
		~Mesh(); //�f�X�g���N�^

		void UpdateVertexWorldPos(const Quaternion &qt, const vec3d &vecWorldPos); //�S�Ă̒��_�̃��[���h���W�����݂̈ʒu�Ǝp���̂��̂ɍX�V
		double GetBoundingRadius(); //���E�����a���擾

		void AddFace(const std::vector<vec3d> &vectorvecVt); //3�̒��_��񂩂�A���_�A�ӁA�ʂ�ǉ�����

		//�A�N�Z�b�T
		const std::vector<std::shared_ptr<Vertex>> &getvectorspVertexObj() const { return vectorspVertexObj; }
		const std::vector<std::shared_ptr<Edge>> &getvectorspEdgeObj() const { return vectorspEdgeObj; }
		const std::vector<std::shared_ptr<Face>> &getvectorspFaceObj() const { return vectorspFaceObj; }
	private:
		std::weak_ptr<Mesh> wpThisObj;
		std::vector<std::shared_ptr<Vertex>> vectorspVertexObj;
		std::vector<std::shared_ptr<Edge>> vectorspEdgeObj;
		std::vector<std::shared_ptr<Face>> vectorspFaceObj;

		void Merge(); //���_�ƕӂƖʂ̋��ʊ֌W���`��
	};

	//���_�N���X
	class Vertex : public ho::sp_base<Vertex>
	{
	public:
		Vertex(const vec3d &vecLocalPos, const int &ID); //�R���X�g���N�^
		Vertex(const Vertex &obj); //�R�s�[�R���X�g���N�^
		~Vertex(); //�f�X�g���N�^

		bool AddwpEdgeObj(const std::weak_ptr<Edge> &wpEdgeObj, const boost::optional<vec3d> &vecN = boost::optional<vec3d>(boost::none)); //�ӃI�u�W�F�N�g��ǉ�
		void AddwpFaceObj(const std::weak_ptr<Face> &wpFaceObj); //�ʃI�u�W�F�N�g��ǉ�
		void UpdatevecPos(const Quaternion &qt, const vec3d &vecWorldPos); //���W�����[���h���W�ɕϊ����A���[�J�����W��ޔ�̈�ɃR�s�[
		void RestoreLocalPos(); //���W�����[�J�����W�ɖ߂�
		void ErasewpEdgeObj(const std::weak_ptr<Edge> &wpEdgeObj); //�ӃI�u�W�F�N�g���폜
		std::weak_ptr<Edge> GetwpCounterClockwiseEdgeObj(const std::weak_ptr<Edge> &wpEdgeObj); //�ڑ����ꂽ�C�ӂ̕ӂ̔����v�������ׂ̗ɐڑ�����Ă���ӂ��擾

		static void Merge(const std::weak_ptr<Vertex> &wpAbsorbObj, const std::weak_ptr<Vertex> &wpEraseObj); //2�̒��_��1�ɋz������BwpEraseObj���������̓��e��wpAbsorbObj�ɒǉ������B

		//�A�N�Z�b�T
		const vec3d &getvecLocalPos() const { return vecLocalPos; }
		const vec3d &getvecWorldPos() const { return vecWorldPos; }
		const int &getID() const { return ID; }
		std::vector<std::weak_ptr<Edge>> &getvectorwpEdgeObj() { return vectorwpEdgeObj; }
		std::vector<std::weak_ptr<Face>> &getvectorwpFaceObj() { return vectorwpFaceObj; }
	private:
		std::weak_ptr<Vertex> wpThisObj;
		vec3d vecLocalPos; //���[�J�����W
		vec3d vecWorldPos; //���[���h���W
		int ID;
		std::vector<std::weak_ptr<Face>> vectorwpFaceObj;
		std::vector<std::weak_ptr<Edge>> vectorwpEdgeObj;
	};

	//�ӃN���X
	class Edge : public ho::sp_base<Edge>
	{
	public:
		Edge(const std::weak_ptr<Vertex> &wpVertexObj1, const std::weak_ptr<Vertex> &wpVertexObj2, const int &ID, const boost::optional<vec3d> &vecN = boost::optional<vec3d>(boost::none)); //�R���X�g���N�^
		//Edge(const Edge &obj); //�R�s�[�R���X�g���N�^
		~Edge(); //�f�X�g���N�^

		void Init(); //������

		void AddwpFaceObj(const std::weak_ptr<Face> &wpFaceObj); //�ʂ�ǉ�����
		void ExchangewpVertexObj(const std::weak_ptr<Vertex> &wpAddVertexObj, const std::weak_ptr<Vertex> &wpEraseVertexObj); //�ڑ���̒��_�I�u�W�F�N�g�����ւ���
		std::weak_ptr<Vertex> GetwpOppositeVertexObj(const std::weak_ptr<Vertex> &wpVertexObj); //��������ɐڑ����ꂽ���_���擾

		//�A�N�Z�b�T
		const std::weak_ptr<Vertex> &getwpVertexObj1() const { return wpVertexObj[0]; }
		const std::weak_ptr<Vertex> &getwpVertexObj2() const { return wpVertexObj[1]; }
		const std::vector<std::weak_ptr<Face>> &getvectorwpFaceObj() const { return vectorwpFaceObj; }
		const int &getID() const { return ID; }
		void setID(const int &id) { this->ID = id; }
	private:
		std::weak_ptr<Edge> wpThisObj;
		std::weak_ptr<Vertex> wpVertexObj[2];
		std::vector<std::weak_ptr<Face>> vectorwpFaceObj;
		int ID;
	};

	//�ʃN���X
	class Face : public ho::sp_base<Face>
	{
	public:
		Face(); //�R���X�g���N�^
		Face(const std::weak_ptr<Mesh> &wpMeshObj, const std::weak_ptr<Vertex> &wpVertexObj1, const std::weak_ptr<Vertex> &wpVertexObj2, const std::weak_ptr<Vertex> &wpVertexObj3, const int &ID); //�R���X�g���N�^
		//Face(const std::list<std::weak_ptr<Vertex>> &listwpVertexObj, const std::list<std::weak_ptr<Edge>> &listwpEdgeObj, const int &ID); //�R���X�g���N�^�i���ɍ쐬���ꂽ���_�ƕӂ���쐬�j
		//Face(const std::vector<std::weak_ptr<Vertex>> &vectorwpVertexObj, const std::vector<std::weak_ptr<Edge>> &vectorwpEdgeObj, const int &ID); //�R���X�g���N�^�i���ɍ쐬���ꂽ���_�ƕӂ̔z�񂩂�O�p�`���쐬�j
		//Face(const Face &obj); //�R�s�[�R���X�g���N�^
		~Face(); //�f�X�g���N�^

		void Init(const std::weak_ptr<Face> &wpFaceObj); //������

		void AddwpEdgeObj(const std::weak_ptr<Edge> &wpEdgeObj); //�ӂ�ǉ�����
		void AddwpVertexObj(const std::weak_ptr<Vertex> &wpVertexObj); //���_��ǉ�����
		void ErasewpVertexObj(const std::weak_ptr<Vertex> &wpVertexObj); //�o�^����Ă��钸�_���폜
		bool SearchVertices(const std::weak_ptr<Vertex> &wpVertexObj1, const std::weak_ptr<Vertex> &wpVertexObj2); //2�̒��_�I�u�W�F�N�g�������Ă��邩�ǂ����𔻒�
		//std::shared_ptr<Face> Duplicate(); //��������i���_��ӂ����Ȃ����j
		void ErasewpEdgeObj(const std::weak_ptr<Edge> &wpEdgeObj); //�o�^����Ă���ӂ��폜
		double CalcArea(); //�ʐς��v�Z����
		double CalcVolume(const vec3d &vecCenter); //���钆�S���W�Ƃ�4�_�̑̐ς����߂�
		vec3d CalcCentroid(const vec3d &vecCenter); //���钆�S���W�Ƃ�4�_�̕��ρi�d�S�j�����߂�
		bool CalcDistance(double &OutDistance, const vec3d &vecPos, const vec3d &vecDirection); //�C�ӂ̍��W����C�ӂ̕����֌����������Ƃ̌�_�����߁A���܂����ꍇ�͔C�ӂ̍��W����̋�����Ԃ�
		std::weak_ptr<Edge> GetwpEdgeObj(const int VertexIndex1, const int VertexIndex2); //2�̒��_�̃C���f�b�N�X����ӃI�u�W�F�N�g�𓾂�
		void DeleteVertexAndEdge(); //�������钸�_�ƕӂ̃I�u�W�F�N�g���폜
		void CalcvecLocalNormal(); //���[�J�����W�̖@���x�N�g�����v�Z����
		void CalcvecWorldNormal(); //���[���h���W�̖@���x�N�g�����v�Z����

		//�A�N�Z�b�T
		const std::weak_ptr<Mesh> &getwpMeshObj() const { return wpMeshObj; }
		std::vector<std::weak_ptr<Vertex>> &getvectorwpVertexObj() { return vectorwpVertexObj; }
		std::vector<std::weak_ptr<Edge>> &getvectorwpEdgeObj() { return vectorwpEdgeObj; }
		const int &getID() const { return ID; }
		const vec3d &getvecLocalNormal() const { return vecLocalNormal; }
		const vec3d &getvecWorldNormal() const { return vecWorldNormal; }
	private:
		std::weak_ptr<Mesh> wpMeshObj;
		std::vector<std::weak_ptr<Vertex>> vectorwpVertexObj;
		std::vector<std::weak_ptr<Edge>> vectorwpEdgeObj;
		int ID; //���b�V�����猩���ʂ��ԍ�
		vec3d vecLocalNormal, vecWorldNormal; //�@���x�N�g��
	};
}
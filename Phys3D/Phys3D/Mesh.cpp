#include "Mesh.h"
#include "../hoLib/Common.h"
#include "../hoLib/Error.h"
#include "../hoLib/num/constant.h"
using namespace ho::num;

namespace Phys3D
{
	//�R���X�g���N�^
	Mesh::Mesh()
	{
		this->wpThisObj = this->sp_this;
	}

	//�R���X�g���N�^
	Mesh::Mesh(const std::vector<vec3d> &vectorvecVt, const std::vector<int> &vectorVtEdgeIndex, const std::vector<int> &vectorVtFaceIndex)
	{
		this->wpThisObj = this->sp_this;

		//���_�I�u�W�F�N�g���쐬
		for (unsigned int i = 0; i < vectorvecVt.size(); i++)
			this->vectorspVertexObj.push_back(ho::sp_init<Vertex>(new Vertex(vectorvecVt.at(i), i)));

		//�ӃI�u�W�F�N�g���쐬
		for (unsigned int i = 0; i < vectorVtEdgeIndex.size(); i += 2)
			this->vectorspEdgeObj.push_back(ho::sp_init<Edge>(new Edge(this->vectorspVertexObj.at(vectorVtEdgeIndex.at(i)), vectorspVertexObj.at(vectorVtEdgeIndex.at(i + 1)), i / 2)));

		//�ʃI�u�W�F�N�g���쐬
		for (unsigned int i = 0; i < vectorVtFaceIndex.size(); i += 3)
			this->vectorspFaceObj.push_back(ho::sp_init<Face>(new Face(this->wpThisObj, this->vectorspVertexObj.at(vectorVtFaceIndex.at(i)), vectorspVertexObj.at(vectorVtFaceIndex.at(i + 1)), vectorspVertexObj.at(vectorVtFaceIndex.at(i + 2)), i / 3)));

		Merge(); //���_�ƕӂƖʂ̋��ʊ֌W���`��
	}

	//�f�X�g���N�^
	Mesh::~Mesh()
	{
	}

	//���_�ƕӂƖʂ̋��ʊ֌W���`��
	void Mesh::Merge()
	{
		//�ӂƖʂ̋��ʊ֌W���`��
		for (unsigned int i = 0; i < this->vectorspEdgeObj.size(); i++) //�ӃI�u�W�F�N�g�𑖍�
		{
			for (unsigned int j = 0; j < this->vectorspFaceObj.size(); j++) //�ʃI�u�W�F�N�g�𑖍�
			{
				if (this->vectorspFaceObj.at(j)->SearchVertices(this->vectorspEdgeObj.at(i)->getwpVertexObj1(), this->vectorspEdgeObj.at(i)->getwpVertexObj2())) //�ӂƖʂŋ��ʂ̒��_�������Ă��邩�𒲂ׁA�����Ă���ꍇ
				{
					this->vectorspEdgeObj.at(i)->AddwpFaceObj(this->vectorspFaceObj.at(j)); //�ӂɖʂ�ǉ�
					this->vectorspFaceObj.at(j)->AddwpEdgeObj(this->vectorspEdgeObj.at(i)); //�ʂɕӂ�ǉ�
				}
			}
		}

		return;
	}

	//�S�Ă̒��_�̃��[���h���W�����݂̈ʒu�Ǝp���̂��̂ɍX�V
	void Mesh::UpdateVertexWorldPos(const Quaternion &qt, const vec3d &vecWorldPos)
	{
		for (unsigned int i = 0; i < this->vectorspVertexObj.size(); i++)
			this->vectorspVertexObj.at(i)->UpdatevecPos(qt, vecWorldPos);
		for (std::vector<std::shared_ptr<Face>>::iterator itr = this->vectorspFaceObj.begin(); itr != this->vectorspFaceObj.end(); itr++)
			(*itr)->CalcvecWorldNormal(); //���[���h���W�n�̖@���x�N�g�����X�V

		return;
	}

	//���E�����a���擾
	double Mesh::GetBoundingRadius()
	{
		double MaxRadius = 0.0;
		for (unsigned int i = 0; i < this->vectorspVertexObj.size(); i++)
			if (vectorspVertexObj.at(i)->getvecLocalPos().GetPower() > MaxRadius)
				MaxRadius = vectorspVertexObj.at(i)->getvecLocalPos().GetPower();

		return MaxRadius;
	}

	//3�̒��_��񂩂�A���_�A�ӁA�ʂ�ǉ�����
	void Mesh::AddFace(const std::vector<vec3d> &vectorvecVt)
	{
		//���_�I�u�W�F�N�g���쐬
		const int Vertices = this->vectorspVertexObj.size();
		for (unsigned int i = 0; i < 3; i++)
			this->vectorspVertexObj.push_back(ho::sp_init<Vertex>(new Vertex(vectorvecVt.at(i), Vertices + i)));

		//�ӃI�u�W�F�N�g���쐬
		const int Edges = this->vectorspEdgeObj.size();
		for (unsigned int i = 0; i < 3; i++)
			this->vectorspEdgeObj.push_back(ho::sp_init<Edge>(new Edge(this->vectorspVertexObj.at(i), this->vectorspVertexObj.at((i + 1) % 3), Edges + i)));

		//�ʃI�u�W�F�N�g���쐬
		const int Faces = this->vectorspFaceObj.size();
		this->vectorspFaceObj.push_back(ho::sp_init<Face>(new Face(this->wpThisObj, this->vectorspVertexObj.at(Vertices), this->vectorspVertexObj.at(Vertices + 1), this->vectorspVertexObj.at(Vertices + 2), Faces)));

		return;
	}




	//�R���X�g���N�^
	Vertex::Vertex(const vec3d &vecLocalPos, const int &ID)
		: vecLocalPos(vecLocalPos), ID(ID)
	{
		this->wpThisObj = this->sp_this;
	}

	//�R�s�[�R���X�g���N�^
	Vertex::Vertex(const Vertex &obj)
	{
		this->vecLocalPos = obj.vecLocalPos;
		this->vecWorldPos = obj.vecWorldPos;
		this->ID = obj.ID;
	}

	//�f�X�g���N�^
	Vertex::~Vertex()
	{
	}

	//�ӃI�u�W�F�N�g��ǉ�
	bool Vertex::AddwpEdgeObj(const std::weak_ptr<Edge> &wpEdgeObj, const boost::optional<vec3d> &vecN)
	{
		if (this->vectorwpEdgeObj.size() < 2) //�ӃI�u�W�F�N�g��2��菭�Ȃ��ꍇ
		{
			this->vectorwpEdgeObj.push_back(wpEdgeObj); //�ǉ�
		} else {  //�ӃI�u�W�F�N�g��2�ȏ�̏ꍇ
			if (vecN) //�p�x�\�[�g�̊�ƂȂ�ʂ̖@���x�N�g�����w�肳��Ă���ꍇ
			{
				vec3d vec[2];
				std::vector<double> vectorAngle(this->vectorwpEdgeObj.size()); //�����̕ӂƂ̊p�x
				vec3d vecCross;

				//�e�ӂƒǉ����ꂽ�ӂƂ̎��v���̊p�x���v�Z
				int i = 0;
				for (std::vector<std::weak_ptr<Edge>>::iterator itr = this->vectorwpEdgeObj.begin(); itr != this->vectorwpEdgeObj.end(); itr++) //�����̕ӂ𑖍�
				{
					if (auto p = itr->lock())
					{
						auto spEdgeObj = wpEdgeObj.lock();

						if (auto p2 = p->GetwpOppositeVertexObj(this->wpThisObj).lock())
							vec[0] = p2->getvecLocalPos() - this->vecLocalPos; //���̒��_���瑖�����̕ӂ̂�������̒��_�ւ̃x�N�g��
						if (auto p2 = spEdgeObj->GetwpOppositeVertexObj(this->wpThisObj).lock())
							vec[1] = p2->getvecLocalPos() - this->vecLocalPos; //���̒��_����ǉ������ӂ̂�������̒��_�ւ̃x�N�g��
						double Angle;
						vec3d::GetAngle(Angle, vec[0], vec[1]); //�x�N�g���Ԃ̊p�x���v�Z
						vecCross = vec3d::GetCross(vec[0], vec[1]); //�O�ς��v�Z
						if (vec3d::GetDot(vecN.get(), vecCross) < 0) //�O�ς̌��ʂ����̐��̏ꍇ
							Angle = PI * 2.0 - Angle; //�p�x�͔����v���ŋ��܂��Ă���̂ŁA���v���̊p�x�ɕϊ�����
						vectorAngle.at(i++) = Angle;
					}
				}

				//��Ԋp�x�̑傫�����́i�ǉ������ӂ̈���v���̕����ׂ̗̂��́j��T��
				double MaxAngle = 0;
				unsigned int MaxIndex = -1;
				for (unsigned int i = 0; i < vectorAngle.size(); i++)
				{
					if (MaxAngle < vectorAngle.at(i))
					{
						MaxAngle = vectorAngle.at(i);
						MaxIndex = i;
					}
				}

				if (MaxIndex == -1) //�C���f�b�N�X�������������Ȃ������ꍇ
				{
					ERR(true, TEXT("�C���f�b�N�X�����������܂�܂���ł����B"), __WFILE__, __LINE__);
					return false;
				}

				//��Ԋp�x�̑傫���ӂ̎�O�ɑ}��
				std::vector<std::weak_ptr<Edge>>::iterator itr = this->vectorwpEdgeObj.begin();
				itr += MaxIndex;
				this->vectorwpEdgeObj.insert(itr, wpEdgeObj);
			}
		}

		return true;
	}

	//�ʃI�u�W�F�N�g��ǉ�
	void Vertex::AddwpFaceObj(const std::weak_ptr<Face> &wpFaceObj)
	{
		this->vectorwpFaceObj.push_back(wpFaceObj);

		return;
	}

	//���W�����[���h���W�ɕϊ����A���[�J�����W��ޔ�̈�ɃR�s�[
	void Vertex::UpdatevecPos(const Quaternion &qt, const vec3d &vecWorldPos)
	{
		qt.TransformVector(this->vecWorldPos, this->vecLocalPos); //��]
		this->vecWorldPos += vecWorldPos; //���s�ړ�

		return;
	}

	//���W�����[�J�����W�ɖ߂�
	void Vertex::RestoreLocalPos()
	{
		//vecPos = vecBuffer;

		return;
	}

	//�ӃI�u�W�F�N�g���폜
	void Vertex::ErasewpEdgeObj(const std::weak_ptr<Edge> &wpEdgeObj)
	{
		for (std::vector<std::weak_ptr<Edge>>::iterator itr = this->vectorwpEdgeObj.begin(); itr != this->vectorwpEdgeObj.end();)
		{
			if ((*itr).lock() == wpEdgeObj.lock())
				itr = this->vectorwpEdgeObj.erase(itr);
			else
				itr++;
		}

		return;
	}

	//�ڑ����ꂽ�C�ӂ̕ӂ̔����v�������ׂ̗ɐڑ�����Ă���ӂ��擾
	std::weak_ptr<Edge> Vertex::GetwpCounterClockwiseEdgeObj(const std::weak_ptr<Edge> &wpEdgeObj)
	{
		for (std::vector<std::weak_ptr<Edge>>::iterator itr = this->vectorwpEdgeObj.begin(); itr != this->vectorwpEdgeObj.end(); itr++)
		{
			if (itr->lock() == wpEdgeObj.lock())
			{
				if (itr == this->vectorwpEdgeObj.begin())
				{
					itr = this->vectorwpEdgeObj.end();
					itr--;
					return *itr;
				} else {
					itr--;
					return *itr;
				}
			}
		}

		return std::weak_ptr<Edge>();
	}

	//2�̒��_��1�ɋz������BwpEraseObj���������̓��e��wpAbsorbObj�ɒǉ������B
	void Vertex::Merge(const std::weak_ptr<Vertex> &wpAbsorbObj, const std::weak_ptr<Vertex> &wpEraseObj)
	{
		bool Add;

		const std::shared_ptr<Vertex> spAbsorbObj = wpAbsorbObj.lock();
		const std::shared_ptr<Vertex> spEraseObj = wpEraseObj.lock();

		//�ӃI�u�W�F�N�g�̋z��
		for (std::vector<std::weak_ptr<Edge>>::const_iterator itr1 = spEraseObj->getvectorwpEdgeObj().begin(); itr1 != spEraseObj->getvectorwpEdgeObj().end(); itr1++) //��������钸�_���ɐڑ�����Ă���ӃI�u�W�F�N�g�𑖍�
		{
			Add = true; //�ǉ����邩�ǂ���
			for (std::vector<std::weak_ptr<Edge>>::const_iterator itr2 = spAbsorbObj->getvectorwpEdgeObj().begin(); itr2 != spAbsorbObj->getvectorwpEdgeObj().end(); itr2++) //�z������钸�_���ɐڑ�����Ă���ӃI�u�W�F�N�g�𑖍�
			{
				if (itr1->lock() == itr2->lock()) //�z������鑤�̒��_�ɂ��łɐڑ�����Ă���ꍇ
				{
					Add = false;
					break;
				}
			}
			if (Add) //�z������ꍇ
			{
				if (auto p = itr1->lock())
				{
					p->ExchangewpVertexObj(wpAbsorbObj, wpEraseObj); //�ӂ̐ڑ���̒��_�����ւ���
					spAbsorbObj->getvectorwpEdgeObj().push_back(p); //�z������钸�_���ɂ��ӂ�ǉ�
				}
			}
		}

		//�ʃI�u�W�F�N�g�̋z��
		for (std::vector<std::weak_ptr<Face>>::const_iterator itr1 = spEraseObj->getvectorwpFaceObj().begin(); itr1 != spEraseObj->getvectorwpFaceObj().end(); itr1++) //��������钸�_���ɐڑ�����Ă���ӃI�u�W�F�N�g�𑖍�
		{
			Add = true; //�ǉ����邩�ǂ���
			for (std::vector<std::weak_ptr<Face>>::const_iterator itr2 = spAbsorbObj->getvectorwpFaceObj().begin(); itr2 != spAbsorbObj->getvectorwpFaceObj().end(); itr2++) //�z������钸�_���ɐڑ�����Ă���ӃI�u�W�F�N�g�𑖍�
			{
				if (itr1->lock() == itr2->lock()) //�z������鑤�̒��_�ɂ��łɐڑ�����Ă���ꍇ
				{
					Add = false;
					break;
				}
			}
			if (Add) //�z������ꍇ
			{
				if (auto p = itr1->lock())
				{
					p->ErasewpVertexObj(wpEraseObj); //�ʂ���폜����钸�_���폜
					p->AddwpVertexObj(wpAbsorbObj); //�ʂ֒ǉ�����钸�_��ǉ�
					spAbsorbObj->getvectorwpFaceObj().push_back(p); //�z������钸�_���ɂ��ӂ�ǉ�
				}
			}
		}

		return;
	}














	//�R���X�g���N�^
	Edge::Edge(const std::weak_ptr<Vertex> &wpVertexObj1, const std::weak_ptr<Vertex> &wpVertexObj2, const int &ID, const boost::optional<vec3d> &vecN)
	{
		this->wpThisObj = this->sp_this;
		this->ID = ID;

		this->wpVertexObj[0] = wpVertexObj1;
		this->wpVertexObj[1] = wpVertexObj2;

		const std::shared_ptr<Vertex> spVertexObj1 = wpVertexObj1.lock();
		const std::shared_ptr<Vertex> spVertexObj2 = wpVertexObj2.lock();

		spVertexObj1->AddwpEdgeObj(wpThisObj, vecN);
		spVertexObj2->AddwpEdgeObj(wpThisObj, vecN);

	}

	/*
	//�R�s�[�R���X�g���N�^
	Edge::Edge(const Edge &obj)
	{
	}
	*/

	//�f�X�g���N�^
	Edge::~Edge()
	{
	}

	//�ʂ�ǉ�����
	void Edge::AddwpFaceObj(const std::weak_ptr<Face> &wpFaceObj)
	{
		this->vectorwpFaceObj.push_back(wpFaceObj);

		return;
	}

	//�ڑ���̒��_�I�u�W�F�N�g�����ւ���
	void Edge::ExchangewpVertexObj(const std::weak_ptr<Vertex> &wpAddVertexObj, const std::weak_ptr<Vertex> &wpEraseVertexObj)
	{
		for (int i = 0; i < 2; i++)
			if (this->wpVertexObj[i].lock() == wpEraseVertexObj.lock())
				this->wpVertexObj[i] = wpAddVertexObj;

		return;
	}

	//��������ɐڑ����ꂽ���_���擾
	std::weak_ptr<Vertex> Edge::GetwpOppositeVertexObj(const std::weak_ptr<Vertex> &wpVertexObj)
	{
		if (this->wpVertexObj[0].lock() == wpVertexObj.lock())
			return this->wpVertexObj[1];
		else if (this->wpVertexObj[1].lock() == wpVertexObj.lock())
			return this->wpVertexObj[0];

		return std::weak_ptr<Vertex>();
	}












	//�R���X�g���N�^
	Face::Face()
	{
		this->ID = 0;
	}

	//�R���X�g���N�^
	Face::Face(const std::weak_ptr<Mesh> &wpMeshObj, const std::weak_ptr<Vertex> &wpVertexObj1, const std::weak_ptr<Vertex> &wpVertexObj2, const std::weak_ptr<Vertex> &wpVertexObj3, const int &ID)
	{
		this->wpMeshObj = wpMeshObj;
		this->vectorwpVertexObj.push_back(wpVertexObj1);
		this->vectorwpVertexObj.push_back(wpVertexObj2);
		this->vectorwpVertexObj.push_back(wpVertexObj3);
		this->ID = ID;

		wpVertexObj1.lock()->AddwpFaceObj(this->sp_this);
		wpVertexObj2.lock()->AddwpFaceObj(this->sp_this);
		wpVertexObj3.lock()->AddwpFaceObj(this->sp_this);

		this->CalcvecLocalNormal();
	}

	/*
	//�R���X�g���N�^�i���ɍ쐬���ꂽ���_�ƕӂ���쐬�j
	Face::Face(const std::list<std::weak_ptr<Vertex>> &listwpVertexObj, const std::list<std::weak_ptr<Edge>> &listwpEdgeObj, const int &ID)
	{
		this->vectorwpVertexObj.resize(listwpVertexObj.size());
		int i = 0;
		for (std::list<std::weak_ptr<Vertex>>::const_iterator itr = listwpVertexObj.begin(); itr != listwpVertexObj.end(); itr++)
			this->vectorwpVertexObj.at(i++) = *itr;

		this->vectorwpEdgeObj.resize(listwpEdgeObj.size());
		i = 0;
		for (std::list<std::weak_ptr<Edge>>::const_iterator itr = listwpEdgeObj.begin(); itr != listwpEdgeObj.end(); itr++)
			this->vectorwpEdgeObj.at(i++) = *itr;

		this->ID = ID;
	}

	//�R���X�g���N�^�i���ɍ쐬���ꂽ���_�ƕӂ̔z�񂩂�O�p�`���쐬�j
	Face::Face(const std::vector<std::weak_ptr<Vertex>> &vectorwpVertexObj, const std::vector<std::weak_ptr<Edge>> &vectorwpEdgeObj, const int &ID)
	{
		this->vectorwpVertexObj.resize(3);
		this->vectorwpEdgeObj.resize(3);
		for (int i = 0; i < 3; i++)
		{
			this->vectorwpVertexObj.at(i) = vectorwpVertexObj.at(i);
			this->vectorwpEdgeObj.at(i) = vectorwpEdgeObj.at(i);
		}

		this->ID = ID;
	}
	*/

	/*
	//�R�s�[�R���X�g���N�^
	Face::Face(const Face &obj)
	{
	}
	*/

	//�f�X�g���N�^
	Face::~Face()
	{
	}

	//�ӂ�ǉ�����
	void Face::AddwpEdgeObj(const std::weak_ptr<Edge> &wpEdgeObj)
	{
		this->vectorwpEdgeObj.push_back(wpEdgeObj);

		return;
	}

	//���_��ǉ�����
	void Face::AddwpVertexObj(const std::weak_ptr<Vertex> &wpVertexObj)
	{
		this->vectorwpVertexObj.push_back(wpVertexObj);

		return;
	}

	//�o�^����Ă��钸�_���폜
	void Face::ErasewpVertexObj(const std::weak_ptr<Vertex> &wpVertexObj)
	{
		for (std::vector<std::weak_ptr<Vertex>>::iterator itr =this-> vectorwpVertexObj.begin(); itr != this->vectorwpVertexObj.end();)
		{
			if (itr->lock() == wpVertexObj.lock())
				itr = this->vectorwpVertexObj.erase(itr);
			else
				itr++;
		}

		return;
	}

	//2�̒��_�I�u�W�F�N�g�������Ă��邩�ǂ����𔻒�
	bool Face::SearchVertices(const std::weak_ptr<Vertex> &wpVertexObj1, const std::weak_ptr<Vertex> &wpVertexObj2)
	{
		bool Find[2] = { false, false };

		for (unsigned int i = 0; i < this->vectorwpVertexObj.size(); i++)
		{
			if (wpVertexObj1.lock() == this->vectorwpVertexObj.at(i).lock())
				Find[0] = true;
			if (wpVertexObj2.lock() == this->vectorwpVertexObj.at(i).lock())
				Find[1] = true;
		}

		if (Find[0] == true && Find[1] == true)
			return true;

		return false;
	}

	/*
	//��������i���_��ӂ����Ȃ����j
	std::shared_ptr<Face> Face::Duplicate()
	{
		
		std::shared_ptr<Face> spNewFaceObj(new Face);

		//���_�ƕӂ̃I�u�W�F�N�g���R�s�[���쐬
		spNewFaceObj->getvectorwpVertexObj().resize(this->vectorwpVertexObj.size());
		spNewFaceObj->getvectorwpEdgeObj().resize(this->vectorwpEdgeObj.size());
		for (unsigned int i = 0; i < this->vectorwpVertexObj.size(); i++)
		{
			spNewFaceObj->getpvectorwpVertexObj().at(i) = new Vertex(*vectorpVertexObj.at(i));
			spNewFaceObj->getpvectorwpVertexObj().at(i)->AddpFaceObj(pNewFaceObj);
		}
		for (unsigned int i = 0; i < vectorpEdgeObj.size(); i++)
		{
			pNewFaceObj->GetpvectorpEdgeObj()->at(i) = new Edge(pNewFaceObj->GetpvectorpVertexObj()->at(i), pNewFaceObj->GetpvectorpVertexObj()->at((i + 1) % vectorpEdgeObj.size()), i);
			pNewFaceObj->GetpvectorpEdgeObj()->at(i)->AddpFaceObj(pNewFaceObj);
		}

		//�ӂ�ID�����̕ӂ�ID�Ɠ����ɂ���
		for (unsigned int i = 0; i < pNewFaceObj->GetpvectorpEdgeObj()->size(); i++)
		{
			for (unsigned int j = 0; j < vectorpEdgeObj.size(); j++)
			{
				if ((vectorpEdgeObj.at(j)->GetppVertexObj()[0]->GetID() == pNewFaceObj->GetpvectorpEdgeObj()->at(i)->GetppVertexObj()[0]->GetID() &&
					vectorpEdgeObj.at(j)->GetppVertexObj()[1]->GetID() == pNewFaceObj->GetpvectorpEdgeObj()->at(i)->GetppVertexObj()[1]->GetID()) ||
					(vectorpEdgeObj.at(j)->GetppVertexObj()[0]->GetID() == pNewFaceObj->GetpvectorpEdgeObj()->at(i)->GetppVertexObj()[1]->GetID() &&
					vectorpEdgeObj.at(j)->GetppVertexObj()[1]->GetID() == pNewFaceObj->GetpvectorpEdgeObj()->at(i)->GetppVertexObj()[0]->GetID()))
				{
					pNewFaceObj->GetpvectorpEdgeObj()->at(i)->SetID(vectorpEdgeObj.at(j)->GetID());
				}
			}
		}

		return pNewFaceObj;
		
	}
	*/

	//�o�^����Ă���ӂ��폜
	void Face::ErasewpEdgeObj(const std::weak_ptr<Edge> &wpEdgeObj)
	{
		for (std::vector<std::weak_ptr<Edge>>::iterator itr = this->vectorwpEdgeObj.begin(); itr != this->vectorwpEdgeObj.end();)
		{
			if (itr->lock() == wpEdgeObj.lock())
				itr = vectorwpEdgeObj.erase(itr);
			else
				itr++;
		}

		return;
	}

	//�ʐς��v�Z����
	double Face::CalcArea()
	{
		double Len[3];
		Len[0] = vec3d::GetDistance(this->vectorwpVertexObj.at(0).lock()->getvecLocalPos(), this->vectorwpVertexObj.at(1).lock()->getvecLocalPos());
		Len[1] = vec3d::GetDistance(this->vectorwpVertexObj.at(1).lock()->getvecLocalPos(), this->vectorwpVertexObj.at(2).lock()->getvecLocalPos());
		Len[2] = vec3d::GetDistance(this->vectorwpVertexObj.at(2).lock()->getvecLocalPos(), this->vectorwpVertexObj.at(0).lock()->getvecLocalPos());

		double s = 0.5 * (Len[0] + Len[1] + Len[2]);

		double ToSqrt = s * (s - Len[0]) * (s - Len[1]) * (s - Len[2]); //sqrt�֐��ɓ����O�i�K�̒l

		if (ToSqrt < 0) //sqrt�ɕ��̒l������Ɛ���ɋ��܂�Ȃ�����
			return 0;

		return sqrt(ToSqrt);
	}

	//���钆�S���W�Ƃ�4�_�̑̐ς����߂�
	double Face::CalcVolume(const vec3d &vecCenter)
	{
		return abs(vec3d::GetDot(this->vectorwpVertexObj.at(0).lock()->getvecLocalPos() - vecCenter, vec3d::GetCross(this->vectorwpVertexObj.at(2).lock()->getvecLocalPos() - vecCenter, this->vectorwpVertexObj.at(1).lock()->getvecLocalPos() - vecCenter)) / 6.0);
	}

	//���钆�S���W�Ƃ�4�_�̕��ρi�d�S�j�����߂�
	vec3d Face::CalcCentroid(const vec3d &vecCenter)
	{
		return (vecCenter + this->vectorwpVertexObj.at(0).lock()->getvecLocalPos() + this->vectorwpVertexObj.at(1).lock()->getvecLocalPos() + this->vectorwpVertexObj.at(2).lock()->getvecLocalPos()) * 0.25;
	}

	//�C�ӂ̍��W����C�ӂ̕����֌����������Ƃ̌�_�����߁A���܂����ꍇ�͔C�ӂ̍��W����̋�����Ԃ�
	bool Face::CalcDistance(double &OutDistance, const vec3d &vecPos, const vec3d &vecDirection)
	{
		vec3d vecN = getvecLocalNormal(); //�ʂ̖@���x�N�g�����v�Z
		double Dot = vec3d::GetDot(vecN, vecDirection);
		if (Dot == 0.0) //�C�ӂ̕����x�N�g���Ɩʂ̖@���x�N�g�������s�ȏꍇ
			return false; //�����Ȃ�

		double Distance = vec3d::GetDot(-vecN, (vecPos - this->vectorwpVertexObj.at(0).lock()->getvecLocalPos())) / Dot; //�C�ӂ̍��W�����_�܂ł̋��������߂�

		vec3d vecCrossPoint = vecPos + vecDirection * Distance; //��_���W�����߂�

		//��_���W���O�p�`�̓����ɂ��邩�ǂ����𒲂ׂ�
		bool Inner = true;
		vec3d vecCross;
		for (int i = 0; i < 3; i++)
		{
			vecCross = vec3d::GetCross(vecCrossPoint - this->vectorwpVertexObj.at(i).lock()->getvecLocalPos(), this->vectorwpVertexObj.at((i + 1) % 3).lock()->getvecLocalPos() - this->vectorwpVertexObj.at(i).lock()->getvecLocalPos()); //�O�p�`�̕ӂƁA�����_�J����_�֌������x�N�g���Ƃ̊O��
			if (vec3d::GetDot(vecCross, vecN) > 0) //���ς��������A�܂�O�p�`�̊O���̏ꍇ
			{
				Inner = false;
				break;
			}
		}

		if (!Inner) //��_���O�p�`�̊O���������ꍇ
			return false;

		OutDistance = Distance;

		return true;
	}
	
	//2�̒��_�̃C���f�b�N�X����ӃI�u�W�F�N�g�𓾂�
	std::weak_ptr<Edge> Face::GetwpEdgeObj(const int VertexIndex1, const int VertexIndex2)
	{
		for (std::vector<std::weak_ptr<Edge>>::iterator itr = this->vectorwpEdgeObj.begin(); itr != this->vectorwpEdgeObj.end(); itr++) //�ӃI�u�W�F�N�g�𑖍�
			if ((this->vectorwpVertexObj.at(VertexIndex1).lock() == itr->lock()->getwpVertexObj1().lock() && this->vectorwpVertexObj.at(VertexIndex2).lock() == itr->lock()->getwpVertexObj2().lock() ||
				(this->vectorwpVertexObj.at(VertexIndex1).lock() == itr->lock()->getwpVertexObj1().lock() && this->vectorwpVertexObj.at(VertexIndex2).lock() == itr->lock()->getwpVertexObj1().lock()))) //�����ő����Ă����C���f�b�N�X�̒��_�����ӂ������ꍇ
				return *itr;

		return std::weak_ptr<Edge>();
	}

	//�������钸�_�ƕӂ̃I�u�W�F�N�g���폜
	void Face::DeleteVertexAndEdge()
	{
		for (std::vector<std::weak_ptr<Vertex>>::iterator itr = this->vectorwpVertexObj.begin(); itr != this->vectorwpVertexObj.end(); itr++)
			itr->lock().reset();
		for (std::vector<std::weak_ptr<Edge>>::iterator itr = this->vectorwpEdgeObj.begin(); itr != this->vectorwpEdgeObj.end(); itr++)
			itr->lock().reset();

		this->vectorwpVertexObj.clear();
		this->vectorwpEdgeObj.clear();

		return;
	}

	//���[�J�����W�̖@���x�N�g�����v�Z����
	void Face::CalcvecLocalNormal()
	{
		this->vecLocalNormal = vec3d::GetCross(this->vectorwpVertexObj.at(1).lock()->getvecLocalPos() - this->vectorwpVertexObj.at(0).lock()->getvecLocalPos(), this->vectorwpVertexObj.at(2).lock()->getvecLocalPos() - this->vectorwpVertexObj.at(0).lock()->getvecLocalPos()).GetNormalize();

		return;
	}
	//���[���h���W�̖@���x�N�g�����v�Z����
	void Face::CalcvecWorldNormal()
	{
		this->vecWorldNormal = vec3d::GetCross(this->vectorwpVertexObj.at(1).lock()->getvecWorldPos() - this->vectorwpVertexObj.at(0).lock()->getvecWorldPos(), this->vectorwpVertexObj.at(2).lock()->getvecWorldPos() - this->vectorwpVertexObj.at(0).lock()->getvecWorldPos()).GetNormalize();

		return;
	}
}
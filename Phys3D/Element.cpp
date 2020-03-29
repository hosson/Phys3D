#include "Element.h"
#include "AppCore.h"
#include "Phys3D\Phys3D.h"
#include "hoLib\num\vector.h"
#include "Phys3D\MassPoint.h"
#include "hoLib\num\Quaternion.h"
#include "hoLib\Random.h"

using namespace ho::num;

//�R���X�g���N�^
ElementManager::ElementManager(const std::weak_ptr<AppCore> &wpAppCoreObj)
	: wpAppCoreObj(wpAppCoreObj)
{
	ho::xorShift xorShiftObj;

	const std::shared_ptr<Phys3D::Phys3D> &spPhys3DObj = wpAppCoreObj.lock()->getspPhys3DObj();

	//���̂̐����F��
	{
		std::vector<Phys3D::MASSPOINT> vectorMassPoint; //���_�z��
		std::vector<vec3d> vectorvecVertex; //���_�z��
		std::vector<int> vectorVtIndex; //���_�C���f�b�N�X

		//���_��ݒ�
		vectorMassPoint.push_back(Phys3D::MASSPOINT(vec3d(-0.5f, 1.0f, 0.5f), 10.0));
		vectorMassPoint.push_back(Phys3D::MASSPOINT(vec3d(0.5f, 1.0f, 0.5f), 10.0));
		vectorMassPoint.push_back(Phys3D::MASSPOINT(vec3d(-0.5f, 1.0f, -0.5f), 10.0));
		vectorMassPoint.push_back(Phys3D::MASSPOINT(vec3d(0.5f, 1.0f, -0.5f), 10.0));
		vectorMassPoint.push_back(Phys3D::MASSPOINT(vec3d(-1.0f, -1.0f, 1.0f), 10.0));
		vectorMassPoint.push_back(Phys3D::MASSPOINT(vec3d(1.0f, -1.0f, 1.0f), 10.0));
		vectorMassPoint.push_back(Phys3D::MASSPOINT(vec3d(-1.0f, -1.0f, -1.0f), 10.0));
		vectorMassPoint.push_back(Phys3D::MASSPOINT(vec3d(1.0f, -1.0f, -1.0f), 10.0));

		//���_�z��
		vectorvecVertex.push_back(vec3d(-50.0f, 1.0f, 50.0f));
		vectorvecVertex.push_back(vec3d(50.0f, 1.0f, 50.0f));
		vectorvecVertex.push_back(vec3d(-50.0f, 1.0f, -50.0f));
		vectorvecVertex.push_back(vec3d(50.0f, 1.0f, -50.0f));
		vectorvecVertex.push_back(vec3d(-50.0f, -1.0f, 50.0f));
		vectorvecVertex.push_back(vec3d(50.0f, -1.0f, 50.0f));
		vectorvecVertex.push_back(vec3d(-50.0f, -1.0f, -50.0f));
		vectorvecVertex.push_back(vec3d(50.0f, -1.0f, -50.0f));

		//���_�C���f�b�N�X
		{
			//��
			vectorVtIndex.push_back(0);
			vectorVtIndex.push_back(1);
			vectorVtIndex.push_back(2);
			vectorVtIndex.push_back(2);
			vectorVtIndex.push_back(1);
			vectorVtIndex.push_back(3);
			//��
			vectorVtIndex.push_back(5);
			vectorVtIndex.push_back(4);
			vectorVtIndex.push_back(7);
			vectorVtIndex.push_back(7);
			vectorVtIndex.push_back(4);
			vectorVtIndex.push_back(6);
			//�O
			vectorVtIndex.push_back(2);
			vectorVtIndex.push_back(3);
			vectorVtIndex.push_back(6);
			vectorVtIndex.push_back(6);
			vectorVtIndex.push_back(3);
			vectorVtIndex.push_back(7);
			//��
			vectorVtIndex.push_back(1);
			vectorVtIndex.push_back(0);
			vectorVtIndex.push_back(5);
			vectorVtIndex.push_back(5);
			vectorVtIndex.push_back(0);
			vectorVtIndex.push_back(4);
			//��
			vectorVtIndex.push_back(0);
			vectorVtIndex.push_back(2);
			vectorVtIndex.push_back(4);
			vectorVtIndex.push_back(4);
			vectorVtIndex.push_back(2);
			vectorVtIndex.push_back(6);
			//�E
			vectorVtIndex.push_back(3);
			vectorVtIndex.push_back(1);
			vectorVtIndex.push_back(7);
			vectorVtIndex.push_back(7);
			vectorVtIndex.push_back(1);
			vectorVtIndex.push_back(5);
		}

		//�����G���W�����ɕ��̂𐶐����āA���̃|�C���^�𓾂�
		std::shared_ptr<Phys3D::Body> spBodyObj = spPhys3DObj->CreateBody(
			vec3d(0, 0.0, 0),							//�ʒu
			vec3d(0, 0, 0),							//���x
			Quaternion(vec3d(0.0f, 0.0, 1.0), 0.0),//��]�p�x�i�p���j
			vec3d(0, 0.0, 0),						//�p���x
			vectorMassPoint,						//���_�z��
			vectorvecVertex,						//���_�z��
			vectorVtIndex,							//���_�C���f�b�N�X
			0.2,									//�����W��
			0.5,									//���C�W��
			true);

		//�G�������g�𐶐����ă��X�g�ɒǉ�
		this->listspElementObj.push_back(std::shared_ptr<Element>(new Element(spBodyObj, vectorvecVertex, vectorVtIndex, XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f))));
	}

	/*
	//���̂̐����F�����K
	{
		const double size = 1.0f;

		std::vector<Phys3D::MASSPOINT> vectorMassPoint; //���_�z��
		std::vector<vec3d> vectorvecVertex; //���_�z��
		std::vector<int> vectorVtIndex; //���_�C���f�b�N�X

		//���_�z��
		vectorvecVertex.push_back(vec3d(-size * 2.0, size, size));
		vectorvecVertex.push_back(vec3d(size * 2.0, size, size));
		vectorvecVertex.push_back(vec3d(-size * 2.0, size, -size));
		vectorvecVertex.push_back(vec3d(size * 2.0, size, -size));
		vectorvecVertex.push_back(vec3d(-size * 2.0, -size, size));
		vectorvecVertex.push_back(vec3d(size * 2.0, -size, size));
		vectorvecVertex.push_back(vec3d(-size * 2.0, -size, -size));
		vectorvecVertex.push_back(vec3d(size * 2.0, -size, -size));

		//���_�C���f�b�N�X
		{
			//��
			vectorVtIndex.push_back(0);
			vectorVtIndex.push_back(1);
			vectorVtIndex.push_back(2);
			vectorVtIndex.push_back(2);
			vectorVtIndex.push_back(1);
			vectorVtIndex.push_back(3);
			//��
			vectorVtIndex.push_back(5);
			vectorVtIndex.push_back(4);
			vectorVtIndex.push_back(7);
			vectorVtIndex.push_back(7);
			vectorVtIndex.push_back(4);
			vectorVtIndex.push_back(6);
			//�O
			vectorVtIndex.push_back(2);
			vectorVtIndex.push_back(3);
			vectorVtIndex.push_back(6);
			vectorVtIndex.push_back(6);
			vectorVtIndex.push_back(3);
			vectorVtIndex.push_back(7);
			//��
			vectorVtIndex.push_back(1);
			vectorVtIndex.push_back(0);
			vectorVtIndex.push_back(5);
			vectorVtIndex.push_back(5);
			vectorVtIndex.push_back(0);
			vectorVtIndex.push_back(4);
			//��
			vectorVtIndex.push_back(0);
			vectorVtIndex.push_back(2);
			vectorVtIndex.push_back(4);
			vectorVtIndex.push_back(4);
			vectorVtIndex.push_back(2);
			vectorVtIndex.push_back(6);
			//�E
			vectorVtIndex.push_back(3);
			vectorVtIndex.push_back(1);
			vectorVtIndex.push_back(7);
			vectorVtIndex.push_back(7);
			vectorVtIndex.push_back(1);
			vectorVtIndex.push_back(5);
		}

		const int iMax = 10, jMax = 10;
		for (int i = 0; i < iMax; i++)
		{
			for (int j = 0; j < jMax; j++)
			{
				//�����G���W�����ɕ��̂𐶐����āA���̃|�C���^�𓾂�
				std::shared_ptr<Phys3D::Body> spBodyObj = spPhys3DObj->CreateBody(
					vec3d((jMax * -0.5 + j + (i % 2) * 0.5) * size * 4.0, 2.0 + i * size * 2.0, 0),//�ʒu
					vec3d(0, 0, 0),								//���x
					Quaternion(vec3d(0.0f, 0.0, 1.0), 0.0),		//��]�p�x�i�p���j
					vec3d(0, 0.0, 0.0),							//�p���x
					1.0,							//���_�z��
					vectorvecVertex,							//���_�z��
					vectorVtIndex,								//���_�C���f�b�N�X
					0.2,										//�����W��
					0.5,										//���C�W��
					false);

				//�G�������g�𐶐����ă��X�g�ɒǉ�
				this->listspElementObj.push_back(std::shared_ptr<Element>(new Element(spBodyObj, vectorvecVertex, vectorVtIndex, XMFLOAT4((float)xorShiftObj.get(0.0, 1.0), (float)xorShiftObj.get(0.0, 1.0), (float)xorShiftObj.get(0.0, 1.0), (float)xorShiftObj.get(1.0, 1.0)))));
			}
		}
	}
	*/


	
	//���̂̐����F�ςݏグ�锠
	{
	const double size = 1.0f;

	std::vector<Phys3D::MASSPOINT> vectorMassPoint; //���_�z��
	std::vector<vec3d> vectorvecVertex; //���_�z��
	std::vector<int> vectorVtIndex; //���_�C���f�b�N�X

	//���_�z��
	vectorvecVertex.push_back(vec3d(-size, size, size));
	vectorvecVertex.push_back(vec3d(size, size, size));
	vectorvecVertex.push_back(vec3d(-size, size, -size));
	vectorvecVertex.push_back(vec3d(size, size, -size));
	vectorvecVertex.push_back(vec3d(-size, -size, size));
	vectorvecVertex.push_back(vec3d(size, -size, size));
	vectorvecVertex.push_back(vec3d(-size, -size, -size));
	vectorvecVertex.push_back(vec3d(size, -size, -size));

	//���_�C���f�b�N�X
	{
	//��
	vectorVtIndex.push_back(0);
	vectorVtIndex.push_back(1);
	vectorVtIndex.push_back(2);
	vectorVtIndex.push_back(2);
	vectorVtIndex.push_back(1);
	vectorVtIndex.push_back(3);
	//��
	vectorVtIndex.push_back(5);
	vectorVtIndex.push_back(4);
	vectorVtIndex.push_back(7);
	vectorVtIndex.push_back(7);
	vectorVtIndex.push_back(4);
	vectorVtIndex.push_back(6);
	//�O
	vectorVtIndex.push_back(2);
	vectorVtIndex.push_back(3);
	vectorVtIndex.push_back(6);
	vectorVtIndex.push_back(6);
	vectorVtIndex.push_back(3);
	vectorVtIndex.push_back(7);
	//��
	vectorVtIndex.push_back(1);
	vectorVtIndex.push_back(0);
	vectorVtIndex.push_back(5);
	vectorVtIndex.push_back(5);
	vectorVtIndex.push_back(0);
	vectorVtIndex.push_back(4);
	//��
	vectorVtIndex.push_back(0);
	vectorVtIndex.push_back(2);
	vectorVtIndex.push_back(4);
	vectorVtIndex.push_back(4);
	vectorVtIndex.push_back(2);
	vectorVtIndex.push_back(6);
	//�E
	vectorVtIndex.push_back(3);
	vectorVtIndex.push_back(1);
	vectorVtIndex.push_back(7);
	vectorVtIndex.push_back(7);
	vectorVtIndex.push_back(1);
	vectorVtIndex.push_back(5);
	}

	for (int i = 0; i < 7; i++)
	//for (int i = 7; i >= 0; i--)
	{
	//�����G���W�����ɕ��̂𐶐����āA���̃|�C���^�𓾂�
	std::shared_ptr<Phys3D::Body> spBodyObj = spPhys3DObj->CreateBody(
	vec3d(0.0 + i * 0.0000, 2.0 + i * size * 2.0, 0),						//�ʒu
	vec3d(0, 0, 0),								//���x
	Quaternion(vec3d(0.0f, 0.0, 1.0), 0.0),		//��]�p�x�i�p���j
	vec3d(0, 0.0, 0.0),							//�p���x
	1.0,							//���_�z��
	vectorvecVertex,							//���_�z��
	vectorVtIndex,								//���_�C���f�b�N�X
	0.2,										//�����W��
	0.5,										//���C�W��
	false);

	//�G�������g�𐶐����ă��X�g�ɒǉ�
	this->listspElementObj.push_back(std::shared_ptr<Element>(new Element(spBodyObj, vectorvecVertex, vectorVtIndex, XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f))));
	}
	}
	

/*
	//���̂̐���
	{
	const double size = 1.0f;

	std::vector<vec3d> vectorvecVertex; //���_�z��
	std::vector<int> vectorVtIndex; //���_�C���f�b�N�X

	//���_�z��
	vectorvecVertex.push_back(vec3d(-size, size, 0));
	vectorvecVertex.push_back(vec3d(0, size, size));
	vectorvecVertex.push_back(vec3d(size, size, 0));
	vectorvecVertex.push_back(vec3d(0, size, -size));
	vectorvecVertex.push_back(vec3d(0, -size, 0));

	//���_�C���f�b�N�X
	{
	//��
	vectorVtIndex.push_back(0);
	vectorVtIndex.push_back(1);
	vectorVtIndex.push_back(2);
	vectorVtIndex.push_back(0);
	vectorVtIndex.push_back(2);
	vectorVtIndex.push_back(3);

	vectorVtIndex.push_back(1);
	vectorVtIndex.push_back(0);
	vectorVtIndex.push_back(4);

	vectorVtIndex.push_back(2);
	vectorVtIndex.push_back(1);
	vectorVtIndex.push_back(4);

	vectorVtIndex.push_back(3);
	vectorVtIndex.push_back(2);
	vectorVtIndex.push_back(4);

	vectorVtIndex.push_back(0);
	vectorVtIndex.push_back(3);
	vectorVtIndex.push_back(4);
	}

	//�����G���W�����ɕ��̂𐶐����āA���̃|�C���^�𓾂�
	std::shared_ptr<Phys3D::Body> spBodyObj = spPhys3DObj->CreateBody(
	vec3d(0.0, size * 2.0, 0),						//�ʒu
	vec3d(0, 0, 0),								//���x
	Quaternion(vec3d(0.0f, 0.0, 1.0), -0.1),		//��]�p�x�i�p���j
	vec3d(0, 40.0, 0.0),							//�p���x
	1.0,							//���_�z��
	vectorvecVertex,							//���_�z��
	vectorVtIndex,								//���_�C���f�b�N�X
	0.8,										//�����W��
	0.1,										//���C�W��
	false);

	//�G�������g�𐶐����ă��X�g�ɒǉ�
	this->listspElementObj.push_back(std::shared_ptr<Element>(new Element(spBodyObj, vectorvecVertex, vectorVtIndex, XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f))));
	}
	*/

}

//�f�X�g���N�^
ElementManager::~ElementManager()
{}





//�R���X�g���N�^
Element::Element(const std::shared_ptr<Phys3D::Body> &spBodyObj, const std::vector<vec3d> &vectorvecVertex, const std::vector<int> &vectorVtIndex, const XMFLOAT4 &vecColor)
	: spBodyObj(spBodyObj)
{
	for (unsigned int i = 0; i < vectorVtIndex.size(); i += 3) //�|���S���P�ʂŒ��_�C���f�b�N�X�𑖍�
	{
		//�O�ςɂ��@���x�N�g�����v�Z
		XMVECTOR vecNormal = XMVector3Normalize(XMVector3Cross(XMVectorSubtract(XMLoadFloat3(&(XMFLOAT3)vectorvecVertex.at(vectorVtIndex.at(i + 1))), XMLoadFloat3(&(XMFLOAT3)vectorvecVertex.at(vectorVtIndex.at(i)))), XMVectorSubtract(XMLoadFloat3(&(XMFLOAT3)vectorvecVertex.at(vectorVtIndex.at(i + 2))), XMLoadFloat3(&(XMFLOAT3)vectorvecVertex.at(vectorVtIndex.at(i))))));

		for (int j = 0; j < 3; j++) //�|���S������3�̒��_�𑖍�
		{
			ShaderSimple_base::CustomVertex cv;
			cv.position = vectorvecVertex.at(vectorVtIndex.at(i + j)); //vec3d ���� XMFLOAT3 �ɕϊ����Ȃ���R�s�[
			cv.Color = vecColor; //�F
			XMStoreFloat3(&cv.Normal, vecNormal);

			this->vectorvecVertex.push_back(cv);
		}
	}
}

//�f�X�g���N�^
Element::~Element()
{}
#include "AppCore.h"
#include "App.h"
#include "hoLib\D3D11.h"
#include "hoLib\Error.h"
#include "ShaderSimple.h"
#include "Shader2DTexture.h"
#include <vector>
#include "hoLib\Profiler.h"
#include "hoLib\MultiThread.h"
#include "Phys3D\Phys3D.h"
#include "Element.h"

//�R���X�g���N�^
AppCore::AppCore(const std::shared_ptr<App> &spAppObj)
	: spAppObj(spAppObj)
{
}

//�f�X�g���N�^
AppCore::~AppCore()
{
}

//������
void AppCore::Init(const std::weak_ptr<AppCore> &wpThisObj)
{
	this->wpThisObj = wpThisObj;

	this->spShaderSimpleObj.reset(new ShaderSimple(spAppObj->getspD3D11Obj())); //�V���v���V�F�[�_�I�u�W�F�N�g����
	this->spShader2DTextureObj.reset(new Shader2DTexture(spAppObj->getspD3D11Obj())); //2D�e�N�X�`���`��V�F�[�_�I�u�W�F�N�g����

	this->spProfilerObj.reset(new ho::Profiler(10)); //�v���t�@�C���I�u�W�F�N�g����
	this->spMultiThreadObj.reset(new ho::MultiThread(4)); //�}���`�X���b�h�I�u�W�F�N�g����

	this->spPhys3DObj = ho::sp_init<Phys3D::Phys3D>(new Phys3D::Phys3D(1.0 / 60.0, 50)); //�������Z�I�u�W�F�N�g����
	this->spElementManagerObj.reset(new ElementManager(wpThisObj)); //���̊Ǘ��I�u�W�F�N�g����

	return;
}

//�X�V
void AppCore::Update()
{
	this->spPhys3DObj->Update(); //�������Z�I�u�W�F�N�g��1�t���[�����̏���

	return;
}

//�`��
void AppCore::Draw()
{
	std::shared_ptr<ho::D3D11::D3D11> spDirect3D11Obj = this->spAppObj->getspD3D11Obj();

	spDirect3D11Obj->DrawFrameBegin(); //1�t���[���̕`��J�n

	//�V�F�[�_�̕`��
	{
		//�G�������g���X�g���擾
		const std::list<std::shared_ptr<Element>> &listspElementObj = this->spElementManagerObj->getlistspElementObj();

		for (auto itr = listspElementObj.begin(); itr != listspElementObj.end(); itr++) //�G�������g�𑖍�
		{
			const std::shared_ptr<Phys3D::Body> &spBodyObj = (*itr)->getspBodyObj();

			XMMATRIX mtrxWorld = spBodyObj->GetCoordTransMatrix(); //�`��p�̍��W�ϊ��s����擾

			this->spShaderSimpleObj->DrawUp(
				(ShaderSimple_base::CustomVertex *const)&(*itr)->getvectorvecVertex().at(0),
				(*itr)->getvectorvecVertex().size(),
				NULL,
				0,
				mtrxWorld);
		}

		//this->spShaderSimpleObj->Draw();
		//this->spShader2DTextureObj->Draw();
	}

	spDirect3D11Obj->DrawFrameEnd(); //1�t���[���̕`��I��

	return;
}

//���݂̕��̂���`��p�̒��_���𐶐�
void AppCore::CreateVertices(std::vector<DirectX::XMFLOAT3> &vectorvecVertex)
{
	/*
	const std::list<std::shared_ptr<Element>> &listspElementObj = this->spElementManagerObj->getlistspElementObj();

	std::for_each(listspElementObj.cbegin(), listspElementObj.cend(), [&](const std::shared_ptr<Element> &p)
	{
		if (auto p2 = p->getwpElementObj().lock())
		{
			const std::vector<vec2d> &vectorvecShapeWorld = p2->getvectorvecShapeWorld(); //���[���h���W�ł̌`��f�[�^���擾

			for (unsigned int i = 1; i < vectorvecShapeWorld.size() - 1; i++) //�`��f�[�^���O�p�`��ɑ���
			{
				vectorvecVertex.push_back(vectorvecShapeWorld.at(0).Getvec3d_XY(0));
				vectorvecVertex.push_back(vectorvecShapeWorld.at(i).Getvec3d_XY(0));
				vectorvecVertex.push_back(vectorvecShapeWorld.at(i + 1).Getvec3d_XY(0));
			}
		}
	});
	*/

	return;
}
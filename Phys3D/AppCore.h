//�X�V�ƕ`����i��A�A�v���P�[�V�����̊j�ƂȂ�N���X
#pragma once

#include <Windows.h>
#include <vector>
#include <boost\shared_ptr.hpp>
#include <DirectXMath.h>

class App;
class ShaderSimple;
class Shader2DTexture;
class ElementManager;

namespace ho
{
	class Profiler;
	class MultiThread;
}

namespace Phys3D
{
	class Phys3D;
}

class AppCore
{
public:
	AppCore(const std::shared_ptr<App> &spAppObj); //�R���X�g���N�^
	~AppCore(); //�f�X�g���N�^

	void Init(const std::weak_ptr<AppCore> &wpThisObj); //������

	void Update(); //�X�V
	void Draw(); //�`��

	//�A�N�Z�b�T
	const std::weak_ptr<AppCore> &getwpThisObj() const { return wpThisObj; }
	const std::shared_ptr<Phys3D::Phys3D> &getspPhys3DObj() const { return spPhys3DObj; }
private:
	std::weak_ptr<AppCore> wpThisObj;
	const std::shared_ptr<App> &spAppObj; //�A�v���P�[�V�����I�u�W�F�N�g
	std::shared_ptr<ShaderSimple> spShaderSimpleObj; //�V���v���V�F�[�_�I�u�W�F�N�g
	std::shared_ptr<Shader2DTexture> spShader2DTextureObj; //2D�e�N�X�`����`�悷��V�F�[�_�I�u�W�F�N�g
	std::shared_ptr<ho::Profiler> spProfilerObj; //�v���t�@�C���I�u�W�F�N�g
	std::shared_ptr<ho::MultiThread> spMultiThreadObj; //�}���`�X���b�h�I�u�W�F�N�g
	std::shared_ptr<Phys3D::Phys3D> spPhys3DObj; //�������Z�I�u�W�F�N�g
	std::shared_ptr<ElementManager> spElementManagerObj; //���̊Ǘ��I�u�W�F�N�g

	void CreateVertices(std::vector<DirectX::XMFLOAT3> &vectorvecVertex); //���݂̕��̂���`��p�̒��_���𐶐�
};
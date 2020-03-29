//���̃N���X
#pragma once
#include <memory>
#include <list>
#include "Phys3D\Body.h"
#include <DirectXMath.h>
#include "ShaderSimple_base.h"

class AppCore;
class Element;

using namespace DirectX;

class ElementManager
{
public:
	ElementManager(const std::weak_ptr<AppCore> &wpAppCoreObj); //�R���X�g���N�^
	~ElementManager(); //�f�X�g���N�^

	//�A�N�Z�b�T
	const std::list<std::shared_ptr<Element>> &getlistspElementObj() const { return listspElementObj; }
private:
	std::weak_ptr<AppCore> wpAppCoreObj;
	std::list<std::shared_ptr<Element>> listspElementObj; //���̃��X�g
};

class Element
{
public:
	Element(const std::shared_ptr<Phys3D::Body> &spBodyObj, const std::vector<vec3d> &vectorvecVertex, const std::vector<int> &vectorVtIndex, const XMFLOAT4 &vecColor); //�R���X�g���N�^
	~Element(); //�f�X�g���N�^

	//�A�N�Z�b�T
	const std::shared_ptr<Phys3D::Body> &getspBodyObj() const { return spBodyObj; }
	const std::vector<ShaderSimple_base::CustomVertex> &getvectorvecVertex() const { return vectorvecVertex; }
private:
	std::shared_ptr<Phys3D::Body> spBodyObj; //�����G���W�����̕��̃I�u�W�F�N�g�ւ̃|�C���^
	std::vector<ShaderSimple_base::CustomVertex> vectorvecVertex; //���_�z��
};
//ShaderSimple.h �̊��N���X
//�\���̂��p�������邽��
#pragma once

#include <DirectXMath.h>
using namespace DirectX;

class ShaderSimple_base
{
public:
	//���_�V�F�[�_�֑��邽�߂̒��_���\����
	struct CustomVertex
	{
		XMFLOAT3 position; //�ʒu���
		XMFLOAT4 Color; //�F
		XMFLOAT3 Normal; //�@���x�N�g��
	};

	//�萔�o�b�t�@�\����
	struct ConstantBuffer
	{
		XMMATRIX mtrxWorld, mtrxView, mtrxProj;
		XMFLOAT3 vecLight; //���C�g�����i�����֌����������j
	};
};
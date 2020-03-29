#pragma once
#include "D3D11.h"
//�e�V�F�[�_�̊��N���X
//�o�[�e�b�N�X�V�F�[�_�A�W�I���g���V�F�[�_�A�s�N�Z���V�F�[�_�A���_�o�b�t�@�A�萔�o�b�t�@���ЂƂ܂Ƃ߂ɂ����N���X

namespace ho
{
	namespace D3D11
	{
		class Shader_Base
		{
		public:
			Shader_Base(const std::shared_ptr<D3D11> &spD3D11Obj); //�R���X�g���N�^
			virtual ~Shader_Base(); //�f�X�g���N�^

			virtual void Draw(); //1�t���[�����̕`��
		protected:
			void CompileShaderFromFile(const tstring &strFilename, LPCSTR pEntryPoint, LPCSTR pShaderModel, ho::ComPtr<ID3DBlob *> &cpBlobObj); //�t�@�C������V�F�[�_���R���p�C������

			std::shared_ptr<D3D11> spD3D11Obj;
		};
	}
}
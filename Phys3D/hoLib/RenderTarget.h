//�����_�����O�Ώۂ������N���X
#pragma once
#include "ComPtr.hpp"
#include "PT.h"
#include "D3DVertex.h"

namespace ho
{
	class Direct3D;

	class RenderTarget
	{
	public:
		RenderTarget(Direct3D *pDirect3DObj, const PT<int> &ptSize, D3DFORMAT fmt = D3DFMT_A8R8G8B8, DWORD VTColor = 0xffffffff); //�R���X�g���N�^
		~RenderTarget(); //�f�X�g���N�^

		void SetRenderTarget(int Index); //�f�o�C�X�̃����_�[�^�[�Q�b�g�Ɏw��
		void Render(); //�e�N�X�`�������݂̃����_�[�^�[�Q�b�g�ɕ`��
		void Render(PT<int> &ptSize, DWORD *pVtColor = NULL); //�e�N�X�`�������݂̃����_�[�^�[�Q�b�g�ɕ`��
		void Render(RenderTarget *pRenderTargetObj, int Index, DWORD *pVtColor = NULL, LPDIRECT3DVERTEXDECLARATION9 pvd = NULL); //�C�ӂ̃����_�[�^�[�Q�b�g�փe�N�X�`����`��
		void Fill(const DWORD &Color); //�e�N�X�`����C�ӂ̐F�Ŗ��߂�

		//�A�N�Z�b�T
		ho::ComPtr<LPDIRECT3DTEXTURE9> GetcpTex() { return cpTex; }
		PT<int> &GetptSize() { return ptSize; }
	private:
		Direct3D *pDirect3DObj;
		PT<int> ptSize; //�T�C�Y
		DWORD VTColor; //���_�J���[
		ho::ComPtr<LPDIRECT3DTEXTURE9> cpTex;
	};
}

/*
�e�N�X�`���Ⓒ�_����ێ����A�����_�����O�^�[�Q�b�g�ɐݒ肵����A
�e�N�X�`���ɐݒ肵���肷��@�\�����B

�o�b�N�o�b�t�@�ւ̕`��Ȃǂ� Render() ���g���A
���� RenderTarget �I�u�W�F�N�g�ǂ�����
�����_�����O����Ƃ��� Render(RenderTarget *pRenderTargetObj, int Index)
���g���B
*/
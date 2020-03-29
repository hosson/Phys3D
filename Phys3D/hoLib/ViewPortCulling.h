//������J�����O�N���X
#pragma once
#include <d3d9.h>
#include <d3dx9.h>

namespace ho
{
	class ViewPortCulling
	{
	public:
		ViewPortCulling(LPDIRECT3DDEVICE9 pD3DDevice); //�R���X�g���N�^
		~ViewPortCulling(); //�f�X�g���N�^

		void Update(float DrawDistance = 1.0f); //��������X�V
		void Update(::D3DXMATRIX *pmtrxView, ::D3DXMATRIX *pmtrxProj, float DrawDistance = 1.0f); //�C�ӂ̃r���[�s��Ǝˉe�s��ɂ�莋������X�V
		bool Judge(::D3DXVECTOR3 &vecWorldPos, float Radius); //�J�����O����
		void DrawViewPortWireFrame(DWORD Color); //���݂̎�����J�����O�͈̔͂����C���[�t���[���ŕ`��

		//�A�N�Z�b�T
		const D3DXVECTOR3 GetvecNearLeftTop() const { return vecNear[0]; }
		const D3DXVECTOR3 GetvecNearRightTop() const { return vecNear[1]; }
		const D3DXVECTOR3 GetvecNearLeftBottom() const { return vecNear[2]; }
		const D3DXVECTOR3 GetvecNearRightBottom() const { return vecNear[3]; }
		const D3DXVECTOR3 GetvecFarLeftTop() const { return vecFar[0]; }
		const D3DXVECTOR3 GetvecFarRightTop() const { return vecFar[1]; }
		const D3DXVECTOR3 GetvecFarLeftBottom() const { return vecFar[2]; }
		const D3DXVECTOR3 GetvecFarRightBottom() const { return vecFar[3]; }
		const D3DXVECTOR3 &GetvecNFar() const { return vecNFar; }
		const D3DXVECTOR3 &GetvecNNear() const { return vecNNear; }
		const D3DXVECTOR3 &GetvecNLeft() const { return vecNLeft; }
		const D3DXVECTOR3 &GetvecNRight() const { return vecNRight; }
		const D3DXVECTOR3 &GetvecNTop() const { return vecNTop; }
		const D3DXVECTOR3 &GetvecNBottom() const { return vecNBottom; }
	private:
		LPDIRECT3DDEVICE9 pD3DDevice;
		D3DXVECTOR3 vecNear[4], vecFar[4]; //�������8�_�̍��W
		D3DXVECTOR3 vecNFar, vecNNear, vecNLeft, vecNRight, vecNTop, vecNBottom; //�@���x�N�g��

		void CalcNormal(); //�������8�_�̍��W����@���x�N�g�����v�Z
	};
}

/*
1�̃v���O������1�����I�u�W�F�N�g���쐬����B
�e�t���[���̍ŏ��Ƀf�o�C�X�ɍ��W�ϊ��s�񂪐ݒ肳�ꂽ��� Update() ���\�b�h����x�����Ăяo���B
������0.0f�`1.0f�͈̔͂����A0�ɋ߂��قǋߋ��������`�悳��Ȃ��Ȃ�B
����œ����̎�����̈ʒu���ݒ肳���̂ŁA��͕`�悲�Ƃ� Judge() ���\�b�h���Ăяo���Ĕ��肷��B
*/

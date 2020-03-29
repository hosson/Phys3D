//Direct3D��DrawPrimitive�n�p���_�f�[�^

#pragma once
#include <Windows.h>
#include "Direct3D.h"
#include "Vector.h"

namespace ho {
	//3�����̒��_�t�H�[�}�b�g�\����
	class D3DVERTEX
	{
	public:
		//�R���X�g���N�^
		D3DVERTEX() {}
		D3DVERTEX(float x, float y, float z, float nx, float ny, float nz, DWORD diff, DWORD spec, float tu, float tv)
		{ SetVertex(x, y, z, nx, ny, nz, diff, spec, tu, tv); }

		float x, y, z; //���_���W
		float nx, ny, nz; //�@���x�N�g��
		DWORD diff; //�f�B�t���[�Y�F
		DWORD spec; //�X�y�L�����F
		float tu, tv; //�e�N�X�`�����W

		const static WORD IndexSquare[6]; //4�p�`�̒��_�C���f�b�N�X
		const static DWORD FVF; //���_�t�H�[�}�b�g
		const static D3DVERTEXELEMENT9 ve[6]; //���_���錾

		void SetVertex(float x, float y, float z, float nx, float ny, float nz, DWORD diff, DWORD spec, float tu, float tv); //���_�����Z�b�g
		void SetVertex(::D3DXVECTOR3 *pvecVt, ::D3DXVECTOR3 *pvecNormal, DWORD diff, DWORD spec, float tu, float tv); //���_�����Z�b�g�iD3DXVECTOR3�g�p�j
		void SetNormalLineVecFromD3DXVECTOR3(::D3DXVECTOR3 vecN) { nx = vecN.x; ny = vecN.y; nz = vecN.z; } //D3DXVECTOR3�^����@���x�N�g����ݒ�

		//���Z�q
		operator D3DXVECTOR3() { return D3DXVECTOR3(x, y, z); } //D3DXVECTOR3�^�œ���
		operator VECTOR3DOUBLE() { return VECTOR3DOUBLE(x, y, z); }

		//�}�`�쐬
		static void CreateLine(D3DVERTEX *pOut, D3DXVECTOR3 *pvecBegin, D3DXVECTOR3 *pvecEnd, D3DXVECTOR3 *pvecNormal, float Width, DWORD diff, DWORD spec); //�n�_�A�I�_�A�@���x�N�g���A������4���_�̐����쐬
	};

	//2D�p���_�t�H�[�}�b�g�\����
	class D3DVERTEX_2D
	{
	public:
		float x, y, z; //���_���W
		float rhw; //���1.0
		DWORD diff;
		float tu, tv; //�e�N�X�`�����W

		void SetVertex(float x, float y, float z, DWORD diff, float tu, float tv); //2D�̒��_�����Z�b�g
		void SetCoordinate(D3DXVECTOR2 &vec) { this->x = vec.x; this->y = vec.y; }
		void SetCoordinate(VECTOR2DOUBLE &vec) { this->x = (float)vec.x; this->y = (float)vec.y; }
		static void SetVertexFromRECT(D3DVERTEX_2D *lpVt, RECT *lpRect, float z, DWORD Color); //RECT�\���̂���4�̒��_����ݒ�
		static void Offset(D3DVERTEX_2D *pVt, int Number); //���W��-0.5f���炷

		const static DWORD FVF; //���_�t�H�[�}�b�g
		const static D3DVERTEXELEMENT9 ve[4]; //���_���錾

		//���Z�q
		operator VECTOR2DOUBLE() { return VECTOR2DOUBLE(x, y); }
	};

}
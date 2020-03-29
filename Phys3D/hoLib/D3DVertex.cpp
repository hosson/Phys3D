#include "D3DVertex.h"
#include "Direct3D.h"
#include <d3dx9math.h>
#include "Vector.h"

namespace ho {

	const WORD D3DVERTEX::IndexSquare[6] = {0, 1, 2, 2, 1, 3};
	const DWORD D3DVERTEX::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1;
	const D3DVERTEXELEMENT9 D3DVERTEX::ve[] = {
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0}, 
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0}, 
		{0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0}, 
		{0, 28, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 1}, 
		{0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0}, 
		//{0, 40, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
		D3DDECL_END() };


	//���_�����Z�b�g
	void D3DVERTEX::SetVertex(float x, float y, float z, float nx, float ny, float nz, DWORD diff, DWORD spec, float tu, float tv) 
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->nx = nx;
		this->ny = ny;
		this->nz = nz;
		this->diff = diff;
		this->spec = spec;
		this->tu = tu;
		this->tv = tv;

		return;
	}

	//���_�����Z�b�g�iD3DXVECTOR3�g�p�j
	void D3DVERTEX::SetVertex(::D3DXVECTOR3 *pvecVt, ::D3DXVECTOR3 *pvecNormal, DWORD diff, DWORD spec, float tu, float tv)
	{
		this->x = pvecVt->x;
		this->y = pvecVt->y;
		this->z = pvecVt->z;
		this->nx = pvecNormal->x;
		this->ny = pvecNormal->y;
		this->nz = pvecNormal->z;
		this->diff = diff;
		this->spec = spec;
		this->tu = tu;
		this->tv = tv;

		return;
	}

	//�n�_�A�I�_�A�@���x�N�g���A������4���_�̐����쐬
	void D3DVERTEX::CreateLine(D3DVERTEX *pOut, D3DXVECTOR3 *pvecBegin, D3DXVECTOR3 *pvecEnd, D3DXVECTOR3 *pvecNormal, float Width, DWORD diff, DWORD spec) 
	{
		D3DXVECTOR3 vecWidth; //�������̃x�N�g��
		D3DXVec3Cross(&vecWidth, &(*pvecEnd - *pvecBegin), pvecNormal); //�n�_����I�_�����ւ̃x�N�g���Ɩ@���x�N�g���̊O�ς����߂�
		D3DXVec3Normalize(&vecWidth, &vecWidth);
		vecWidth *= Width * 0.5f;

		//�n�_�ƏI�_�ɕ���������4�_���v�Z
		pOut[0].SetVertex(&(*pvecBegin + vecWidth), pvecNormal, diff, spec, 0, 0);
		pOut[1].SetVertex(&(*pvecEnd + vecWidth), pvecNormal, diff, spec, 1.0f, 0);
		pOut[2].SetVertex(&(*pvecBegin - vecWidth), pvecNormal, diff, spec, 0, 1.0f);
		pOut[3].SetVertex(&(*pvecEnd - vecWidth), pvecNormal, diff, spec, 1.0f, 1.0f);

		return;
	}







	const DWORD D3DVERTEX_2D::FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
	const D3DVERTEXELEMENT9 D3DVERTEX_2D::ve[] = {
		{0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0}, 
		{0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0}, 
		{0, 20, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0}, 
		D3DDECL_END() };

	//2D�̒��_�����Z�b�g
	void D3DVERTEX_2D::SetVertex(float x, float y, float z, DWORD diff, float tu, float tv) 
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->rhw = 1.0f;
		this->diff = diff;
		this->tu = tu;
		this->tv = tv;

		return;
	}

	//RECT�\���̂���4�̒��_����ݒ�
	void D3DVERTEX_2D::SetVertexFromRECT(D3DVERTEX_2D *lpVt, RECT *lpRect, float z, DWORD Color) 
	{
		lpVt[0].SetVertex((float)lpRect->left, (float)lpRect->top, z, Color, 0.0f, 0.0f); //����
		lpVt[1].SetVertex((float)lpRect->right, (float)lpRect->top, z, Color, 1.0f, 0.0f); //�E��
		lpVt[2].SetVertex((float)lpRect->left, (float)lpRect->bottom, z, Color, 0.0f, 1.0f); //����
		lpVt[3].SetVertex((float)lpRect->right, (float)lpRect->bottom, z, Color, 1.0f, 1.0f); //�E��
	
		return;
	}

	//���W��-0.5f���炷
	void D3DVERTEX_2D::Offset(D3DVERTEX_2D *pVt, int Number) 
	{
		for (int i = 0; i < Number; i++)
		{
			pVt[i].x -= 0.5f;
			pVt[i].y -= 0.5f;
		}

		return;
	}
}
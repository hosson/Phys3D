#include "ViewPortCulling.h"
#include "D3DVertex.h"

namespace ho
{
	//�R���X�g���N�^
	ViewPortCulling::ViewPortCulling(LPDIRECT3DDEVICE9 pD3DDevice)
	{
		this->pD3DDevice = pD3DDevice;
	}

	//�f�X�g���N�^
	ViewPortCulling::~ViewPortCulling()
	{
	}

	//��������X�V
	void ViewPortCulling::Update(float DrawDistance)
	{
		D3DVIEWPORT9 ViewPort; //�r���[�|�[�g
		pD3DDevice->GetViewport(&ViewPort); //�f�o�C�X�ɐݒ肳��Ă���r���[�|�[�g���擾

		//ViewPort����4�_�̍��W�𓾂�
		float x1 = (float)ViewPort.X;
		float y1 = (float)ViewPort.Y;
		float x2 = float(ViewPort.X + ViewPort.Width);
		float y2 = float(ViewPort.Y + ViewPort.Height);

		//�������8�_�̍��W
		//���̎��_�ł̓X�N���[����̍��W���i�[�����
		vecNear[0] = D3DXVECTOR3(x1, y1, ViewPort.MinZ); //����������
		vecNear[1] = D3DXVECTOR3(x2, y1, ViewPort.MinZ); //�E�����E��
		vecNear[2] = D3DXVECTOR3(x1, y2, ViewPort.MinZ); //���と����
		vecNear[3] = D3DXVECTOR3(x2, y2, ViewPort.MinZ); //�E�と�E��
		vecFar[0] = D3DXVECTOR3(x1, y1, ViewPort.MaxZ * DrawDistance); //����������
		vecFar[1] = D3DXVECTOR3(x2, y1, ViewPort.MaxZ * DrawDistance); //�E�����E��
		vecFar[2] = D3DXVECTOR3(x1, y2, ViewPort.MaxZ * DrawDistance); //���と����
		vecFar[3] = D3DXVECTOR3(x2, y2, ViewPort.MaxZ * DrawDistance); //�E�と�E��

		D3DXMATRIX mtrxProj; //���ˍs��
		pD3DDevice->GetTransform(D3DTS_PROJECTION, &mtrxProj); //�f�o�C�X���瓊�ˍs����擾

		D3DXMATRIX mtrxView; //�r���[�s��
		pD3DDevice->GetTransform(D3DTS_VIEW, &mtrxView); //�f�o�C�X����r���[�s����擾

		D3DXMATRIX mtrxWorld; //���[���h�s��
		D3DXMatrixIdentity(&mtrxWorld);

		//�X�N���[����̍��W�����[���h��ԏ�̍��W�ɕϊ�
		for (int i = 0; i < 4; i++)
		{
			D3DXVec3Unproject(&vecNear[i], &vecNear[i], &ViewPort, &mtrxProj, &mtrxView, &mtrxWorld);
			D3DXVec3Unproject(&vecFar[i], &vecFar[i], &ViewPort, &mtrxProj, &mtrxView, &mtrxWorld);
		}

		CalcNormal(); //�������8�_�̍��W����@���x�N�g�����v�Z

		return;
	}

	//�C�ӂ̃r���[�s��Ǝˉe�s��ɂ�莋������X�V
	void ViewPortCulling::Update(D3DXMATRIX *pmtrxView, D3DXMATRIX *pmtrxProj, float DrawDistance) 
	{
		D3DXMATRIX mtrxInvView, mtrxInvProj; //�r���[�s��Ǝˉe�s��̋t�s��
		D3DXMatrixInverse(&mtrxInvView, NULL, pmtrxView); //�r���[�s��̋t�s����v�Z
		D3DXMatrixInverse(&mtrxInvProj, NULL, pmtrxProj); //�ˉe�s��̋t�s����v�Z

		vecNear[0] = D3DXVECTOR3(-1.0f, 1.0f, 0.0f); //����
		vecNear[1] = D3DXVECTOR3(1.0f, 1.0f, 0.0f); //�E��
		vecNear[2] = D3DXVECTOR3(-1.0f, -1.0f, 0.0f); //����
		vecNear[3] = D3DXVECTOR3(1.0f, -1.0f, 0.0f); //�E��

		vecFar[0] = D3DXVECTOR3(-1.0f, 1.0f, 1.0f); //����
		vecFar[1] = D3DXVECTOR3(1.0f, 1.0f, 1.0f); //�E��
		vecFar[2] = D3DXVECTOR3(-1.0f, -1.0f, 1.0f); //����
		vecFar[3] = D3DXVECTOR3(1.0f, -1.0f, 1.0f); //�E��

		D3DXMATRIX mtrx = mtrxInvProj * mtrxInvView; //�ˉe�t�s��ƃr���[�t�s���ώZ

		//�ˉe��ԓ��̍��W�����[���h��ԓ��̍��W�ɕϊ�
		for (int i = 0; i < 4; i++)
		{
			D3DXVec3TransformCoord(&vecNear[i], &vecNear[i], &mtrx);
			D3DXVec3TransformCoord(&vecFar[i], &vecFar[i], &mtrx);
		}

		CalcNormal(); //�������8�_�̍��W����@���x�N�g�����v�Z

		return;
	}

	//�������8�_�̍��W����@���x�N�g�����v�Z
	void ViewPortCulling::CalcNormal()
	{
		//�������̖@���x�N�g��
		D3DXVec3Cross(&vecNFar, &(vecFar[1] - vecFar[0]), &(vecFar[0] - vecFar[2]));
		D3DXVec3Normalize(&vecNFar, &vecNFar);

		//��O�����̖@���x�N�g��
		D3DXVec3Cross(&vecNNear, &(vecNear[1] - vecNear[0]), &(vecNear[2] - vecNear[0]));
		D3DXVec3Normalize(&vecNNear, &vecNNear);

		//�������̖@���x�N�g��
		D3DXVec3Cross(&vecNLeft, &(vecNear[2] - vecNear[0]), &(vecFar[0] - vecNear[0]));
		D3DXVec3Normalize(&vecNLeft, &vecNLeft);

		//�E�����̖@���x�N�g��
		D3DXVec3Cross(&vecNRight, &(vecNear[0] - vecNear[2]), &(vecFar[1] - vecNear[1]));
		D3DXVec3Normalize(&vecNRight, &vecNRight);

		//������̖@���x�N�g��
		D3DXVec3Cross(&vecNTop, &(vecNear[0] - vecNear[1]), &(vecFar[0] - vecNear[0]));
		D3DXVec3Normalize(&vecNTop, &vecNTop);

		//�������̖@���x�N�g��
		D3DXVec3Cross(&vecNBottom, &(vecNear[1] - vecNear[0]), &(vecFar[2] - vecNear[2]));
		D3DXVec3Normalize(&vecNBottom, &vecNBottom);

		return;
	}


	//�J�����O����
	bool ViewPortCulling::Judge(::D3DXVECTOR3 &vecWorldPos, float Radius) 
	{
		//������̖@���x�N�g���Ɠ��O����
		if (D3DXVec3Dot(&(vecWorldPos - vecNear[0]), &vecNNear) - Radius > 0) //��O�̕ӂ��@���x�N�g���i���E�̊O�����j�Ɠ��������i���̐��j�̏ꍇ
			return false;
		if (D3DXVec3Dot(&(vecWorldPos - vecFar[0]), &vecNFar) - Radius > 0) //���ӂ��@���x�N�g���i���E�̊O�����j�Ɠ��������i���̐��j�̏ꍇ
			return false;
		if (D3DXVec3Dot(&(vecWorldPos - vecNear[0]), &vecNLeft) - Radius > 0) //���ӂ��@���x�N�g���i���E�̊O�����j�Ɠ��������i���̐��j�̏ꍇ
			return false;
		if (D3DXVec3Dot(&(vecWorldPos - vecNear[1]), &vecNRight) - Radius > 0) //�E�ӂ��@���x�N�g���i���E�̊O�����j�Ɠ��������i���̐��j�̏ꍇ
			return false;
		if (D3DXVec3Dot(&(vecWorldPos - vecNear[0]), &vecNTop) - Radius > 0) //��ӂ��@���x�N�g���i���E�̊O�����j�Ɠ��������i���̐��j�̏ꍇ
			return false;
		if (D3DXVec3Dot(&(vecWorldPos - vecNear[2]), &vecNBottom) - Radius > 0) //���ӂ��@���x�N�g���i���E�̊O�����j�Ɠ��������i���̐��j�̏ꍇ
			return false;

		return true; //�J�����O����Ȃ��i���E���ɓ���̂ŕ`��ΏۂƂȂ�j�ꍇ
	}

	//���݂̎�����J�����O�͈̔͂����C���[�t���[���ŕ`��
	void ViewPortCulling::DrawViewPortWireFrame(DWORD Color) 
	{
		/*���̊֐��͎�����J�����O�̊m�F�p�ɂ̂ݎg���B
		�܂��{���̃J�����ʒu���f�o�C�X�ɐݒ肵UpdateViewPortCulling()���Ăяo���B
		�����āA������𒭂߂����ʒu�ɃJ�������ړ����A�ʏ�ʂ�̎�����J�����O������s���Ȃ��炱�̊֐����Ăяo���B*/

		//ViewPortCulling�\���̂̏��𒸓_���ɕϊ�
		D3DVERTEX vt[8];
		for (int i = 0; i < 4; i++)
			vt[i].SetVertex(&vecFar[i], &D3DXVECTOR3(0, 0, 0), Color, Color, 0, 0);
		for (int i = 0; i < 4; i++)
			vt[4 + i].SetVertex(&vecNear[i], &D3DXVECTOR3(0, 0, 0), Color, Color, 0, 0);

		WORD Index[24] = { 0, 1, 1, 3, 3, 2, 2, 0, 0, 4, 1, 5, 2, 6, 3, 7, 4, 5, 5, 7, 7, 6, 6, 4};

		pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST, 0, 8, 12, Index, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX)); //�`��

		return;
	}
}
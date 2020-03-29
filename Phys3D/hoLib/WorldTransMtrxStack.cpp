#include "WorldTransMtrxStack.h"

namespace ho
{
	//���[���h�ϊ��s��̍X�V�J�n
	void WorldTransMtrxStack::UpdateFrame(D3DXFRAME_WORLD *pFrame) 
	{
		//�X�^�b�N������
		while (!stackMatrix.empty())
			stackMatrix.pop();

		listpDrawFrame.clear(); //�`��t���[�����X�g������
	
		stackMatrix.push(&WorldTransMatrix); //���[���h�ϊ��s����X�^�b�N�ɐς�

		CalcFrameWorldMatrix(pFrame); //���[�g�t���[�����烏�[���h�ϊ��s���A���v�Z

		return;
	}

	//�t���[���̃��[���h�s��v�Z�p�̍ċA�֐�
	void WorldTransMtrxStack::CalcFrameWorldMatrix(D3DXFRAME_WORLD *pFrame) 
	{
		//���݂̃X�^�b�N�擪�ɂ��郏�[���h�ϊ��s����Q��
		D3DXMATRIX *pStackMtrx = stackMatrix.top();

		//�����̃t���[���ɑΉ����郏�[���h�ϊ��s����v�Z
		D3DXMatrixMultiply(&pFrame->WorldTransMatrix, &pFrame->TransformationMatrix, pStackMtrx);

		//�t���[���Ƀ��b�V���R���e�i������΁A���̃t���[�������X�g�ɒǉ�����
		if (pFrame->pMeshContainer)
			listpDrawFrame.push_back(pFrame);

		//�q�t���[��������΃X�^�b�N��ς�ŁA�q�t���[���̃��[���h���W�ϊ��̌v�Z��
		if (pFrame->pFrameFirstChild)
		{
			stackMatrix.push(&pFrame->WorldTransMatrix); //�X�^�b�N�ɐς�
			CalcFrameWorldMatrix((D3DXFRAME_WORLD *)pFrame->pFrameFirstChild); //�q�t���[���̌v�Z
			stackMatrix.pop(); //�q�t���[���̌v�Z���I������̂ŃX�^�b�N��1�O��
		}

		//�Z��t���[��������ꍇ�͌��݂̃X�^�b�N���g���Čv�Z����
		if (pFrame->pFrameSibling)
			CalcFrameWorldMatrix((D3DXFRAME_WORLD *)pFrame->pFrameSibling);

		return;
	}
}

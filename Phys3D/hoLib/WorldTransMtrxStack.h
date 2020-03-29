#pragma once

#include <d3dx9.h>
#include <stack>
#include <list>
#include "MyAllocateHierarchy.h"

namespace ho
{
	class WorldTransMtrxStack
	{
	public:
		WorldTransMtrxStack(D3DXMATRIX *pWorldTransMatrix) { this->WorldTransMatrix = *pWorldTransMatrix; } //�R���X�g���N�^

		void UpdateFrame(D3DXFRAME_WORLD *pFrame); //���[���h�ϊ��s��̍X�V�J�n

		//�A�N�Z�b�T
		std::list<D3DXFRAME_WORLD *> *GetplistpDrawFrame() { return &listpDrawFrame; }
	private:
		D3DXMATRIX WorldTransMatrix; //�I�u�W�F�N�g���S�̃��[�J�����烏�[���h�֕ϊ�����s��
		std::stack<D3DXMATRIX *> stackMatrix; //���[���h�s��̃X�^�b�N
		std::list<D3DXFRAME_WORLD *> listpDrawFrame; //�`��t���[�����X�g

		void CalcFrameWorldMatrix(D3DXFRAME_WORLD *pFrame); //�t���[���̃��[���h�s��v�Z�p�̍ċA�֐�
	};
}

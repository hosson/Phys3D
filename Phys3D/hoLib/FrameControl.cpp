//�t���[�����[�g����N���X
#include "FrameControl.h"
#include <Windows.h>

namespace ho
{
	//�R���X�g���N�^
	FrameControl::FrameControl(const std::function<void()> &fUpdateObj, const std::function<void()> &fDrawObj)
		: fUpdateObj(fUpdateObj), fDrawObj(fDrawObj), ControlFirst(true)
	{
		if (!QueryPerformanceFrequency(&this->qpf)) //�n�[�h�E�F�A�� QueryPerformanceCounter ���T�|�[�g���Ă��Ȃ��ꍇ
			this->qpf.QuadPart = 0;
	}

	//�f�X�g���N�^
	FrameControl::~FrameControl()
	{
	}

	//�t���[���R���g���[��
	bool FrameControl::Control() 
	{
		QueryPerformanceCounter(&qpcNow); //���ݎ��Ԃ��擾

		if (this->ControlFirst) //�t���[���R���g���[���֐�������Ăяo���̏ꍇ
		{
			//���t���[���J�n���Ԃ��v�Z
			this->qpcNext.QuadPart= this->qpcNow.QuadPart + LONGLONG(this->qpf.QuadPart / 60.0);

			this->ControlFirst = false;
		}

		if (this->qpcNow.QuadPart >= this->qpcNext.QuadPart) //���t���[���̏������Ԃ��߂��Ă����ꍇ
		{
			this->fUpdateObj(); //�X�V�֐����s
			this->fDrawObj(); //�`��֐����s

			this->qpcNext.QuadPart += LONGLONG(this->qpf.QuadPart / 60.0); //���t���[���̎��Ԃ��X�V
		} else {
			Sleep(1);
		}

		return false;
	}
}
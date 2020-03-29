//�t���[�����[�g����N���X
#pragma once
#include <functional>
#include <Windows.h>

namespace ho
{
	class FrameControl
	{
	public:
		FrameControl(const std::function<void()> &fUpdateObj, const std::function<void()> &fDrawObj); //�R���X�g���N�^
		virtual ~FrameControl(); //�f�X�g���N�^

		virtual bool Control(); //�t���[�����䏈��
	protected:
	private:
		const std::function<void()> fUpdateObj; //�X�V�֐�
		const std::function<void()> fDrawObj; //�`��֐�
		bool ControlFirst; //Control �֐�������Ăяo�����ǂ���
		LARGE_INTEGER qpf; //QueryPerformanceCounter �� 1 �b������̃J�E���g��
		LARGE_INTEGER qpcNow; //���݂̎���
		LARGE_INTEGER qpcNext; //���̃t���[�����J�n���鎞��
	};
}

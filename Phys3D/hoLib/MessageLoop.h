//���b�Z�[�W���[�v�N���X

#pragma once
#include <functional>

namespace ho
{
	class MessageLoop
	{
	public:
		MessageLoop(const std::function<bool()> &fFrameControlObj); //�R���X�g���N�^
		~MessageLoop() {} //�f�X�g���N�^

		int Run(); //���b�Z�[�W���[�v���s
	private:
		const std::function<bool()> fFrameControlObj; //�t���[������֐�
	};
}

/*
WinMain���̃��b�Z�[�W���[�v����������N���X

�ʏ�A�v���P�[�V�����̏ꍇ�̓R���X�g���N�^��
std::function<bool()>()
��n���B

�t���[������֐������ރA�v���P�[�V�����̏ꍇ�́A
�R���X�g���N�^�Ƀt���[������֐��̃I�u�W�F�N�g��n���B
*/
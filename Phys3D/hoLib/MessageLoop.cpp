#include <windows.h>
#include "MessageLoop.h"

namespace ho
{
	//�R���X�g���N�^
	MessageLoop::MessageLoop(const std::function<bool()> &fFrameControlObj)
		: fFrameControlObj(fFrameControlObj)
	{
	}

	//���b�Z�[�W���[�v���s
	int MessageLoop::Run()
	{
		MSG msg;

		if (this->fFrameControlObj._Empty()) //�t���[������֐����ݒ肳��Ă��Ȃ��ꍇ
		{
			//�ʏ�A�v���P�[�V�����̃��[�v
			while (GetMessage(&msg, NULL, 0, 0)) //WM_QUITE��������܂ŌJ��Ԃ�
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} else {
			//�t���[����������ރA�v���P�[�V�����̃��[�v
			while (true)
			{
				if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) //���b�Z�[�W������ꍇ
				{
					if (!GetMessage(&msg, NULL, 0, 0)) //���b�Z�[�W��WM_QUIT�̏ꍇ
						break;
					TranslateMessage(&msg); //�L�[�{�[�h�𗘗p�\�ɂ���
					DispatchMessage(&msg); //�����Windows�ɖ߂�
				} else {
					if (!this->fFrameControlObj._Empty()) //�t���[������֐����ݒ肳��Ă���ꍇ
						if (this->fFrameControlObj()) //�t���[������֐������s���A�I������ꍇ
							break;
				}
			}
		}

		return msg.wParam;
	}
}

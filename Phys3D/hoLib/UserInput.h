//�L�[�{�[�h�ƃ}�E�X�̓��̓N���X
#pragma once
#include <Windows.h>
#include "PT.h"
#include "DirectInput.h"

namespace ho
{
	class UserInput
	{
	public:
		struct MOUSESTATE //�}�E�X��ԍ\����
		{
			ho::PT<int> ptPos; //�J�[�\���ʒu
			BOOL LeftButton; //���{�^��
			BOOL RightButton; //�E�{�^��
			int MouseWheel; //�}�E�X�z�C�[���̈ʒu
		};
	public:
		UserInput(HWND hWnd, HINSTANCE hInstance); //�R���X�g���N�^
		~UserInput(); //�f�X�g���N�^

		void FrameProcess(); //1�t���[�����̏���
		void WM_MouseWheel(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); //WM_MOUSEWHEEL���Ăяo���ꂽ�Ƃ�

		//�A�N�Z�b�T
		BYTE *GetpKeyState() { return KeyState[0]; }
		MOUSESTATE *GetpMouseState() { return MouseState; }
		int GetFrameFlag() { return FrameFlag; }
		void SetFullScreen(bool b) { this->FullScreen = b; }
		DirectInput *GetpDirectInputObj() { return pDirectInputObj; }
	private:
		HWND hWnd; //���͑ΏۂƂ���E�B���h�E�n���h��
		int FrameFlag; //0��1�����݂ɌJ��Ԃ�
		BYTE KeyState[2][256]; //�L�[�{�[�h�̏��
		MOUSESTATE MouseState[2]; //�}�E�X�̏��
		int MouseWheel; //�}�E�X�z�C�[���̈ړ���
		bool FullScreen; //�t���X�N���[�����[�h���ǂ���
		DirectInput *pDirectInputObj; //DirectInput �I�u�W�F�N�g�ւ̃|�C���^
	};
}

/*�L�[�{�[�h�ƃ}�E�X�̏�Ԃ��擾����N���X�B
1�t���[������FrameProcess()���Ăяo���A���͏�Ԃ𓾂����Ƃ���
GetpKeyState()���g���BGetFrameFlag()��KeyState[0]��KeyState[1]��
�ǂ���ɍŐV�̓��͂������Ă��邩�𓾂���B*/

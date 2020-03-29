#include "UserInput.h"
#include "Debug.h"
#include "hoLib.h"
#include "DirectInput.h"

namespace ho
{
	//�R���X�g���N�^
	UserInput::UserInput(HWND hWnd, HINSTANCE hInstance) //�R���X�g���N�^
	{
		this->hWnd = hWnd;
		this->pDirectInputObj = new DirectInput(hInstance, hWnd); //DirectInput �I�u�W�F�N�g���쐬
		FrameFlag = 0;
		this->FullScreen = false;

		FrameProcess(); //1�t���[���O����͂��Ă���
	}

	//�f�X�g���N�^
	UserInput::~UserInput() 
	{
		SDELETE(pDirectInputObj); //DirectInput �I�u�W�F�N�g���폜
	}

	//1�t���[�����̏���
	void UserInput::FrameProcess() 
	{
		FrameFlag = 1 - FrameFlag;

		GetKeyboardState(KeyState[FrameFlag]); //�L�[�{�[�h�̏�Ԃ𓾂�

		//�}�E�X�ʒu���擾
		POINT pt;
		GetCursorPos(&pt);
		if (!FullScreen) //�t���X�N���[�����[�h�ł͂Ȃ��ꍇ
			ScreenToClient(hWnd, &pt); //�E�B���h�E�̃N���C�A���g�̈���̍��W�ɕϊ�
		MouseState[FrameFlag].ptPos.x = pt.x;
		MouseState[FrameFlag].ptPos.y = pt.y;
		MouseState[FrameFlag].LeftButton = KeyState[FrameFlag][VK_LBUTTON] & 0x80;
		MouseState[FrameFlag].RightButton = KeyState[FrameFlag][VK_RBUTTON] & 0x80;
		MouseState[FrameFlag].MouseWheel = MouseWheel;
		MouseWheel = 0;


		return;
	}

	//WM_MOUSEWHEEL���Ăяo���ꂽ�Ƃ�
	void UserInput::WM_MouseWheel(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
	{
		MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA; //�}�E�X�z�C�[���ړ��ʂ����Z

		return;
	}
}

//�E�B���h�E��̃R���g���[�����Ǘ�����N���X

#pragma once
#include <Windows.h>
#include <vector>

namespace ho {
	class ControlManager
	{
	public:
		struct CONTROLINFO //�e�R���g���[���Ɋւ�����
		{
			HWND hWnd; //�R���g���[���̃E�B���h�E�n���h��
			FARPROC OldProc; //�T�u�N���X�����ɏ�����Ԃ̃v���V�[�W����ۑ�����ϐ�
		};

		ControlManager() {}
		BOOL RegisthWnd(int Number, HWND hControl); //�C�ӂ̊Ǘ��ԍ��ɃR���g���[���̃E�B���h�E�n���h����o�^
		BOOL RegistOldProc(int Number, FARPROC OldProc); //�C�ӂ̊Ǘ��ԍ��ɃR���g���[���̏�����Ԃ̃E�B���h�E�v���V�[�W����o�^
		HWND GethWnd(int Number); //�C�ӂ̊Ǘ��ԍ��̃R���g���[���̃E�B���h�E�n���h�����擾
		FARPROC GetOldProc(int Number); //�C�ӂ̊Ǘ��ԍ��̏�����Ԃ̃E�B���h�E�v���V�[�W�����擾
		void SetFontSize(int Height); //�S�ẴR���g���[���̃t�H���g�T�C�Y���ꊇ�ݒ�
	private:
		std::vector<CONTROLINFO> vectorCI; //�e�R���g���[���̃E�B���h�E�n���h���z��
	};
}

/*�E�B���h�E��̃R���g���[�����Ǘ�����N���X�B
�Ǘ��ԍ��Ŋe�R���g���[�������ʂ��A�R���g���[���̃E�B���h�E�n���h����
�f�t�H���g�̃E�B���h�E�v���V�[�W���Ȃǂ�o�^�E�Q�Ƃ��邱�Ƃ��ł���B*/

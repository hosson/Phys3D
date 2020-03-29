#include "ControlManager.h"
#include <WindowsX.h>

namespace ho {
	//�C�ӂ̊Ǘ��ԍ��ɃR���g���[���̃E�B���h�E�n���h����o�^
	BOOL ControlManager::RegisthWnd(int Number, HWND hControl)
	{
		if (Number < (signed)vectorCI.size()) //�ǉ�����R���g���[���̊Ǘ��ԍ����z��̃T�C�Y���̏ꍇ
		{
			if (!vectorCI.at(Number).hWnd) //�ǉ�����R���g���[���̊Ǘ��ԍ��ɂ͉����o�^����Ă��Ȃ��ꍇ
			{
				vectorCI.at(Number).hWnd = hControl;
			} else { //���ɕʂ̃R���g���[���̃E�B���h�E�n���h�����o�^����Ă���ꍇ
				return FALSE; //���s
			}
		} else { //�ǉ�����R���g���[���̊Ǘ��ԍ����z��̃T�C�Y���傫���ꍇ
			CONTROLINFO CI;
			ZeroMemory(&CI, sizeof(CONTROLINFO));
			vectorCI.resize(Number + 1, CI); //�z��̃T�C�Y���g��
			vectorCI.at(Number).hWnd = hControl;
		}

		return TRUE;
	}

	//�C�ӂ̊Ǘ��ԍ��ɃR���g���[���̏�����Ԃ̃E�B���h�E�v���V�[�W����o�^
	BOOL ControlManager::RegistOldProc(int Number, FARPROC OldProc)
	{
		if (Number < (signed)vectorCI.size()) //�ǉ�����R���g���[���̊Ǘ��ԍ����z��̃T�C�Y���̏ꍇ
		{
			if (!vectorCI.at(Number).OldProc) //�ǉ�����R���g���[���̊Ǘ��ԍ��ɂ͉����o�^����Ă��Ȃ��ꍇ
			{
				vectorCI.at(Number).OldProc = OldProc;
			} else { //���ɕʂ̃R���g���[���̃E�B���h�E�v���V�[�W�����o�^����Ă���ꍇ
				return FALSE; //���s
			}
		} else { //�ǉ�����R���g���[���̊Ǘ��ԍ����z��̃T�C�Y���傫���ꍇ
			CONTROLINFO CI;
			ZeroMemory(&CI, sizeof(CONTROLINFO));
			vectorCI.resize(Number + 1, CI); //�z��̃T�C�Y���g��
			vectorCI.at(Number).OldProc = OldProc;
		}

		return TRUE;
	}


	//�C�ӂ̊Ǘ��ԍ��̃R���g���[���̃E�B���h�E�n���h�����擾
	HWND ControlManager::GethWnd(int Number)
	{
		if (Number < (signed)vectorCI.size()) //�ǉ�����R���g���[���̊Ǘ��ԍ����z��̃T�C�Y���̏ꍇ
			return vectorCI.at(Number).hWnd;

		return NULL;
	}

	//�C�ӂ̊Ǘ��ԍ��̏�����Ԃ̃E�B���h�E�v���V�[�W�����擾
	FARPROC ControlManager::GetOldProc(int Number)
	{
		if (Number < (signed)vectorCI.size()) //�ǉ�����R���g���[���̊Ǘ��ԍ����z��̃T�C�Y���̏ꍇ
			return vectorCI.at(Number).OldProc;

		return NULL;
	}


	//�S�ẴR���g���[���̃t�H���g�T�C�Y���ꊇ�ݒ�
	void ControlManager::SetFontSize(int Height) 
	{
		//�t�H���g���쐬
		LOGFONT logFont;
		HFONT hFont;
		ZeroMemory(&logFont, sizeof(LOGFONT));
		logFont.lfHeight = Height;
		hFont = CreateFontIndirect(&logFont);

		for (unsigned int i = 0; i < vectorCI.size(); i++)
			SetWindowFont(vectorCI.at(i).hWnd, hFont, TRUE);

		return;
	}
}

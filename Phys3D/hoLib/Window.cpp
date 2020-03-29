//�E�B���h�E�N���X

#pragma once
#include "Window.h"
#include "Common.h"

namespace ho
{
	//�R���X�g���N�^
	WindowManager::WindowManager()
	{
	}

	//�f�X�g���N�^
	WindowManager::~WindowManager()
	{
		//�S�Ă�Window�I�u�W�F�N�g���폜
		for (int i = 0; i < (signed)vectorpWindowObj.size(); i++)
			SDELETE(vectorpWindowObj.at(i));
	}

	//�E�B���h�E�ǉ�
	bool WindowManager::AddWnd(int Number, Window *pWindowObj)
	{
		if (Number < (signed)vectorpWindowObj.size()) //�ǉ�����E�B���h�E�̊Ǘ��ԍ����z��̃T�C�Y���̏ꍇ
		{
			if (!vectorpWindowObj.at(Number)) //�ǉ�����E�B���h�E�̊Ǘ��ԍ��ɂ͉����o�^����Ă��Ȃ��ꍇ
				vectorpWindowObj.at(Number) = pWindowObj;
			else //���ɕʂ̃E�B���h�E���o�^����Ă���ꍇ
				return FALSE; //���s
		} else { //�ǉ�����E�B���h�E�̊Ǘ��ԍ����z��̃T�C�Y���傫���ꍇ
			vectorpWindowObj.resize(Number + 1, NULL); //�z��̃T�C�Y���g��
			vectorpWindowObj.at(Number) = pWindowObj;
		}

		return TRUE;
	}

	//�C�ӂ̔ԍ��̃E�B���h�E�I�u�W�F�N�g�ւ̃|�C���^���擾
	Window *WindowManager::GetpWindowObj(int Number) 
	{
		if (vectorpWindowObj.size() <= (unsigned)Number) //�z��̃T�C�Y���͈͊O�������ꍇ
			return NULL;
	
		return vectorpWindowObj.at(Number);
	}







	//�E�B���h�E����
	BOOL Window::CreateWnd(WNDCLASSEX *lpWC, CREATEWINDOWEX *lpCW, int nWinMode) 
	{
		//�E�B���h�E�N���X��o�^����
		if (RegisterClassEx(lpWC))
		{
			//�E�B���h�E�N���X�̓o�^���ł����̂ŁA�E�B���h�E�𐶐�����
			hWnd = CreateWindowExIndirect(lpCW);
			if (!hWnd)
				return FALSE;

			SetProp(hWnd, TEXT("__WINPROC__"), this); //�E�B���h�E�փv���p�e�B��ǉ����ăI�u�W�F�N�g���֘A�t����

			ShowWindow(hWnd, nWinMode); //�E�B���h�E��\������
			UpdateWindow(hWnd);
		} else {
			return FALSE;
		}

		return TRUE;
	}

	//�R���X�g���N�^�i�ȈՍ쐬�j
	Window::Window(HINSTANCE hInstance, TCHAR *pCaption, TCHAR *pClassName, int Width, int Height)
	{
		WNDCLASSEX wc;

		//�E�B���h�E�N���X���`����
		ZeroMemory(&wc, sizeof(WNDCLASSEX));
		wc.hInstance = hInstance;								//���̃C���X�^���X�ւ̃n���h��
		wc.lpszClassName = pClassName;							//�E�B���h�E�N���X��
		wc.lpfnWndProc = Window::WndProc;						//�E�B���h�E�v���V�[�W���֐�
		wc.style = 0;											//�E�B���h�E�X�^�C���i�f�t�H���g�j
		wc.cbSize = sizeof(WNDCLASSEX);							//WNDCLASSEX�\���̂̃T�C�Y��ݒ�
		wc.hIcon = LoadIcon(hInstance, IDI_WINLOGO);			//���[�W�A�C�R��
		wc.hIconSm = LoadIcon(hInstance, IDI_WINLOGO);			//�X���[���A�C�R��
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);				//�J�[�\���X�^�C��
		wc.lpszMenuName = NULL;									//���j���[
		wc.cbClsExtra = 0;										//�G�L�X�g��
		wc.cbWndExtra = 0;										//�K�v�ȏ��
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);	//�E�B���h�E�̔w�i

		RECT rect, rcDesktop;

		GetWindowRect(GetDesktopWindow(), &rcDesktop); //�f�X�N�g�b�v�̑傫�����擾
		SetRect(&rect, 0, 0, Width, Height);
		AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0);

		CREATEWINDOWEX cw;

		ZeroMemory(&cw, sizeof(CREATEWINDOWEX));
		cw.dwExStyle = 0;								//�g���E�B���h�E�X�^�C��
		cw.lpClassName = wc.lpszClassName;				//�E�B���h�E�N���X�̖��O
		cw.lpWindowName	= pCaption;						//�E�B���h�E�^�C�g��
		cw.dwStyle = WS_OVERLAPPEDWINDOW;				//�E�B���h�E�X�^�C��
		cw.x = (rcDesktop.right - rect.right) / 2;		// �E�B���h�E�̍��p�w���W
		cw.y = (rcDesktop.bottom - rect.bottom) / 2;	// �E�B���h�E�̍��p�x���W
		cw.nWidth = rect.right - rect.left;				// �E�B���h�E�̕�
		cw.nHeight = rect.bottom - rect.top;			// �E�B���h�E�̍���
		cw.hWndParent =	NULL;							//�e�E�B���h�E
		cw.hMenu = 	NULL;								//���j���[
		cw.hInstance = hInstance;						//���̃v���O�����̃C���X�^���X�̃n���h��
		cw.lpParam = NULL;								//�ǉ�����	
			
		if (!CreateWnd(&wc, &cw, SW_SHOWNORMAL)) //�E�B���h�E����
		{
			//�E�B���h�E�����Ɏ��s�����ꍇ�̏���������
		}
	}

	//�R���X�g���N�^�i�ڍ׍쐬�j
	Window::Window(WNDCLASSEX *pWCE, CREATEWINDOWEX *pCWE) 
	{
		CreateWnd(pWCE, pCWE, SW_SHOWNORMAL); //�E�B���h�E����
	}

	//CreateWindowEx�̃��b�p�[�֐�
	HWND Window::CreateWindowExIndirect(CREATEWINDOWEX *lpCW) 
	{
		return CreateWindowEx(
			lpCW->dwExStyle,	//�g���E�B���h�E�X�^�C��
			lpCW->lpClassName,	//�E�B���h�E�N���X�̖��O
			lpCW->lpWindowName,	//�E�B���h�E�^�C�g��
			lpCW->dwStyle,		//�E�B���h�E�X�^�C��
			lpCW->x,			//�E�B���h�E�̍��p�w���W
			lpCW->y,			//�E�B���h�E�̍��p�x���W
			lpCW->nWidth,		//�E�B���h�E�̕�
			lpCW->nHeight,		//�E�B���h�E�̍���
			lpCW->hWndParent,	//�e�E�B���h�E
			lpCW->hMenu,		//���j���[
			lpCW->hInstance,	//���̃v���O�����̃C���X�^���X�̃n���h��
			lpCW->lpParam		//�ǉ�����
		);
	}

	//�f�X�g���N�^
	Window::~Window()
	{
		if (hWnd)
			SendMessage(hWnd, WM_DESTROY, 0, 0); //�E�B���h�E�����
	}

	//�E�B���h�E�v���V�[�W���֐�
	LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
	{
		Window *lpObj = (Window *)GetProp(hWnd, TEXT("__WINPROC__")); //�E�B���h�E�n���h������֘A�t����ꂽ�I�u�W�F�N�g���擾

		if (lpObj) //�I�u�W�F�N�g���擾�ł����ꍇ
			return lpObj->LocalWndProc(hWnd, msg, wParam, lParam);

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	//�I�[�o�[���C�h�p�E�B���h�E�v���V�[�W��
	LRESULT Window::LocalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
	{
		switch (msg)
		{
		case WM_DESTROY: //����{�^�����N���b�N������
			RemoveProp(hWnd, TEXT("__WINPROC__")); //�֘A�t����Ă����v���p�e�B���폜
			PostQuitMessage(0); //WM_QUIT���b�Z�[�W�𔭍s
			break;
		default: //��L�ȊO�̃��b�Z�[�W��Windows�֏�����C����
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	//�����̈�̍쐬�i�`��X�V�j
	void Window::MakeInvalidateRect() 
	{
		InvalidateRect(hWnd, NULL, TRUE);

		return;
	}
}

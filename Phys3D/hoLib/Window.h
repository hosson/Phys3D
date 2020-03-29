//�E�B���h�E�N���X
#pragma once
#include <windows.h>
#include <vector>

namespace ho
{
	//�O���錾
	class Window;

	//�E�B���h�E�N���X���Ǘ�����N���X
	class WindowManager
	{
	public:
		WindowManager(); //�R���X�g���N�^
		~WindowManager(); //�f�X�g���N�^
		bool AddWnd(int Number, Window *pWindowObj); //�E�B���h�E�ǉ�
		Window *GetpWindowObj(int Number); //�C�ӂ̔ԍ��̃E�B���h�E�I�u�W�F�N�g�ւ̃|�C���^���擾
	private:
		std::vector<Window *> vectorpWindowObj; //�E�B���h�E�I�u�W�F�N�g�ւ̃|�C���^�̔z��
	};

	//�E�B���h�E�N���X
	class Window
	{
	public:
		struct CREATEWINDOWEX
		{
			unsigned int dwExStyle;		// �g���E�B���h�E�X�^�C��
			LPCTSTR lpClassName;	// �o�^����Ă���N���X��
			LPCTSTR lpWindowName;	// �E�B���h�E��
			unsigned int dwStyle;			// �E�B���h�E�X�^�C��
			int x;					// �E�B���h�E�̉������̈ʒu
			int y;					// �E�B���h�E�̏c�����̈ʒu
			int nWidth;				// �E�B���h�E�̕�
			int nHeight;			// �E�B���h�E�̍���
			HWND hWndParent;		// �e�E�B���h�E�܂��̓I�[�i�[�E�B���h�E�̃n���h��
			HMENU hMenu;			// ���j���[�n���h���܂��͎q���ʎq
			HINSTANCE hInstance;	// �A�v���P�[�V�����C���X�^���X�̃n���h��
			LPVOID lpParam;			// �E�B���h�E�쐬�f�[�^
		};

		Window(HINSTANCE hInstance, TCHAR *pCaption, TCHAR *pClassName, int Width = 640, int Height = 480); //�R���X�g���N�^�i�ȈՍ쐬�j
		Window(WNDCLASSEX *pWCE, CREATEWINDOWEX *pCWE); //�R���X�g���N�^�i�ڍ׍쐬�j
		Window() {} //�R���X�g���N�^�i�������Ȃ��j
		virtual ~Window(); //�f�X�g���N�^

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); //�E�B���h�E�v���V�[�W��
		void MakeInvalidateRect(); //�����̈�̍쐬�i�`��X�V�j
		HWND GethWnd() { return hWnd; } //�E�B���h�E�n���h�����擾
	protected:
		HWND hWnd;

		BOOL CreateWnd(WNDCLASSEX *lpWC, CREATEWINDOWEX *lpCW, int nWinMode); //�E�B���h�E����
		HWND CreateWindowExIndirect(CREATEWINDOWEX *); //CreateWindowEx�̃��b�p�[�֐�
		virtual LRESULT LocalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); //�I�[�o�[���C�h�p�E�B���h�E�v���V�[�W��
		virtual void Init() {} //�E�B���h�E���쐬���ꂽ���̏����������i�I�[�o�[���C�h�p�j
	};
}

/*�v���O�����S�̂ŃO���[�o���ȃE�B���h�E�Ǘ��N���X�iWindowManager�j������B

�E�B���h�E���쐬����ɂ�Window�N���X�̃I�u�W�F�N�g�����A���̃|�C���^��WindowManager::AddWnd()�œo�^����B
AddWnd()�̈�����Number�ŌX�̃E�B���h�E�͎��ʂ����̂ŁA�����ԍ��ɕ����̃E�B���h�E����邱�Ƃ͏o���Ȃ��B
*/

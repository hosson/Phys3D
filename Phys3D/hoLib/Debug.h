//�f�o�b�O�N���X
#pragma once
#include <Windows.h>
#include <vector>
#include "tstring.h"
#include "LibError.h"

class App;

//�G���[�o�̓}�N��
#define ERR(Msg, File, Line, Continue) ho::Debug::pObj->Throw(Msg, File, Line, Continue, ho::Debug::Error);
#define ERRLOG(Msg, File, Line, Continue) ho::Debug::pObj->Throw(Msg, File, Line, Continue, ho::Debug::Log);

namespace ho {
	class Debug : public ErrorReceiver
	{
	public:
		enum eOutputType
		{
			Log,	//�ʏ탍�O
			Error	//�G���[
		};
	public:
		Debug(::App *const pAppObj, HWND hWnd, bool Console, bool Txt, const SYSTEMTIME &st, const bool PlayBeep); //�R���X�g���N�^
		~Debug(); //�f�X�g���N�^

		void Throw(const TCHAR *pMsg, const TCHAR *pFile, const int Line, const bool Continue, eOutputType OutputType); //�o��
		void Receive(const TCHAR *pMsg, const TCHAR *pFile, const int Line, const bool Continue); //���C�u��������G���[���󂯎��
		static Debug *pObj;

		void TimerBegin(DWORD Index); //���Ԍv���J�n
		void TimerEnd(DWORD Index); //���Ԍv���I��
		double GetTimerTime(DWORD Index); //�v�����Ԃ��擾
	private:
		::App *const pAppObj;
		bool Console; //�R���\�[�����g�����ǂ���
		bool Txt; //�e�L�X�g�t�@�C���֏o�͂��邩�ǂ���
		bool PlayBeep; //�G���[����Beep��炷���ǂ���
		tstring strFilename; //���O�o�͗p�̃t�@�C����
		HANDLE hFile; //�t�@�C���n���h��
		HWND hWnd; //���s�s�\�ȃG���[�������Ƀ��b�Z�[�W�{�b�N�X���o���E�B���h�E�̃n���h��
		DWORD BeginTime; //�f�o�b�O�V�X�e���N������timeGetTime()�œ���ꂽ����
		DWORD LastBeepTime; //�Ō��Beep���炳�ꂽ����
		LARGE_INTEGER qpf; //QueryPerformanceFrequency()�œ����镪��\
		std::vector<double> vectorTime; //�^�C�}�[�Ōv����������
		std::vector<LARGE_INTEGER> vectorqpc; //�v���J�n����

		void Finalize(bool Normal); //�I������
	};

}

/*�G���[���b�Z�[�W��G���[���O�A���샍�O�̏o�͂Ȃǂ̃f�o�b�O�����p�N���X�B
*/

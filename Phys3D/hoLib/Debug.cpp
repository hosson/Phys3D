#include "Debug.h"
#include <conio.h>
#include <tchar.h>
#include "Common.h"
#include "hoLib.h"
#include <stdlib.h>
#include "CGI_Upload.h"
#include "../Version.h"
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include "..\App.h"
#include "../Config.h"

namespace ho {
	Debug *Debug::pObj;

	//�R���X�g���N�^
	Debug::Debug(::App *const pAppObj, HWND hWnd, bool Console, bool Txt, const SYSTEMTIME &st, const bool PlayBeep)
		: pAppObj(pAppObj), PlayBeep(PlayBeep)
	{
		//�����ƃo�[�W��������t�@�C������ݒ�
		ho::tstring Filename;
		ho::tstring ver = _VERSION;
		Filename.push_back(ver.at(0));
		Filename.push_back(ver.at(2));
		Filename.push_back(ver.at(3));
		if (_TRIAL_TF) //�̌��ł̏ꍇ
			Filename.push_back(TEXT('t'));
		else //���i�ł̏ꍇ
			Filename.push_back(TEXT('p'));
		Filename.push_back(TEXT('_'));
		ho::tPrint(this->strFilename, TEXT("%04d%02d%02d%02d%02d%02d.txt"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		this->strFilename = Filename + this->strFilename;

		pObj = this;
		BeginTime = timeGetTime(); //�N�����̎��Ԃ��擾
		LastBeepTime = BeginTime;

		this->hWnd = hWnd;

		this->Console = Console;
		if (Console)
			AllocConsole(); //�R���\�[���쐬

		this->Txt = Txt;
		if (Txt)
		{
			if (!PathIsDirectory(TEXT("Log"))) //Log �t�H���_�����݂��Ȃ��ꍇ
			{
				if (!CreateDirectory(TEXT("Log"), NULL)) //�f�B���N�g�����쐬���A���s�����ꍇ
				{
					ho::tstring strError;
					ho::LibError::GetLastErrorStr(strError); //�G���[������擾
					throw ho::Exception(strError.c_str(), __WFILE__, __LINE__, true);
				}
			}

			if (!SetCurrentDirectory(TEXT("Log"))) //�J�����g�f�B���N�g����Log�t�H���_�ֈړ�
			{
				ho::tstring strError;
				ho::LibError::GetLastErrorStr(strError); //�G���[������擾
				throw ho::Exception(strError.c_str(), __WFILE__, __LINE__, true);
			}

			hFile = CreateFile(this->strFilename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL); //�t�@�C���쐬
			if (hFile == INVALID_HANDLE_VALUE) //�t�@�C���쐬�Ɏ��s�����ꍇ
			{
				ER(TEXT("�G���[���O�p�̃t�@�C�����J���܂���ł����B"), __WFILE__, __LINE__, true);

				ho::tstring str;
				LibError::GetLastErrorStr(str); //�G���[������擾
				ER(str.c_str(), __WFILE__, __LINE__, true); //�G���[�o��

				this->Txt = false;
			} else {
				unsigned char BOM[2] = { 0xFF, 0xFE };
				DWORD Bytes;
				WriteFile(hFile, BOM, 2, &Bytes, NULL);

				Throw(TEXT("�G���[���O�t�@�C�����J���܂����B"), __WFILE__, __LINE__, true, Log);
			}

			if (!SetCurrentDirectory(TEXT("..\\"))) //�J�����g�f�B���N�g�������ɖ߂�
			{
				ho::tstring strError;
				ho::LibError::GetLastErrorStr(strError); //�G���[������擾
				throw ho::Exception(strError.c_str(), __WFILE__, __LINE__, true);
			}
		}

		QueryPerformanceFrequency(&qpf);
	}

	//�f�X�g���N�^
	Debug::~Debug() 
	{
		Finalize(true); //�I������
	}

	//�I������
	void Debug::Finalize(bool Normal) 
	{
		if (Normal) //����I���̏ꍇ
			Throw(TEXT("�f�o�b�O�V�X�e���͐���ɏI�����܂��B"), __WFILE__, __LINE__, true, Log);
		else
			Throw(TEXT("���v���O���������s�ł��Ȃ��G���[�������������߁A�f�o�b�O�V�X�e���ɂ����s�����f����܂����B"), __WFILE__, __LINE__, true, Log);

		if (Console)
			FreeConsole(); //�R���\�[���폜
		if (Txt)
		{
			Throw(TEXT("�G���[���O�t�@�C������鏈�����J�n���܂��B"), __WFILE__, __LINE__, true, Log);
			if (CloseHandle(hFile)) //�t�@�C���n���h�������
			{
				Throw(TEXT("�G���[���O�t�@�C������܂����B"), __WFILE__, __LINE__, true, Log);
			} else {
				ER(TEXT("�G���[���O�p�̃t�@�C�������̂Ɏ��s���܂����B"), __WFILE__, __LINE__, true);

				ho::tstring str;
				LibError::GetLastErrorStr(str); //�G���[������擾
				ER(str.c_str(), __WFILE__, __LINE__, true); //�G���[�o��
			}

			if (!SetCurrentDirectory(TEXT("Log"))) //�J�����g�f�B���N�g����Log�t�H���_�ֈړ�
			{
				ho::tstring strError;
				ho::LibError::GetLastErrorStr(strError); //�G���[������擾
				throw ho::Exception(strError.c_str(), __WFILE__, __LINE__, true);
			}

			if (!this->pAppObj->GetpConfigObj()->Getnonetwork()) //�l�b�g���[�N���M���Ȃ��ݒ�ł͂Ȃ��ꍇ
			{
				//�G���[���O���T�[�o�ɃA�b�v���[�h
				CGI_Upload CGI_UploadObj("vertex.matrix.jp");
				std::string strVer = ho::WCharToChar(_VERSION); //Unicode�̃o�[�W�����������char�ɕϊ�
				CGI_UploadObj.Upload(this->strFilename, strVer);
			}

			if (!SetCurrentDirectory(TEXT("..\\"))) //�J�����g�f�B���N�g�������ɖ߂�
			{
				ho::tstring strError;
				ho::LibError::GetLastErrorStr(strError); //�G���[������擾
				throw ho::Exception(strError.c_str(), __WFILE__, __LINE__, true);
			}
		}

		return;
	}

	//�o��
	void Debug::Throw(const TCHAR *pMsg, const TCHAR *pFile, const int Line, const bool Continue, eOutputType OutputType)
	{
		//�o�͗p�̕�������쐬
		tstring str;
		ho::tPrint(str, TEXT("[%d] %s %s (%d)\n"), timeGetTime() - BeginTime, pMsg, pFile, Line);

		if (Console) //�R���\�[���֏o�͂���ݒ�̏ꍇ
		{
			_cputts(str.c_str()); //���o��
			OutputDebugString(str.c_str()); //VisualStudio�ɏo��

			if (OutputType == Error) //�G���[�̏ꍇ
			{
				if (LastBeepTime - timeGetTime() > 1000) //�Ō��Beep���炳��Ă���1�b�ȏ�o�߂��Ă���ꍇ
				{
					if (this->PlayBeep) //Beep����炷�ݒ�̏ꍇ
						Beep(440, 50);
					LastBeepTime = timeGetTime();
				}
			}
		}

		if (Txt) //�t�@�C���ɏo�͂���ݒ�̏ꍇ
		{
			if (hFile)
			{
				DWORD Bytes;
				WriteFile(hFile, str.c_str(), sizeof(TCHAR) * str.size(), &Bytes, NULL);
				WriteFile(hFile, TEXT("\r\n"), sizeof(TCHAR) * 2, &Bytes, NULL);
			}
		}

		if (!Continue) //���s�ł��Ȃ��G���[�̏ꍇ
		{
			str += TEXT("\n\nLog �t�H���_���ɃG���[���O���L�^����܂��B");
			MessageBox(NULL, str.c_str(), TEXT("�v���O���������s�ł��Ȃ��G���["), MB_OK);
			Finalize(false);

			this->pAppObj->ErrorExit();
			std::exit(-1); //���̏�ŏI��
		}
		
		return;
	}

	//���C�u��������G���[���󂯎��
	void Debug::Receive(const TCHAR *pMsg, const TCHAR *pFile, const int Line, const bool Continue) 
	{
		Throw(pMsg, pFile, Line, Continue, Error); //�o��

		return;
	}

	//���Ԍv���J�n
	void Debug::TimerBegin(DWORD Index)
	{
		if (vectorTime.size() <= Index)
		{
			vectorTime.resize(Index + 1);
			vectorqpc.resize(Index + 1);
		}

		QueryPerformanceCounter(&vectorqpc.at(Index)); //�v���J�n

		return;
	}

	//���Ԍv���I��
	void Debug::TimerEnd(DWORD Index)
	{
		LARGE_INTEGER EndTime;
		QueryPerformanceCounter(&EndTime); //�v���I��

		vectorTime.at(Index) = (double(EndTime.QuadPart - vectorqpc.at(Index).QuadPart) / (double)qpf.QuadPart) * 1000.0; //ms�P�ʂŊi�[

		return;
	}

	//�v�����Ԃ��擾
	double Debug::GetTimerTime(DWORD Index) 
	{
		if (vectorTime.size() <= Index)
			return 0.0;

		return vectorTime.at(Index);
	}
}
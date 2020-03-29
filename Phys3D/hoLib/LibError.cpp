#include "LibError.h"
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

namespace ho
{
	LibError *LibError::pLibErrorObj = NULL;

	//�R���X�g���N�^
	LibError::LibError() 
	{
		pErrorReceiverObj = NULL;
	}

	//�G���[����
	void LibError::Throw(const TCHAR *pMsg, const TCHAR *pFile, const int Line, const bool Continue) 
	{
		if (pErrorReceiverObj) //�G���[�󂯎��I�u�W�F�N�g���ݒ肳��Ă���ꍇ
			pErrorReceiverObj->Receive(pMsg, pFile, Line, Continue);

		return;
	}

	//�G���[�����擾
	void LibError::GetLastErrorStr(tstring &str)
	{
		void *pBuf;

		//�G���[�\��������쐬
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&pBuf, 0, NULL);

		str = (TCHAR *)pBuf;

		LocalFree(pBuf);

		return;
	}



	//�R���X�g���N�^
	Exception::Exception(const tstring &strMsg, const TCHAR *pFile, const int Line, const bool Continue)
	{
		this->strMsg = strMsg;
		this->pFile = (TCHAR *)pFile;
		this->Line = Line;
		this->Continue = (bool)Continue;
	}

	//�R���X�g���N�^
	Exception::Exception(const TCHAR *pMsg, const TCHAR *pFile, const int Line, const bool Continue)
	{
		this->strMsg = pMsg;
		this->pFile = (TCHAR *)pFile;
		this->Line = Line;
		this->Continue = (bool)Continue;
	}
}

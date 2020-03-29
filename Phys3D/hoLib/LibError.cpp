#include "LibError.h"
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

namespace ho
{
	LibError *LibError::pLibErrorObj = NULL;

	//コンストラクタ
	LibError::LibError() 
	{
		pErrorReceiverObj = NULL;
	}

	//エラー発生
	void LibError::Throw(const TCHAR *pMsg, const TCHAR *pFile, const int Line, const bool Continue) 
	{
		if (pErrorReceiverObj) //エラー受け取りオブジェクトが設定されている場合
			pErrorReceiverObj->Receive(pMsg, pFile, Line, Continue);

		return;
	}

	//エラー文字取得
	void LibError::GetLastErrorStr(tstring &str)
	{
		void *pBuf;

		//エラー表示文字列作成
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&pBuf, 0, NULL);

		str = (TCHAR *)pBuf;

		LocalFree(pBuf);

		return;
	}



	//コンストラクタ
	Exception::Exception(const tstring &strMsg, const TCHAR *pFile, const int Line, const bool Continue)
	{
		this->strMsg = strMsg;
		this->pFile = (TCHAR *)pFile;
		this->Line = Line;
		this->Continue = (bool)Continue;
	}

	//コンストラクタ
	Exception::Exception(const TCHAR *pMsg, const TCHAR *pFile, const int Line, const bool Continue)
	{
		this->strMsg = pMsg;
		this->pFile = (TCHAR *)pFile;
		this->Line = Line;
		this->Continue = (bool)Continue;
	}
}

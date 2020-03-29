#include "Error.h"

namespace ho
{
	std::shared_ptr<ErrorManager> ErrorManager::spErrorManagerObj;

	//コンストラクタ
	ErrorManager::ErrorManager()
	{
		QueryPerformanceFrequency(&this->qpf);
		QueryPerformanceCounter(&this->qpc);

		AllocConsole(); //コンソール生成
	}

	//デストラクタ
	ErrorManager::~ErrorManager()
	{
		FreeConsole(); //コンソール解放
	}

	//エラー情報を追加する
	void ErrorManager::addErrorInfo(std::shared_ptr<ErrorInfo_Base> spErrorInfoObj)
	{
		_cputts(spErrorInfoObj->getErrorStr(this->spErrorManagerObj).c_str()); //エラー文字列をコンソールへ出力

		
		if (spErrorInfoObj->getExit()) //続行できないエラーの場合
		{
			MessageBox(NULL, spErrorInfoObj->getErrorStr(this->spErrorManagerObj).c_str(), TEXT("アプリケーションが続行できないエラー"), MB_OK); //メッセージボックス表示
			std::exit(-1); //その場で終了
		}
		

		return;
	}

	//コンストラクタ
	ErrorInfo_Base::ErrorInfo_Base(const bool &Exit)
		: Exit(Exit)
	{
	}

	//デストラクタ
	ErrorInfo_Base::~ErrorInfo_Base()
	{
	}


	//コンストラクタ
	ErrorInfo::ErrorInfo(const bool &Exit, const tstring &str)
		: ErrorInfo_Base(Exit), str(str)
	{}

	//デストラクタ
	ErrorInfo::~ErrorInfo()
	{}

	//エラー文字列を取得
	tstring ErrorInfo::getErrorStr(std::shared_ptr<ErrorManager> spErrorManagerObj) const
	{
		return this->str;
	}


	//コンストラクタ
	ErrorInfoDetail::ErrorInfoDetail(const bool &Exit, const tstring &str, const tstring &strFile, const int &Line)
		: ErrorInfo(Exit, str), strFile(strFile), Line(Line)
	{
		QueryPerformanceCounter(&this->qpc);
	}

	//デストラクタ
	ErrorInfoDetail::~ErrorInfoDetail()
	{}

	//エラー文字列を取得
	tstring ErrorInfoDetail::getErrorStr(std::shared_ptr<ErrorManager> spErrorManagerObj) const
	{
		//時間情報を、エラー管理オブジェクトが生成されてから経過した時間（ミリ秒単位）に変換する
		LARGE_INTEGER t;
		t.QuadPart = this->qpc.QuadPart - spErrorManagerObj->getqpc().QuadPart;
		t.QuadPart = t.QuadPart / LONGLONG(spErrorManagerObj->getqpf().QuadPart * 0.001);

		return tPrint(TEXT("[%lld] %s (%d)\n%s"), t.QuadPart, this->strFile.c_str(), this->Line, this->str.c_str());
	}


	//コンストラクタ
	Exception::Exception(std::shared_ptr<ErrorManager> spErrorManagerObj, const tstring &strMsg, const TCHAR *pFile, const int Line, const bool Exit)
	{
		std::shared_ptr<ErrorInfo> spErrorInfoObj(new ErrorInfo(Exit, strMsg));

		spErrorManagerObj->addErrorInfo(spErrorInfoObj);
	}

	//コンストラクタ
	Exception::Exception(std::shared_ptr<ErrorManager> spErrorManagerObj, const TCHAR *pMsg, const TCHAR *pFile, const int Line, const bool Exit)
	{
		std::shared_ptr<ErrorInfo> spErrorInfoObj(new ErrorInfo(Exit, tstring(TEXT("ERROR!"))));

		spErrorManagerObj->addErrorInfo(spErrorInfoObj);
	}

}
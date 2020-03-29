//エラー情報の処理を行うクラス
#pragma once

#include "tstring.h"
#include <Windows.h>
#include <boost/shared_ptr.hpp>

//エラー出力マクロ
#define ERR(Exit, str, strFile, Line) ho::ErrorManager::spErrorManagerObj->addErrorInfo(std::shared_ptr<ho::ErrorInfo>(new ho::ErrorInfoDetail(Exit, str, strFile, Line)));
#define ERRc(Exit, str, strFile, Line) ho::ErrorManager::spErrorManagerObj->addErrorInfo(std::shared_ptr<ho::ErrorInfo>(new ho::ErrorInfoDetail(Exit, str, strFile, Line)));

// __FILE__ を TCHAR に対応するため __WFILE__ を使えば解決するようにするマクロ
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#if _UNICODE //Unicodeの場合
#define __WFILE__ WIDEN(__FILE__) //Unicodeに変更
#else
#define __WFILE__ __FILE__ //マルチバイト文字のまま
#endif

namespace ho
{
	class ErrorInfo_Base;

	//エラー情報管理クラス
	class ErrorManager
	{
	public:
		ErrorManager(); //コンストラクタ
		~ErrorManager(); //デストラクタ

		void addErrorInfo(std::shared_ptr<ErrorInfo_Base> spErrorInfoObj); //エラー情報を追加する

		static std::shared_ptr<ErrorManager> spErrorManagerObj;

		//アクセッサ
		const LARGE_INTEGER &getqpf() const { return qpf; }
		const LARGE_INTEGER &getqpc() const { return qpc; }
	private:
		LARGE_INTEGER qpf; //QueryPerformanceFrequency の値
		LARGE_INTEGER qpc; //オブジェクト生成時の QueryPerformanceCounter の値
	};

	//一つのエラー情報を表す基底クラス
	class ErrorInfo_Base
	{
	public:
		ErrorInfo_Base(const bool &Exit); //コンストラクタ
		virtual ~ErrorInfo_Base(); //デストラクタ

		virtual tstring getErrorStr(std::shared_ptr<ErrorManager> spErrorManagerObj) const { return tstring(TEXT("Require Override.")); } //エラー文字列を取得

		//アクセッサ
		const bool getExit() const { return Exit; }
	protected:
		const bool Exit; //エラー発生と同時にアプリケーションを終了するかどうか
	};

	//通常のエラー情報を表すクラス
	class ErrorInfo : public ErrorInfo_Base
	{
	public:
		ErrorInfo(const bool &Exit, const tstring &str); //コンストラクタ
		virtual ~ErrorInfo(); //デストラクタ

		virtual tstring getErrorStr(std::shared_ptr<ErrorManager> spErrorManagerObj) const override; //エラー文字列を取得
	protected:
		const tstring str; //エラー文字列
	};

	//エラー発生時刻、ソースコードファイル名、行番号、を含むエラー情報を表すクラス
	class ErrorInfoDetail : public ErrorInfo
	{
	public:
		ErrorInfoDetail(const bool &Exit, const tstring &str, const tstring &strFile, const int &Line); //コンストラクタ
		virtual ~ErrorInfoDetail(); //デストラクタ

		virtual tstring getErrorStr(std::shared_ptr<ErrorManager> spErrorManagerObj) const override; //エラー文字列を取得
	protected:
		LARGE_INTEGER qpc; //エラー発生時刻
		const tstring strFile; //ソースコードファイル名
		const int Line; //行数
	};

	//例外オブジェクト
	class Exception
	{
	public:
		Exception(std::shared_ptr<ErrorManager> spErrorManagerObj, const tstring &strMsg, const TCHAR *pFile, const int Line, const bool Exit); //コンストラクタ
		Exception(std::shared_ptr<ErrorManager> spErrorManagerObj, const TCHAR *pMsg, const TCHAR *pFile, const int Line, const bool Exit); //コンストラクタ

		tstring strMsg;
		TCHAR *pFile;
		int Line;
		bool Exit;
	};

}
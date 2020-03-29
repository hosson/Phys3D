//ライブラリ内部のエラーを処理するクラス
#pragma once
#include <tchar.h>
#include "tstring.h"

//ライブラリ用エラー出力マクロ
#define ER(Msg, File, Line, Continue) LibError::pLibErrorObj->Throw(Msg, File, Line, Continue); 

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
	//前方宣言
	class ErrorReceiver;

	class LibError
	{
	public:
		LibError(); //コンストラクタ
		~LibError() {} //デストラクタ

		void Throw(const TCHAR *pMsg, const TCHAR *pFile, const int Line, const bool Continue = true); //エラー発生

		static void GetLastErrorStr(tstring &str); //エラー文字取得
		static LibError *pLibErrorObj;

		//アクセッサ
		void SetpErrorReceiverObj(ErrorReceiver *pObj) { this->pErrorReceiverObj = pObj; }
	private:
		ErrorReceiver *pErrorReceiverObj;
	};

	class ErrorReceiver
	{
	public:
		ErrorReceiver() {} //コンストラクタ
		virtual ~ErrorReceiver() {} //デストラクタ
		virtual void Receive(const TCHAR *pMsg, const TCHAR *pFile, const int Line, const bool Continue) {} //エラーを受け取る
	};

	//例外オブジェクト
	class Exception
	{
	public:
		Exception(const tstring &strMsg, const TCHAR *pFile, const int Line, const bool Continue); //コンストラクタ
		Exception(const TCHAR *pMsg, const TCHAR *pFile, const int Line, const bool Continue); //コンストラクタ
		tstring strMsg;
		TCHAR *pFile;
		int Line;
		bool Continue;
	};
}

/*
ho::hoLib クラスのオブジェクトを作成すると自動で Error オブジェクトも作成され、
ライブラリ内部で発生したエラー情報は一旦ここに集約される。

ErrorReceiver クラスを継承して Receive() メソッドをオーバーライドしたクラスを
プログラム本体側に作ると、プログラム本体で使うエラー処理機能へエラー内容を送信することができる。
*/
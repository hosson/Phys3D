//デバッグクラス
#pragma once
#include <Windows.h>
#include <vector>
#include "tstring.h"
#include "LibError.h"

class App;

//エラー出力マクロ
#define ERR(Msg, File, Line, Continue) ho::Debug::pObj->Throw(Msg, File, Line, Continue, ho::Debug::Error);
#define ERRLOG(Msg, File, Line, Continue) ho::Debug::pObj->Throw(Msg, File, Line, Continue, ho::Debug::Log);

namespace ho {
	class Debug : public ErrorReceiver
	{
	public:
		enum eOutputType
		{
			Log,	//通常ログ
			Error	//エラー
		};
	public:
		Debug(::App *const pAppObj, HWND hWnd, bool Console, bool Txt, const SYSTEMTIME &st, const bool PlayBeep); //コンストラクタ
		~Debug(); //デストラクタ

		void Throw(const TCHAR *pMsg, const TCHAR *pFile, const int Line, const bool Continue, eOutputType OutputType); //出力
		void Receive(const TCHAR *pMsg, const TCHAR *pFile, const int Line, const bool Continue); //ライブラリからエラーを受け取る
		static Debug *pObj;

		void TimerBegin(DWORD Index); //時間計測開始
		void TimerEnd(DWORD Index); //時間計測終了
		double GetTimerTime(DWORD Index); //計測時間を取得
	private:
		::App *const pAppObj;
		bool Console; //コンソールを使うかどうか
		bool Txt; //テキストファイルへ出力するかどうか
		bool PlayBeep; //エラー時にBeepを鳴らすかどうか
		tstring strFilename; //ログ出力用のファイル名
		HANDLE hFile; //ファイルハンドル
		HWND hWnd; //続行不可能なエラー発生時にメッセージボックスを出すウィンドウのハンドル
		DWORD BeginTime; //デバッグシステム起動時にtimeGetTime()で得られた時間
		DWORD LastBeepTime; //最後のBeepが鳴らされた時間
		LARGE_INTEGER qpf; //QueryPerformanceFrequency()で得られる分解能
		std::vector<double> vectorTime; //タイマーで計測した時間
		std::vector<LARGE_INTEGER> vectorqpc; //計測開始時間

		void Finalize(bool Normal); //終了処理
	};

}

/*エラーメッセージやエラーログ、動作ログの出力などのデバッグ処理用クラス。
*/

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

	//コンストラクタ
	Debug::Debug(::App *const pAppObj, HWND hWnd, bool Console, bool Txt, const SYSTEMTIME &st, const bool PlayBeep)
		: pAppObj(pAppObj), PlayBeep(PlayBeep)
	{
		//時刻とバージョンからファイル名を設定
		ho::tstring Filename;
		ho::tstring ver = _VERSION;
		Filename.push_back(ver.at(0));
		Filename.push_back(ver.at(2));
		Filename.push_back(ver.at(3));
		if (_TRIAL_TF) //体験版の場合
			Filename.push_back(TEXT('t'));
		else //製品版の場合
			Filename.push_back(TEXT('p'));
		Filename.push_back(TEXT('_'));
		ho::tPrint(this->strFilename, TEXT("%04d%02d%02d%02d%02d%02d.txt"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		this->strFilename = Filename + this->strFilename;

		pObj = this;
		BeginTime = timeGetTime(); //起動時の時間を取得
		LastBeepTime = BeginTime;

		this->hWnd = hWnd;

		this->Console = Console;
		if (Console)
			AllocConsole(); //コンソール作成

		this->Txt = Txt;
		if (Txt)
		{
			if (!PathIsDirectory(TEXT("Log"))) //Log フォルダが存在しない場合
			{
				if (!CreateDirectory(TEXT("Log"), NULL)) //ディレクトリを作成し、失敗した場合
				{
					ho::tstring strError;
					ho::LibError::GetLastErrorStr(strError); //エラー文字列取得
					throw ho::Exception(strError.c_str(), __WFILE__, __LINE__, true);
				}
			}

			if (!SetCurrentDirectory(TEXT("Log"))) //カレントディレクトリをLogフォルダへ移動
			{
				ho::tstring strError;
				ho::LibError::GetLastErrorStr(strError); //エラー文字列取得
				throw ho::Exception(strError.c_str(), __WFILE__, __LINE__, true);
			}

			hFile = CreateFile(this->strFilename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL); //ファイル作成
			if (hFile == INVALID_HANDLE_VALUE) //ファイル作成に失敗した場合
			{
				ER(TEXT("エラーログ用のファイルを開けませんでした。"), __WFILE__, __LINE__, true);

				ho::tstring str;
				LibError::GetLastErrorStr(str); //エラー文字列取得
				ER(str.c_str(), __WFILE__, __LINE__, true); //エラー出力

				this->Txt = false;
			} else {
				unsigned char BOM[2] = { 0xFF, 0xFE };
				DWORD Bytes;
				WriteFile(hFile, BOM, 2, &Bytes, NULL);

				Throw(TEXT("エラーログファイルを開きました。"), __WFILE__, __LINE__, true, Log);
			}

			if (!SetCurrentDirectory(TEXT("..\\"))) //カレントディレクトリを元に戻す
			{
				ho::tstring strError;
				ho::LibError::GetLastErrorStr(strError); //エラー文字列取得
				throw ho::Exception(strError.c_str(), __WFILE__, __LINE__, true);
			}
		}

		QueryPerformanceFrequency(&qpf);
	}

	//デストラクタ
	Debug::~Debug() 
	{
		Finalize(true); //終了処理
	}

	//終了処理
	void Debug::Finalize(bool Normal) 
	{
		if (Normal) //正常終了の場合
			Throw(TEXT("デバッグシステムは正常に終了します。"), __WFILE__, __LINE__, true, Log);
		else
			Throw(TEXT("■プログラムが続行できないエラーが発生したため、デバッグシステムにより実行が中断されました。"), __WFILE__, __LINE__, true, Log);

		if (Console)
			FreeConsole(); //コンソール削除
		if (Txt)
		{
			Throw(TEXT("エラーログファイルを閉じる処理を開始します。"), __WFILE__, __LINE__, true, Log);
			if (CloseHandle(hFile)) //ファイルハンドルを閉じる
			{
				Throw(TEXT("エラーログファイルを閉じました。"), __WFILE__, __LINE__, true, Log);
			} else {
				ER(TEXT("エラーログ用のファイルを閉じるのに失敗しました。"), __WFILE__, __LINE__, true);

				ho::tstring str;
				LibError::GetLastErrorStr(str); //エラー文字列取得
				ER(str.c_str(), __WFILE__, __LINE__, true); //エラー出力
			}

			if (!SetCurrentDirectory(TEXT("Log"))) //カレントディレクトリをLogフォルダへ移動
			{
				ho::tstring strError;
				ho::LibError::GetLastErrorStr(strError); //エラー文字列取得
				throw ho::Exception(strError.c_str(), __WFILE__, __LINE__, true);
			}

			if (!this->pAppObj->GetpConfigObj()->Getnonetwork()) //ネットワーク送信しない設定ではない場合
			{
				//エラーログをサーバにアップロード
				CGI_Upload CGI_UploadObj("vertex.matrix.jp");
				std::string strVer = ho::WCharToChar(_VERSION); //Unicodeのバージョン文字列をcharに変換
				CGI_UploadObj.Upload(this->strFilename, strVer);
			}

			if (!SetCurrentDirectory(TEXT("..\\"))) //カレントディレクトリを元に戻す
			{
				ho::tstring strError;
				ho::LibError::GetLastErrorStr(strError); //エラー文字列取得
				throw ho::Exception(strError.c_str(), __WFILE__, __LINE__, true);
			}
		}

		return;
	}

	//出力
	void Debug::Throw(const TCHAR *pMsg, const TCHAR *pFile, const int Line, const bool Continue, eOutputType OutputType)
	{
		//出力用の文字列を作成
		tstring str;
		ho::tPrint(str, TEXT("[%d] %s %s (%d)\n"), timeGetTime() - BeginTime, pMsg, pFile, Line);

		if (Console) //コンソールへ出力する設定の場合
		{
			_cputts(str.c_str()); //を出力
			OutputDebugString(str.c_str()); //VisualStudioに出力

			if (OutputType == Error) //エラーの場合
			{
				if (LastBeepTime - timeGetTime() > 1000) //最後にBeepが鳴らされてから1秒以上経過している場合
				{
					if (this->PlayBeep) //Beep音を鳴らす設定の場合
						Beep(440, 50);
					LastBeepTime = timeGetTime();
				}
			}
		}

		if (Txt) //ファイルに出力する設定の場合
		{
			if (hFile)
			{
				DWORD Bytes;
				WriteFile(hFile, str.c_str(), sizeof(TCHAR) * str.size(), &Bytes, NULL);
				WriteFile(hFile, TEXT("\r\n"), sizeof(TCHAR) * 2, &Bytes, NULL);
			}
		}

		if (!Continue) //続行できないエラーの場合
		{
			str += TEXT("\n\nLog フォルダ内にエラーログが記録されます。");
			MessageBox(NULL, str.c_str(), TEXT("プログラムが続行できないエラー"), MB_OK);
			Finalize(false);

			this->pAppObj->ErrorExit();
			std::exit(-1); //その場で終了
		}
		
		return;
	}

	//ライブラリからエラーを受け取る
	void Debug::Receive(const TCHAR *pMsg, const TCHAR *pFile, const int Line, const bool Continue) 
	{
		Throw(pMsg, pFile, Line, Continue, Error); //出力

		return;
	}

	//時間計測開始
	void Debug::TimerBegin(DWORD Index)
	{
		if (vectorTime.size() <= Index)
		{
			vectorTime.resize(Index + 1);
			vectorqpc.resize(Index + 1);
		}

		QueryPerformanceCounter(&vectorqpc.at(Index)); //計測開始

		return;
	}

	//時間計測終了
	void Debug::TimerEnd(DWORD Index)
	{
		LARGE_INTEGER EndTime;
		QueryPerformanceCounter(&EndTime); //計測終了

		vectorTime.at(Index) = (double(EndTime.QuadPart - vectorqpc.at(Index).QuadPart) / (double)qpf.QuadPart) * 1000.0; //ms単位で格納

		return;
	}

	//計測時間を取得
	double Debug::GetTimerTime(DWORD Index) 
	{
		if (vectorTime.size() <= Index)
			return 0.0;

		return vectorTime.at(Index);
	}
}
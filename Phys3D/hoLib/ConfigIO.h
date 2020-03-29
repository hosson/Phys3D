//コンフィグファイル向けのIOを提供するクラス

#pragma once
#include <Windows.h>
#include "hoLib.h"
#include "tstring.h"

namespace ho
{
	class ConfigIO
	{
	public:
		ConfigIO(TCHAR *pFilename); //コンストラクタ
		~ConfigIO(); //デストラクタ

		BOOL Read(TCHAR *pName, int *pValue); //指定された値を読む（int型）
		BOOL Read(TCHAR *pName, double *pValue); //指定された値を読む（double型）
		BOOL Read(TCHAR *pName, tstring &Value); //指定された値を読む（TCHAR型）
		BOOL Write(TCHAR *pName, int Value); //指定された値を書き込む（int型）
		BOOL Write(TCHAR *pName, double Value); //指定された値を書き込む（double型）
		BOOL Write(TCHAR *pName, TCHAR *pValue); //指定された値を読む（TCHAR型）
	private:
		tstring strFilename;		
		HANDLE hFile; //ファイルハンドル

		BOOL ReadGetAt(TCHAR *pName, tstring &str); //ファイルを開いて pName の = の右側の文字列を取得
		BOOL FileOpen(DWORD dwDesiredAccess, DWORD dwCreationDisposition); //ファイルを開く
		BOOL FileClose(); //ファイルを閉じる
		BOOL FindName(TCHAR *pName); //名前を検索（名前の=の右側へhFileのポインタを送る）
		BOOL SearchAt(TCHAR Key); //Keyの文字列が見つかるまでファイルを読み続ける
		TCHAR GetAt(TCHAR *pKeys, int Keys, tstring &strOut); //keyの文字か改行コードが見つかるまでの文字列をlpBufへ格納する
		BOOL SearchAtReturn(); //改行が見つかるまでファイルを読み続ける
		BOOL InsertStr(TCHAR *pStr); //ファイルの現在位置に文字列を挿入する
		BOOL DeleteAtReturn(); //現在位置から現在の行の改行手前までを削除（改行は残る）
	};
}

/*
テキスト形式のコンフィグファイルを読み書きするクラス。
Width=640
などのように、キーとなる名前と値がイコールで区切られる。データは改行を挟んで次々と記録される。
*/

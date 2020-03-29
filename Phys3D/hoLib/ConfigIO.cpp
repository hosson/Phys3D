#include "ConfigIO.h"
#include "Debug.h"
#include <tchar.h>
#include "LibError.h"

namespace ho
{
	//コンストラクタ
	ConfigIO::ConfigIO(TCHAR *pFilename)
	{
		this->strFilename = pFilename;
		this->hFile = NULL;
	}

	//デストラクタ
	ConfigIO::~ConfigIO()
	{
	}

	//指定された値を読む（int型）
	BOOL ConfigIO::Read(TCHAR *pName, int *pValue)
	{
		/*
		if (!FileOpen(GENERIC_READ, OPEN_EXISTING)) //ファイルを読み込みモードで開く、ファイルが存在しない場合は失敗
			return FALSE;

		if (!FindName(pName)) //名前を検索（名前の=の右側へhFileのポインタを送る）
		{
			FileClose(); //ファイルを閉じる
			return FALSE;
		}

		tstring str;

		GetAt(NULL, 0, str); //改行までの文字列を取得
		*/

		tstring str;
		if (ReadGetAt(pName, str))
		{
			*pValue = _ttoi(str.c_str()); //文字列を整数へ変換

			FileClose(); //ファイルを閉じる

			return TRUE;
		}

		return FALSE;
	}

	//指定された値を読む（double型）
	BOOL ConfigIO::Read(TCHAR *pName, double *pValue)
	{
		tstring str;
		if (ReadGetAt(pName, str))
		{
			*pValue = _ttof(str.c_str()); //文字列を double 型へ変換

			FileClose(); //ファイルを閉じる

			return TRUE;
		}

		return FALSE;
	}
	
	//指定された値を読む（TCHAR型）
	BOOL ConfigIO::Read(TCHAR *pName, tstring &Value)
	{
		if (ReadGetAt(pName, Value))
		{
			FileClose(); //ファイルを閉じる
			return TRUE;
		}

		return FALSE;
	}

	//ファイルを開いて pName の = の右側の文字列を取得
	BOOL ConfigIO::ReadGetAt(TCHAR *pName, tstring &str)
	{
		if (!FileOpen(GENERIC_READ, OPEN_EXISTING)) //ファイルを読み込みモードで開く、ファイルが存在しない場合は失敗
			return FALSE;

		if (!FindName(pName)) //名前を検索（名前の=の右側へhFileのポインタを送る）
		{
			FileClose(); //ファイルを閉じる
			return FALSE;
		}

		return GetAt(NULL, 0, str); //改行までの文字列を取得
	}

	//指定された値を書き込む（int型）
	BOOL ConfigIO::Write(TCHAR *pName, int Value)
	{
		if (!FileOpen(GENERIC_READ | GENERIC_WRITE, OPEN_ALWAYS)) //ファイルを読み書きモードで開く、ファイルが存在しない場合は新規作成
			return FALSE;

		tstring strLine;
	
		if (!FindName(pName)) //名前を検索（名前の=の右側へhFileのポインタを送る）
		{ //名前が見つからなかった場合
			ho::tPrint(strLine, TEXT("%s=%d\r\n"), pName, Value);
			InsertStr((TCHAR *)strLine.c_str()); //名前=値 を挿入
		} else { //名前が見つかった場合
			if (DeleteAtReturn()) //現在位置からその行の改行手前までを削除（改行は残る）
			{
				ho::tPrint(strLine, TEXT("%d"), Value);
				InsertStr((TCHAR *)strLine.c_str()); //値を挿入
			}
		}

		FileClose(); //ファイルを閉じる
		
		return TRUE;
	}

	//指定された値を書き込む（double型）
	BOOL ConfigIO::Write(TCHAR *pName, double Value)
	{
		if (!FileOpen(GENERIC_READ | GENERIC_WRITE, OPEN_ALWAYS)) //ファイルを読み書きモードで開く、ファイルが存在しない場合は新規作成
			return FALSE;

		tstring strLine;
	
		if (!FindName(pName)) //名前を検索（名前の=の右側へhFileのポインタを送る）
		{ //名前が見つからなかった場合
			ho::tPrint(strLine, TEXT("%s=%f\r\n"), pName, Value);
			InsertStr((TCHAR *)strLine.c_str()); //名前=値 を挿入
		} else { //名前が見つかった場合
			if (DeleteAtReturn()) //現在位置からその行の改行手前までを削除（改行は残る）
			{
				ho::tPrint(strLine, TEXT("%f"), Value);
				InsertStr((TCHAR *)strLine.c_str()); //値を挿入
			}
		}

		FileClose(); //ファイルを閉じる
		return TRUE;
	}

	//指定された値を読む（TCHAR型）
	BOOL ConfigIO::Write(TCHAR *pName, TCHAR *pValue)
	{
		//■未実装
		return TRUE;
	}

	//ファイルを開く
	BOOL ConfigIO::FileOpen(DWORD dwDesiredAccess, DWORD dwCreationDisposition)
	{
		if (hFile) //ファイルハンドルが既に存在している場合
		{
			ER(TEXT("ファイルハンドルが既に存在しています。"), __WFILE__, __LINE__, true);
			return FALSE;
		}

		hFile = CreateFile(strFilename.c_str(), dwDesiredAccess, 0, 0, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) //ファイルが開けなかった場合
		{
			hFile = NULL;
			return FALSE;
		}

		SetFilePointer(hFile, NULL, NULL, FILE_BEGIN); //ファイルポインタを先頭に持ってくる

		if (dwDesiredAccess & GENERIC_WRITE) //書き込みを含むモードの場合
		{
			WORD bom = 0xfeff; //UTF-16(リトルエンディアン）のBOM Unicodeでもある
			DWORD Bytes;
			WriteFile(hFile, &bom, sizeof(WORD), &Bytes, NULL); //先頭にBomを書き込む
		} else {
			SetFilePointer(hFile, 2, NULL, FILE_CURRENT); //Unicode の最初の16ビットの識別コード（BOM）を飛ばす
		}

		return TRUE;
	}

	//ファイルを閉じる
	BOOL ConfigIO::FileClose()
	{
		if (!hFile) //ファイルハンドルが存在しない場合
		{
			ER(TEXT("ファイルハンドルが既に存在していません。"), __WFILE__, __LINE__, true);
		}

		FlushFileBuffers(hFile); //ファイルのバッファをフラッシュ

		if (!CloseHandle(hFile)) //ファイルが閉じれなかった場合
		{
			ER(TEXT("ファイルを閉じられませんでした。"), __WFILE__, __LINE__, true);
			return FALSE;
		}

		hFile = NULL;

		return TRUE;
	}

	//名前を検索（名前の=の右側へhFileのポインタを送る）
	BOOL ConfigIO::FindName(TCHAR *pName) 
	{
		TCHAR Hit; //見つかった文字
		tstring strRead; //ファイルから読んだ文字を格納する

		while (TRUE) //EOFか見つかるまで繰り返す
		{
			Hit = GetAt(TEXT(" =["), 3, strRead); // =か[かスペースまでの文字列を取得
			if (!Hit) //EOFが見つかった場合
				return FALSE;
	
			if (!_tcsicmp(pName, strRead.c_str())) //検索文字列と取得した=までの文字列が等しい場合
			{
				break;
			} else {
				SearchAtReturn(); //次の行へファイルポインタを移す
			}
		}

		if (Hit != TEXT('=')) //見つかった文字が=以外だった場合
		{
			SearchAt(TEXT('=')); //=が見つかるまでファイルを読む
		}

		return TRUE;
	}

	//Keyの文字列が見つかるまでファイルを読み続ける
	BOOL ConfigIO::SearchAt(TCHAR Key) 
	{
		TCHAR ReadStr;
		DWORD dwBytes;

		do
		{
			if (ReadFile(hFile, &ReadStr, sizeof(TCHAR), &dwBytes, NULL)) //1文字取得
				if (!dwBytes) //EOFに達していた場合
					return FALSE;
		} while (ReadStr != Key); //読んだ文字と探している文字が違う場合は続ける

		return TRUE;
	}

	//keyの文字か改行コードが見つかるまでの文字列をstrOutへ格納する
	TCHAR ConfigIO::GetAt(TCHAR *pKeys, int Keys, tstring &strOut) 
	{
		strOut.clear(); //初期化
		strOut.resize(64); //受取り用バッファの初期サイズ
		DWORD dwBytes;
		TCHAR Hit; //見つかった文字
		TCHAR Buffer;

		DWORD i = 0;
		while (TRUE)
		{
			for (i = 0; i < strOut.size() - 1; i++)
			{
				if (ReadFile(hFile, &Buffer, sizeof(TCHAR), &dwBytes, NULL)) //1文字取得
				{
					if (!dwBytes) //EOFに達していた場合
					{
						strOut.clear();
						return NULL;
					}
				}
				strOut.at(i) = Buffer;
				strOut.at(i + 1) = NULL;

				for (int j = 0; j < Keys; j++)
				{
					if (Buffer == pKeys[j]) //Keyに達した場合
					{
						strOut.at(i) = NULL;
						Hit = Buffer;
						goto LABEL1;
					}
				}

				if (i) //すでに2文字以上読み取った場合
				{
					if (strOut.at(i - 1) == TEXT('\r') && strOut.at(i) == TEXT('\n')) //改行コードだった場合
					{
						Hit = strOut.at(i);
						strOut.at(i) = NULL;
						goto LABEL1;
					}
				}

			}
			strOut.resize(strOut.size() * 2); //バッファサイズを2倍にする
		}

	LABEL1:
		return Hit; //見つかった文字を返す
	}

	//改行が見つかるまでファイルを読み続ける
	BOOL ConfigIO::SearchAtReturn() 
	{
		TCHAR ReadStr[3];
		DWORD dwBytes;
		int i = 0; //読み取る場所

		while (TRUE)
		{
			if (ReadFile(hFile, &ReadStr[i], sizeof(TCHAR), &dwBytes, NULL)) //1文字取得
				if (!dwBytes) //EOFに達していた場合
					return FALSE;

			if (i) //2文字以上読んでいた場合
			{
				if (ReadStr[0] == TEXT('\r') && ReadStr[1] == TEXT('\n')) //改行コードだった場合
					break;

				ReadStr[0] = ReadStr[1];
			} else {
				i = 1;
			}
		}

		return TRUE;
	}

	BOOL ConfigIO::InsertStr(TCHAR *pStr) //ファイルの現在位置に文字列を挿入する
	{
		TCHAR *pBuffer = new TCHAR[GetFileSize(hFile, NULL)]; //ファイルサイズ分のバッファを確保
		if (!pBuffer)
		{
			ER(TEXT("メモリが不足しています。"), __WFILE__, __LINE__, true);
			return FALSE;
		}

		DWORD dwFilePointer = SetFilePointer(hFile, 0, NULL, FILE_CURRENT); //ファイルポインタの現在位置を記録

		BOOL Result;
		DWORD dwBytes;
		int i = 0; //バッファ内の現在位置を示すカウント
		do
		{
			Result = ReadFile(hFile, &pBuffer[i], sizeof(TCHAR), &dwBytes, NULL); //1文字取得
			i++;
		} while (!Result || dwBytes); //EOFに達するまで読み続ける
		int BufferLen = i - 1; //読み取った文字数

		SetFilePointer(hFile, (LONG)dwFilePointer, NULL, FILE_BEGIN); //ファイルポインタを挿入地点に戻す

		//挿入文字の挿入
		for (i = 0; i < lstrlen(pStr); i++)
			WriteFile(hFile, &pStr[i], sizeof(TCHAR), &dwBytes, NULL);

		//バッファに退避していた後続文字を書き込む
		for (i = 0; i < BufferLen; i++)
			WriteFile(hFile, &pBuffer[i], sizeof(TCHAR), &dwBytes, NULL);

		SetEndOfFile(hFile); //EOFを設定

		SDELETE(pBuffer);

		return TRUE;
	}

	BOOL ConfigIO::DeleteAtReturn() //現在位置から現在の行の改行手前までを削除（改行は残る）
	{
		TCHAR *pBuffer = new TCHAR[GetFileSize(hFile, NULL)]; //ファイルサイズ（Byte）*TCHAR のバッファを確保
		if (!pBuffer)
		{
			ER(TEXT("メモリが不足しています。"), __WFILE__, __LINE__, true);
			return FALSE;
		}

		DWORD dwFilePointer = SetFilePointer(hFile, 0, NULL, FILE_CURRENT); //ファイルポインタの現在位置を記録
		DWORD dwBytes;

		if (!SearchAtReturn()) //改行が見つかるまでファイルを読み続ける
		{ //改行が見つからなかった（最後の行だった）場合
			SetFilePointer(hFile, (LONG)dwFilePointer, NULL, FILE_BEGIN); //ファイルポインタを記録地点に戻す
			TCHAR str[3] = TEXT("\r\n");
			WriteFile(hFile, str, sizeof(TCHAR) * 2, &dwBytes, NULL); //改行を書き込む
			SetEndOfFile(hFile); //EOFを設定
			SetFilePointer(hFile, (LONG)dwFilePointer, NULL, FILE_BEGIN); //ファイルポインタを記録地点に戻す
			return TRUE;
		}

		BOOL Result;
		int i = 0; //バッファ内の現在位置を示すカウント
		do
		{
			Result = ReadFile(hFile, &pBuffer[i], sizeof(TCHAR), &dwBytes, NULL); //1文字取得
			i++;
		} while (!Result || dwBytes); //EOFに達するまで読み続ける
		int BufferLen = i - 1; //読み取った文字数

		SetFilePointer(hFile, (LONG)dwFilePointer, NULL, FILE_BEGIN); //ファイルポインタを挿入地点に戻す

		//改行を書き込む
		TCHAR str[3] = TEXT("\r\n");
		WriteFile(hFile, str, sizeof(TCHAR) * 2, &dwBytes, NULL); //改行を書き込む

		//バッファに退避していた後続文字を書き込む
		for (i = 0; i < BufferLen; i++)
		{
			WriteFile(hFile, &pBuffer[i], sizeof(TCHAR), &dwBytes, NULL);
		}

		SetEndOfFile(hFile); //EOFを設定
		SetFilePointer(hFile, (LONG)dwFilePointer, NULL, FILE_BEGIN); //ファイルポインタを挿入地点に戻す

		SDELETE(pBuffer);

		return TRUE;
	}
}

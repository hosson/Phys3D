//CSV形式のファイルを読むクラス
#include <string>
#include <tchar.h>
#include "tstring.h"
#include "CSVReader.h"

/*リソースの文字コードについて
保存は VC++のエディタで Unicode コードページ1200 で保存する
*/

namespace ho {
	//コンストラクタ
	//リソースファイルから読む場合
	CSVReader::CSVReader(HMODULE hModule, TCHAR *lpResName, TCHAR *lpResType) 
	{
		this->hModule = hModule;
		strResName = lpResName;
		strResType = lpResType;

		Mode = Resource;
		Pos = 0;

		bool Error = FALSE; //エラー発生状況

		hGM = LoadResource(hModule, FindResource(hModule, strResName.c_str(), strResType.c_str()));
		if (!hGM)
			Error = TRUE;

		if (!Error)
			p = (LPTSTR)LockResource(hGM); //リソースデータの格納されたメモリへのポインタを取得
		if (p)
		{
			p++; //Unicode先頭のBOM（16ビット）分を飛ばしてポインタを文字列先頭へ持ってくる

			// //で始まる行の場合は次の行へ移動
			while (*p == TEXT('/'))
			{
				FindStr(&p, TEXT('\n'));
				p++;
			}
		}
	}

	//コンストラクタ
	//ファイルから読む場合
	CSVReader::CSVReader(TCHAR *lpFilename)
	{
		this->hModule = NULL;
		strFilename = lpFilename;

		Mode = File;
		Pos = 0;
	}

	//メモリ上のポインタから読む場合
	CSVReader::CSVReader(void *p)
	{
		this->p = (TCHAR *)p;

		if (this->p)
		{
			this->p++; //Unicode先頭のBOM（16ビット）分を飛ばしてポインタを文字列先頭へ持ってくる

			// //で始まる行の場合は次の行へ移動
			while (*this->p == TEXT('/'))
			{
				FindStr(&this->p, TEXT('\n'));
				this->p++;
			}
		}

		Mode = Memory;
		Pos = 0;
	}


	//現在のデータ位置のデータをint型として読む
	bool CSVReader::Read(int *pBuffer)
	{
		*pBuffer = _ttoi(p);
		NextPos(); //ポインタを次のデータ位置に進める

		return TRUE;
	}

	//現在のデータ位置のデータをDWORD型（unsigned long型）として読む
	bool CSVReader::Read(DWORD *pBuffer)
	{
		TCHAR *pEnd;
		*pBuffer = _tcstoul(p, &pEnd, 0);
		NextPos(); //ポインタを次のデータ位置に進める

		return TRUE;
	}

	//現在のデータ位置のデータをfloat型として読む
	bool CSVReader::Read(float *pBuffer)
	{
		*pBuffer = (float)_ttof(p);
		NextPos(); //ポインタを次のデータ位置に進める

		return TRUE;
	}

	//現在のデータ位置のデータをdouble型として読む
	bool CSVReader::Read(double *pBuffer) 
	{
		*pBuffer = _ttof(p);
		NextPos(); //ポインタを次のデータ位置に進める

		return TRUE;
	}

	//現在のデータ位置のデータをtstring型として読む
	bool CSVReader::Read(tstring *pBuffer) 
	{
		tstring str;
		GetStr(&str); //現在のデータ位置の文字列を得る

		pBuffer->clear();

		int dq = 0; // " の出現回数
		for (tstring::iterator itr = str.begin(); itr != str.end(); itr++)
		{
			if (*itr == TEXT('"'))
			{
				dq++;
				if (dq == 1)
					continue;
				else if (dq == 2)
					break;
			}
			if (dq == 1)
				pBuffer->push_back(*itr);
		}

		NextPos(); //ポインタを次のデータ位置に進める

		return TRUE;
	}

	//現在のデータ位置の文字列を得る
	void CSVReader::GetStr(tstring *pStr) 
	{
		TCHAR *pToken = p; //区切り文字の位置
		FindStr(&pToken, TEXT(','), true); //次の区切り文字の位置を検索

		pStr->clear();
		for (TCHAR *i = p; i <= pToken; i++)
			pStr->push_back(*i);

		return;
	}
	//現在のデータ位置のデータ型を返す
	CSVReader::eDataType CSVReader::GetDataType() 
	{
		tstring str;
		GetStr(&str); //現在のデータ位置の文字列を得る

		//文字列中の空白ではなくなる位置を得る
		int Begin = 0;
		for (tstring::iterator itr = str.begin(); itr != str.end(); itr++)
			if (*itr != TEXT(' ')) //空白ではない場合
				break;
			else
				Begin++;

		if (str.at(Begin) == TEXT('"')) //"で始まる場合
			return String; //文字列型

		//数字かどうかの判定
		const TCHAR num[] = TEXT("0123456789.-"); //検索用文字列
		bool Number = FALSE; //数字かどうか
		for (int i = 0; i < 12; i++)
			if (str.at(Begin) == num[i])
				Number = TRUE;
		if (!Number) //数字でもない場合
			return null; //不定

		//Int、Float、Doubleの判定
		bool Period = FALSE, f = FALSE; //小数点とfの有無
		for (unsigned int i = 0; i < str.size(); i++)
		{
			if (str.at(i) == TEXT('.'))
				Period = TRUE;
			if (str.at(i) == TEXT('f'))
				f = TRUE;
		}
		if (Period == FALSE && f == FALSE) //小数点もfもない場合
			return Int; //整数
		if (f)
			return Float; //単精度浮動小数点
		if (Period && !f)
			return Double; //倍精度浮動小数点

		return null; //それ以外は不定
	}

	//データ位置を次へ移動
	void CSVReader::NextPos() 
	{
		TCHAR *pToken = p; //区切り文字の位置
		FindStr(&pToken, TEXT(','), true); //次の区切り文字の位置を検索
		pToken++; //区切り文字の次へ移動

		while (*pToken == TEXT('/')) // /で始まる場合
		{
			FindStr(&pToken, TEXT('\n')); //次の行へ
			pToken++;
		}

		if (*pToken == TEXT('!')) //EOFだった場合
			return; //何もしないで終了

		p = pToken;
		Pos++;

		return;
	}



	//リソース解読中に次の区切り文字を探す
	int CSVReader::FindStr(TCHAR **lpRes, TCHAR Token, bool SkipDoubleQuotes)
	{
		/* SkipDoubleQuotes が true の場合は、 " と " で囲まれた範囲内を区切り文字検索対象から外す。*/

		TCHAR *lpToken = NULL; //区切り文字の位置
		TCHAR *lpEnter = NULL; //改行コードの位置
		//TCHAR *lpToken = _tcschr(*lpRes, Token); //区切り文字の位置
		//TCHAR *lpEnter = _tcschr(*lpRes, TEXT('\n')); //改行コードの位置

		TCHAR *p = *lpRes;
		bool DQ = false; //ダブルクォーテーション内部かどうか
		while (*p) //NULLになるまで走査（注意：ファイル終端に NULL が来る保証は無いので、NULLをあてにしてはいけない）
		{
			if (SkipDoubleQuotes) //ダブルクォーテーション内をスキップする場合
			{
				if (*p == TEXT('"')) //ダブルクォーテーションが見つかった場合
				{
					DQ = (true - DQ) != 0; //ダブルクォーテーション状態を反転
				}
			}

			if (!DQ) //ダブルクォーテーション外の場合
			{
				if (*p == Token && !lpToken)
					lpToken = p;
				if (*p == TEXT('\n') && !lpEnter)
					lpEnter = p;
			}

			if (lpToken || lpEnter) //区切り文字と改行のどちらかが見つかった場合
				break;

			p++;
		}


		if (lpToken) //区切り文字が見つかった場合
			*lpRes = lpToken;
		else //改行コードが見つかった場合
			*lpRes = lpEnter;

		return 0; //この戻り値は使われていない

		/*

		if (!lpToken) //区切り文字が見つからなかった場合
		{
			if (lpEnter) //改行コードは見つかった場合
			{
				*lpRes = lpEnter;
				return 2; //改行コードが先に見つかった場合は2を返す
			}
			return 0;
		}

		if (lpEnter < lpToken) //トークン文字より改行コードの方が先に見つかった場合
		{
			*lpRes = lpEnter;
			return 2; //改行コードが先に見つかった場合は2を返す
		}

		*lpRes = lpToken;

		return 1; //区切り文字の次が存在したら1を返す
		*/
	}
}

#include "tstring.h"
#include <stdarg.h>
#include "Common.h"

namespace ho
{
	//書式フォーマットからtstring型文字列を作成し、そのポインタを返す
	void tPrint(tstring &str, const TCHAR *format, ...)
	{
		va_list args; //可変長引数リスト
		va_start(args, format); //引数取得開始

		int Length = _vsctprintf(format, args); //書式の文字数を取得

		TCHAR *pTChar = new TCHAR[Length + 1]; //書式フォーマットによる出力用領域作成
		_vstprintf_s(pTChar, Length + 1, format, args); //書式フォーマットによる出力

		str = pTChar;

		SDELETE(pTChar);

		return;
	}

	//書式フォーマットからtstring型文字列を作成し、その実体を返す
	tstring tPrint(const TCHAR *format, ...)
	{
		va_list args; //可変長引数リスト
		va_start(args, format); //引数取得開始

		int Length = _vsctprintf(format, args); //書式の文字数を取得

		TCHAR *pTChar = new TCHAR[Length + 1]; //書式フォーマットによる出力用領域作成
		_vstprintf_s(pTChar, Length + 1, format, args); //書式フォーマットによる出力

		tstring str = pTChar;

		SDELETE(pTChar);

		return str;
	}

	//マルチバイト文字(char)をワイド文字(wchar)に変換して tstring 型に詰めて返す
	tstring CharToWChar(const char *pChar)
	{
		tstring strWChar; //return で返却する文字列

#if _UNICODE //Unicodeの場合
		setlocale(LC_ALL, "japanese"); //ロケールを指定

		size_t BufferSize = strlen(pChar) + 1; //バッファサイズ
		TCHAR *pBuffer = new TCHAR[BufferSize]; //受取り用のバッファ

		errno_t err = 0;
		size_t TransformedNumber = 0; //変換された文字数
		err = mbstowcs_s(&TransformedNumber, pBuffer, BufferSize, pChar, _TRUNCATE); //変換する

		strWChar = pBuffer; //変換した文字列を代入
		delete [] pBuffer;
#else //マルチバイトコードの場合
		strWChar = pChar; //変換せずにそのまま代入
#endif
		return strWChar;
	}

	//ワイド文字列(WCHAR*)をマルチバイト文字列(char*)に変換
	std::string WCharToChar(const TCHAR *pTChar)
	{
		std::string strChar; //returnで返却する文字列

#if _UNICODE //Unicodeの場合
		setlocale(LC_ALL, "japanese"); //ロケールを指定

		size_t BufferSize = _tcslen(pTChar) + 1; //バッファサイズ
		char *pBuffer = new char[BufferSize]; //受け取り用のバッファ

		errno_t err = 0;
		size_t TransformedNumber = 0; //変換された文字列
		err = wcstombs_s(&TransformedNumber, pBuffer, BufferSize, pTChar, _TRUNCATE); //変換する

		strChar = pBuffer; //変換した文字列を代入
		delete [] pBuffer;
#else //マルチバイトコードの場合
		strWChar = pChar; //変換せずにそのまま代入
#endif
		return strChar;
	}

	//任意の文字で区切る
	void Split(std::vector<tstring> &vectorStr, const tstring &str, const TCHAR &Key)
	{
		tstring spStr;

		for (tstring::const_iterator itr = str.begin(); itr != str.end(); itr++) //1文字ずつ走査
		{
			if (*itr == Key) //区切り文字に到達した場合
			{
				vectorStr.push_back(spStr); //分割文字までの文字列を配列に追加
				spStr.clear(); //文字列を消去
			} else {
				spStr.push_back(*itr); //1文字追加
			}
		}

		vectorStr.push_back(spStr); //最後の文字列も追加

		return;
	}

	//文字列を16進数として読み int 型に変換する
	__int64 atoi16(const tstring &str)
	{
		auto fTrans = [](const TCHAR &c)->__int64 //1文字を数字に変換する
		{
			switch (c)
			{
			case _T('0'):
				return 0;
				break;
			case _T('1'):
				return 1;
				break;
			case _T('2'):
				return 2;
				break;
			case _T('3'):
				return 3;
				break;
			case _T('4'):
				return 4;
				break;
			case _T('5'):
				return 5;
				break;
			case _T('6'):
				return 6;
				break;
			case _T('7'):
				return 7;
				break;
			case _T('8'):
				return 8;
				break;
			case _T('9'):
				return 9;
				break;
			case _T('a'):
			case _T('A'):
				return 10;
				break;
			case _T('b'):
			case _T('B'):
				return 11;
				break;
			case _T('c'):
			case _T('C'):
				return 12;
				break;
			case _T('d'):
			case _T('D'):
				return 13;
				break;
			case _T('e'):
			case _T('E'):
				return 14;
				break;
			case _T('f'):
			case _T('F'):
				return 15;
				break;
			}

			return -1; //0-9 a-f 以外の文字の場合
		};

		__int64 Number = 0;
		int Digit = 0; //桁数
		tstring::const_iterator itr = str.end();
		do
		{
			itr--;
			int Trans = (int)fTrans(*itr); //1文字を数字に変換する
			if (Trans >= 0) //正常に変換できた場合
			{
				Number += Trans * (__int64)pow(16.0, Digit); //文字を数字に変換して桁の数だけ掛けて加算
				Digit++; //桁を上げる
			}
		} while(itr != str.begin()); //先頭の文字ではない場合は続ける

		return Number;
	}

}

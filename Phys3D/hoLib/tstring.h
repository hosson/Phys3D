//文字列関係
#pragma once
#include <string>
#include <tchar.h>
#include <vector>

namespace ho
{
	typedef std::basic_string<TCHAR> tstring; //TCHAR型の可変文字列

	//書式フォーマットからtstring型文字列を作成し、そのポインタを返す
	void tPrint(tstring &str, const TCHAR *format, ...);

	//書式フォーマットからtstring型文字列を作成し、その実体を返す
	tstring tPrint(const TCHAR *format, ...);

	//マルチバイト文字(char)をワイド文字(wchar)に変換して tstring 型に詰めて返す
	tstring CharToWChar(const char *pChar);

	//ワイド文字列(wchar)をマルチバイト文字列(char)に変換
	std::string WCharToChar(const TCHAR *pTChar);

	//任意の文字で区切る
	void Split(std::vector<tstring> &vectorStr, const tstring &str, const TCHAR &Key);

	//文字列を16進数として読み int 型に変換する
	__int64 atoi16(const tstring &str);
}

/*
STL の 文字列クラスを TCHAR 型にしたもの。
可変長の文字列が扱えるので、基本的に文字列はこれを使う。
*/
#include "MultiLingual.h"
#include "LibError.h"
#include <Windows.h>

namespace ho
{
	const TCHAR MultiLingual::strNull = NULL;

	//コンストラクタ
	MultiLingual::MultiLingual()
	{
		vectorValidLocale.resize(_END, false);
		vectorStr.resize(_END);
	}

	//コンストラクタ
	MultiLingual::MultiLingual(const TCHAR *pStr, ...)
	{
		vectorValidLocale.resize(_END, true);
		vectorStr.resize(_END);

		va_list list;
		va_start(list, pStr);

		vectorStr.at(0) = pStr;
		for (DWORD i = 1; i < _END; i++)
			vectorStr.at(i) = va_arg(list, TCHAR *);

		va_end(list);

	}

	//あるロケールの文字列を設定
	void MultiLingual::SetStr(const TCHAR *pStr, eLocale Locale) 
	{
		if (Locale < 0 || Locale >= _END) //範囲外の場合
		{
			ER(TEXT("範囲外のロケールが指定されました。"), __WFILE__, __LINE__, true);
			return;
		}

		vectorStr.at(Locale) = pStr;
		vectorValidLocale.at(Locale) = true;

		return;
	}

	//全てのロケールの文字列を設定
	void MultiLingual::SetAllStr(const TCHAR *pStr, ...)
	{
		va_list list;
		va_start(list, pStr);

		vectorStr.at(0) = pStr;
		vectorValidLocale.at(0) = true;
		for (DWORD i = 1; i < _END; i++)
		{
			vectorStr.at(i) = va_arg(list, TCHAR);
			vectorValidLocale.at(i) = true;
		}

		va_end(list);

		return;
	}

	//ロケールを指定して文字列へのポインタを取得
	const TCHAR *MultiLingual::c_str(eLocale Locale)
	{
		if (Locale < 0 || Locale >= _END) //範囲外の場合
		{
			ER(TEXT("範囲外のロケールが指定されました。"), __WFILE__, __LINE__, true);
			return &strNull;
		}

		if (vectorValidLocale.at(Locale)) //ロケールに対応した文字列が設定されていない場合
			return &strNull;

		return vectorStr.at(Locale).c_str();
	}
}
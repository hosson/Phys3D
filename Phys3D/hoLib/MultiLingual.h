//多国後に対応した文字列リソースクラス
#pragma once
#include "tstring.h"
#include <vector>
#include <stdarg.h>

namespace ho
{
	class MultiLingual
	{
	public:
		enum eLocale { Japanese = 0, English, _END };
	public:
		MultiLingual(); //コンストラクタ
		MultiLingual(const TCHAR *pStr, ...); //コンストラクタ
		~MultiLingual() {} //デストラクタ

		void SetStr(const TCHAR *pStr, eLocale Locale); //あるロケールの文字列を設定
		void SetAllStr(const TCHAR *pStr, ...); //全てのロケールの文字列を設定
		const TCHAR *c_str(eLocale Locale); //ロケールを指定して文字列へのポインタを取得

		const static TCHAR strNull;
	private:
		std::vector<bool> vectorValidLocale; //文字列が有効なロケールを示す配列
		std::vector<tstring> vectorStr; //各ロケールの文字列配列
	};
}
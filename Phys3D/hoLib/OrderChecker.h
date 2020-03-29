//呼び出し順序をチェックするクラス
#pragma once
#include "tstring.h"
#include <boost/shared_ptr.hpp>

namespace ho
{
	class OrderChecker
	{
	public:
		OrderChecker(const int &AllCheckerNum, const tstring &strName); //コンストラクタ
		~OrderChecker(); //デストラクタ

		bool Check(const int &Num); //チェック実行
	private:
		const int AllCheckerNum; //全体のチェッカー数
		const tstring strName; //チェック対象の名称
		int NowCheckerNum; //現在のチェッカー番号
	};
}

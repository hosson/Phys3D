#include "OrderChecker.h"
#include "Error.h"

namespace ho
{
	//コンストラクタ
	OrderChecker::OrderChecker(const int &AllCheckerNum, const tstring &strName)
		: AllCheckerNum(AllCheckerNum), strName(strName)
	{
		this->NowCheckerNum = 0;
	}

	//デストラクタ
	OrderChecker::~OrderChecker()
	{
	}

	//チェック実行
	bool OrderChecker::Check(const int &Num)
	{
		if (this->NowCheckerNum == Num) //正常な場合
		{
			this->NowCheckerNum = (this->NowCheckerNum + 1) % this->AllCheckerNum; //現在のチェッカー番号を次へ移動
			return true; //正常終了
		}

		//異常な場合
		ERR(true, tPrint(TEXT("OrderChecker により %s の順序エラーが検出されました。\n%d 番の予定に対して実際には %d 番が呼ばれました。"), this->strName.c_str(), this->NowCheckerNum, Num), __WFILE__,__LINE__);

		return false; //エラー
	}
}


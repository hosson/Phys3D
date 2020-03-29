#include "Counter.h"

namespace ho
{
	//コンストラクタ
	Counter::Counter(const int MaxCountNum) : MaxCountNum(MaxCountNum)
	{
		this->CountNum = 0;
	}

	//デストラクタ
	Counter::~Counter()
	{
	}

	//カウントする
	bool Counter::Count()
	{
		CountNum++;
		if (CountNum >= MaxCountNum) //最大値に達した場合
		{
			CountNum = 0;
			return true;
		}

		return false;
	}

	//カウンタをリセット
	void Counter::Reset()
	{
		CountNum = 0;

		return;
	}

	//カウンタの値が最大値の範囲内の前半かどうか
	bool Counter::IsFirstHalf()
	{
		return CountNum < MaxCountNum * 0.5;
	}

	//カウンタの値が最大値の範囲内の後半かどうか
	bool Counter::IsSecondHalf()
	{
		return CountNum >= int(MaxCountNum * 0.5);
	}



	//コンストラクタ
	CounterF::CounterF(const double &MaxCountNum)
		: MaxCountNum(MaxCountNum)
	{
	}

	//デストラクタ
	CounterF::~CounterF()
	{
	}

	//カウントする
	bool CounterF::Count(const double &AddNum)
	{
		this->CountNum += AddNum;

		if (this->CountNum >= this->MaxCountNum)
		{
			this->CountNum -= this->MaxCountNum;
			return true; //最大値に到達した
		}

		return false;
	}

	//カウンタをリセット
	void CounterF::Reset()
	{
		this->CountNum = 0.0;

		return;
	}
}


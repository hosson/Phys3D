//カウンタクラス
#pragma once

namespace ho
{
	class Counter
	{
	public:
		Counter(const int MaxCountNum); //コンストラクタ
		~Counter(); //デストラクタ

		bool Count(); //カウントする
		void Reset(); //カウンタをリセット
		bool IsFirstHalf(); //カウンタの値が最大値の範囲内の前半かどうか
		bool IsSecondHalf(); //カウンタの値が最大値の範囲内の後半かどうか

		//アクセッサ
		int GetCountNum() const { return CountNum; }
	private:
		int CountNum; //カウンタの数字
		const int MaxCountNum; //カウンタの最大値
	};

	class CounterF
	{
	public:
		CounterF(const double &MaxCountNum); //コンストラクタ
		~CounterF(); //デストラクタ

		bool Count(const double &AddNum); //カウントする
		void Reset(); //カウンタをリセット
	private:
		double CountNum; //カウンタの現在の数字
		const double MaxCountNum; //カウンタの最大値
	};
}

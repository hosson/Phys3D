//数理的な便利関数
#pragma once

namespace ho
{
	namespace num
	{
		//変数の符号を返す
		template <typename T> int GetSign(const T &Value)
		{
			if (Value > 0)
				return 1;
			else if (Value < 0)
				return -1;

			return 0;
		}

		//変数の値を指定範囲内に収める
		template <typename T> void Clamp(T &Value, const T &Min, const T &Max)
		{
			if (Value < Min)
				Value = Min;
			else if (Value > Max)
				Value = Max;

			return;
		}

		//変数の値を指定範囲内に収める
		template <typename T> T Clamp(const T &Value, const T &Min, const T &Max)
		{
			if (Value < Min)
				return Min;
			else if (Value > Max)
				return Max;

			return Value;
		}

		//2つの変数の内容を交換する
		template <typename T> void Swap(T &a, T &b)
		{
			T Buf = a;
			a = b;
			b = Buf;

			return;
		}

		//2つの変数のうちの大きいほうの値を得る
		template <typename T> T Max(const T &a, const T &b)
		{
			if (a > b)
				return a;
			return b;
		}

		//2つの変数のうちの小さいほうの値を得る
		template <typename T> T Min(const T &a, const T &b)
		{
			if (a < b)
				return a;
			return b;
		}

		//ある値ValueがMin=0.0、Max=1.0としたとき、どの割合にあるかを得る
		template <typename T> double GetRatio(const T &Value, const T &Min, const T &Max)
		{
			return double(Value - Min) / double(Max - Min);
		}

		//任意のMinとMaxの値の中から、割合（0.0～1.0）をRatioとするときの値を得る
		template <typename T> T GetValueFromRatio(const double &Ratio, const T &Min, const T &Max)
		{
			return T(Min + (Max - Min) * Ratio);
		}
		//上記のfloat版
		template <typename T> T GetValueFromRatioF(const float &Ratio, const T &Min, const T &Max)
		{
			return T(Min + (Max - Min) * Ratio);
		}

	}
}
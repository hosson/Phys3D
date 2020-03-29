//同じ型の2つの変数を代入できるクラス
#pragma once
#include "LibError.h"

namespace ho
{
	template <class T> class Pair
	{
	public:
		Pair()
		{
			Enable[0] = Enable[1] = false;
		}
		Pair(const T *const value1, const T *const value2)
		{
			if (value1)
			{
				value[0] = *value1;
				Enable[0] = true;
			} else {
				Enable[0] = false;
			}
			if (value2)
			{
				value[1] = *value2;
				Enable[1] = true;
			} else {
				Enable[1] = false;
			}
		}
		Pair(const T &value1, const T &value2)
		{
			value[0] = value1;
			value[1] = value2;
			Enable[0] = true;
			Enable[1] = true;
		}

		~Pair() {}

		bool GetEqual()
		{
			if (!Enable[0] || !Enable[1])
			{
				ER(TEXT("無効な値を比較しようとしました。"), __WFILE__, __LINE__, false);
				return false;
			}

			return value[0] == value[1];
		}


		//アクセッサ
		T Get1() const
		{
			if (!Enable[0])
				ER(TEXT("無効な値を取得しようとしました。"), __WFILE__, __LINE__, false);
			return value[0];
		}
		T Get2() const
		{
			if (!Enable[1])
				ER(TEXT("無効な値を取得しようとしました。"), __WFILE__, __LINE__, false);
			return value[1];
		}
		T Get(int i) const
		{
			if (i < 0 || i > 1)
			{
				ER(TEXT("範囲外のインデックスが渡されました。"), __WFILE__, __LINE__, false);
				value[0];
			}
			if (!Enable[i])
				ER(TEXT("無効な値を取得しようとしました。"), __WFILE__, __LINE__, false);

			return value[i];
		}
	private:
		T value[2];
		bool Enable[2];
	};

}
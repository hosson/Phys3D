//組み合わせ（総当り)クラス
#pragma once
#include <vector>
#include "LibError.h"
#include "Math.h"

namespace ho
{
	template <class T> class Combination
	{
	public:
		//コンストラクタ（初期化なし）
		Combination(unsigned int Numbers)
		{
			this->Numbers = Numbers;
			vectorCell.resize(Numbers * Numbers);
		}

		//コンストラクタ（初期化あり）
		Combination(unsigned int Numbers, const T &Init)
		{
			this->Numbers = Numbers;
			vectorCell.resize(Numbers * Numbers, Init);
		}

		//デストラクタ
		~Combination()
		{
		}

		//任意の要素数の組み合わせの値を取得
		T Get(unsigned int Number1, unsigned int Number2)
		{
#ifdef _DEBUG
			if (Number1 >= Numbers || Number2 >= Numbers)
				ER(TEXT("引数の要素数が範囲外です。"), __WFILE__, __LINE__, true);
#endif
			if (Number1 > Number2)
				ho::Swap(Number1, Number2);

			return vectorCell.at(Number1 * Numbers + Number2);
		}
		//任意の要素数の組み合わせの値を設定
		void Set(unsigned int Number1, unsigned int Number2, const T &Value)
		{
#ifdef _DEBUG
			if (Number1 >= Numbers || Number2 >= Numbers)
				ER(TEXT("引数の要素数が範囲外です。"), __WFILE__, __LINE__, true);
#endif
			if (Number1 > Number2)
				ho::Swap(Number1, Number2);

			vectorCell.at(Number1 * Numbers + Number2) = Value;

			return;
		}

	private:
		unsigned int Numbers; //組み合わせの元となる数字の数
		std::vector<T> vectorCell; //それぞれの組み合わせ情報
	};
}
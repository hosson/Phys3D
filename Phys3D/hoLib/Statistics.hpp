//統計に関する処理
#pragma once
#include <vector>
#include <algorithm>

namespace ho
{
	//0.0～1.0の比率から、配列全体を直列に合計した中の何番目の要素に含まれるかを得る
	template <class T> unsigned int GetElementFromRatio(std::vector<T> &vector, const double &Ratio)
	{
		T Total = 0;
		std::for_each(vector.begin(), vector.end(), [&](T v){ Total += v; }); //配列の合計値を取得

		T TargetValue = Ratio * Total; //比率を、配列合計を1.0とみなした値に変換

		Total = 0;
		for (unsigned int i = 0; i < vector.size(); i++) //配列を走査
		{
			Total += vector.at(i);
			if (TargetValue <= Total)
				return i;
		}

		return vector.size();
	}
}

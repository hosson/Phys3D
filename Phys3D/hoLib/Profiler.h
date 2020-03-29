//パフォーマンス解析クラス
#pragma once
#include <vector>
#include <Windows.h>

namespace ho
{
	class Profiler
	{
	public:
		Profiler(unsigned int Num); //コンストラクタ
		~Profiler(); //デストラクタ

		void Begin(unsigned int Index); //計測開始
		double End(unsigned int Index); //計測終了
		double GetTime(unsigned int Index); //計測した時間を取得
	private:
		std::vector<double> vectorTime; //タイマーで計測した時間
		std::vector<LARGE_INTEGER> vectorqpc; //計測開始時間
		LARGE_INTEGER Freq; //計測単位
	};
}

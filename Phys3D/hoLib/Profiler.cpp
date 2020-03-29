#include "Profiler.h"
#include "Error.h"

namespace ho
{
	//コンストラクタ
	Profiler::Profiler(unsigned int Num)
	{
		vectorTime.resize(Num);
		vectorqpc.resize(Num);

		if (!QueryPerformanceFrequency(&Freq)) //高分解能パフォーマンスカウンタの周波数を取得し、ハードウェアがサポートしていない場合
			Freq.QuadPart = 0;
	}

	//デストラクタ
	Profiler::~Profiler()
	{
	}

	//計測開始
	void Profiler::Begin(unsigned int Index)
	{
		if (vectorTime.size() <= Index)
			ERR(true, TEXT("範囲外のインデックスが渡されました。"), __WFILE__, __LINE__);

		QueryPerformanceCounter(&vectorqpc.at(Index)); //計測開始

		return;
	}

	//計測終了
	double Profiler::End(unsigned int Index)
	{
		if (vectorTime.size() <= Index)
			ERR(true, TEXT("範囲外のインデックスが渡されました。"), __WFILE__, __LINE__);

		LARGE_INTEGER EndTime;
		QueryPerformanceCounter(&EndTime); //計測終了

		vectorTime.at(Index) = (double(EndTime.QuadPart - vectorqpc.at(Index).QuadPart) / (double)Freq.QuadPart) * 1000.0; //ms単位で格納

		return vectorTime.at(Index);
	}

	//計測した時間を取得
	double Profiler::GetTime(unsigned int Index)
	{
		if (vectorTime.size() <= Index)
		{
			ERR(true, TEXT("範囲外のインデックスが渡されました。"), __WFILE__, __LINE__);
		} else {
			return vectorTime.at(Index);
		}

		return 0;
	}
}

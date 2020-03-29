#include "Random.h"
#include "Error.h"
#define PI 3.14159265358979

namespace ho
{
	//標準ランタイムライブラリのランダム関数を初期化
	void Randomize()
	{
		SYSTEMTIME st;
		unsigned int TotalTime;	GetLocalTime(&st);
		TotalTime = st.wDay + st.wHour + st.wMinute + st.wSecond + st.wMilliseconds;
		srand(TotalTime);

		return;
	}

	//任意の範囲のランダムの値を取得する
	double GetRand(const double &Min, const double &Max)
	{
		return double((rand() * (double(Max - Min) / (double)RAND_MAX)) + Min);
	}

	//任意の範囲のランダムの値を取得する
	int GetRand(const int Min, const int Max)
	{
		if (Min >= Max)
		{
			ERR(false, TEXT("ランダム関数の引数の値が不正です。"), __WFILE__, __LINE__);
		} else if (Max - Min < 0)
		{
			ERR(true, TEXT("ランダム関数の引数の値が不正です。"), __WFILE__, __LINE__); //続行不能
		}

		return (rand() % (Max - Min + 1)) + Min;
	}

	//正規分布のランダム値を取得（ボックス・ミューラー法）
	double GetBMRand(const double &Center, const double &Width)
	{
		double r1 = rand() / 32767.1, r2 = rand() / 32767.1;

		return Width * sqrt(-2.0 * log(r1)) * cos(2.0 * PI * r2) + Center;
	}




	//コンストラクタ
	xorShift::xorShift()
	{
		this->getNum = 0;
		this->initNum = 0;

		Init(); //種を初期化する
	}

	//デストラクタ
	xorShift::~xorShift() 
	{
	}

	//種を初期化する
	void xorShift::Init() 
	{
		this->initNum++;

		this->x = 123456789;
		this->y = 362436069;
		this->z = 521288629;
		this->w = 88675123;

		return;
	}

	//乱数を得る
	unsigned long xorShift::get()
	{
		this->getNum++;

		unsigned long t; 
		t = (x ^ (x << 11));
		x = y;
		y = z;
		z = w;

		return (w = (w ^ (w >> 19)) ^ (t ^ (t >>8 ))); 
	}

	//範囲を指定して乱数値を得る
	int xorShift::get(const int &Min, const int &Max)
	{
		if (Min >= Max)
		{
			ERR(false, TEXT("ランダム関数の引数の値が不正です。"), __WFILE__, __LINE__);
		} else if (Max - Min < 0)
		{
			ERR(true, TEXT("ランダム関数の引数の値が不正です。"), __WFILE__, __LINE__); //続行不能
		}

		return (this->get() % (Max - Min + 1)) + Min;
	}

	//範囲を指定して乱数値を得る
	double xorShift::get(const double &Min, const double &Max)
	{
		return this->get() * (double(Max - Min) / (double)UINT_MAX) + Min;
	}

	//ボックス・ミューラー法による正規分布の乱数値を得る
	double xorShift::getBM(const double &Center, const double &Width)
	{
		double r1 = this->get() / ((double)UINT_MAX + 1.0), r2 = this->get() / ((double)UINT_MAX + 1.0);

		return Width * sqrt(-2.0 * log(r1)) * cos(2.0 * PI * r2) + Center;
	}

}

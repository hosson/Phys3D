//ランダム値に関する処理
#pragma once
#include <Windows.h>

namespace ho
{
	void Randomize(); //標準ランタイムライブラリのランダム関数を初期化

	double GetRand(const double &Min, const double &Max); //任意の範囲のランダムの値を取得する
	int GetRand(const int Min, const int Max); //任意の範囲のランダムの値を取得する

	double GetBMRand(const double &Center, const double &Width); //正規分布のランダム値を取得（ボックス・ミューラー法）

	//xorShift 法による乱数生成
	class xorShift
	{
	public:
		xorShift(); //コンストラクタ
		~xorShift(); //デストラクタ

		void Init(); //種を初期化する
		unsigned long get(); //unsigned long 型の乱数値を直接得る

		int get(const int &Min, const int &Max); //範囲を指定して乱数値を得る
		double get(const double &Min, const double &Max); //範囲を指定して乱数値を得る
		double getBM(const double &Center, const double &Width); //ボックス・ミューラー法による正規分布の乱数値を得る

		//アクセッサ
		unsigned int getgetNum() const { return getNum; }
	private:
		unsigned long x, y, z, w;
		unsigned int getNum; //get()が呼び出された回数
		unsigned int initNum; //Init()が呼び出された回数
	};
}

//ボックス・ミューラー法の正規分布（参考）
//http://ja.wikipedia.org/wiki/%E3%83%95%E3%82%A1%E3%82%A4%E3%83%AB:Standard_deviation_diagram.svg
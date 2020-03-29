//数学関連のライブラリ
#pragma once
//#include <d3dx9math.h>
#include "D3DVertex.h"
#include <stdlib.h>
#include <list>
#include <stdarg.h>
#include "Debug.h"

#define PI 3.14159265358979323846 //円周率
#define PI2 6.283185307179586476925286766559 //円周率*2
#define PI_HALF 1.5707963267948966192313216916398 //円周率*0.5
          
namespace ho {
	//前方宣言
	class VECTOR2DOUBLE;
	class VECTOR3DOUBLE;

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

	//ある値Valueが始点Beginと終点Endの範囲内に存在するかどうかを調べる
	template <typename T> bool JudgeScope(const T &Begin, const T &End, const T &Value) 
	{
		if (Begin < End)
		{
			if (Value >= Begin && Value <= End)
				return true; //範囲内
		} else if (Begin > End) {
			if (Value >= End && Value <= Begin)
				return true; //範囲内
		} else { //BeginとEndが等しい場合
			if (Value == Begin) //2つの等しい点上にValueも存在する場合
				return true; //範囲内と解釈
		}

		return false; //範囲外
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

	//奇数かどうかを判定
	bool Odd(const int &val);
	bool Odd(const unsigned int &val);

	//偶数かどうかを判定
	bool Even(const int &val);
	bool Even(const unsigned int &val);

	//値を 0.0～2π までの位相に変換する
	float PhaseLimit(const float &Phase);
	void PhaseLimit(float &Phase);
	double PhaseLimit(const double &Phase);
	void PhaseLimit(double &Phase);

	//角度クラス
	//このクラスを使うと角度の値が必ず0.0～2πの間になる
	template <class T> class ANGLE
	{
	public:
		ANGLE() {} //コンストラクタ
		ANGLE(const T &Angle) //コンストラクタ
		{
			this->a = PhaseLimit(Angle);
		}
		~ANGLE() {} //デストラクタ

		//アクセッサ
		T Get() const { return a; }

		//演算子
		ANGLE operator +(const ANGLE &obj) const { return ANGLE(this->a + obj.a); }
		ANGLE& operator +=(const ANGLE &obj) { this->a = PhaseLimit(this->a + obj.a); return *this; }

		ANGLE operator -(const ANGLE &obj) const { return ANGLE(this->a - obj.a); }
		ANGLE& operator -=(const ANGLE &obj) { this->a = PhaseLimit(this->a - obj.a); return *this; }

		ANGLE operator *(const int &value) const { return ANGLE(this->a * value); }
		ANGLE operator *(const float &value) const { return ANGLE(this->a * value); }
		ANGLE operator *(const double &value) const { return ANGLE(this->a * value); }
		ANGLE& operator *=(const int &value) { this->a = PhaseLimit(this->a * value); return *this; }
		ANGLE& operator *=(const float &value) { this->a = PhaseLimit(this->a * value); return *this; }
		ANGLE& operator *=(const double &value) { this->a = PhaseLimit(this->a * value); return *this; }

		ANGLE operator /(const int &value) const { return ANGLE(this->a / value); }
		ANGLE operator /(const float &value) const { return ANGLE(this->a / value); }
		ANGLE operator /(const double &value) const { return ANGLE(this->a / value); }
		ANGLE& operator /=(const int &value) { this->a = PhaseLimit(this->a / value); return *this; }
		ANGLE& operator /=(const float &value) { this->a = PhaseLimit(this->a / value); return *this; }
		ANGLE& operator /=(const double &value) { this->a = PhaseLimit(this->a / value); return *this; }

		//減算し、短距離で到達できる方向の角度を返す
		T operator |(const ANGLE &obj) const
		{
			T Sub = this->a - obj.a; //減算して位相制限にかける
			if (Sub < -PI) //0度の位置をまたぐ場合の処置
				PhaseLimit(Sub);
			else if (Sub > PI) //180度より大きな角度になってしまった場合
				Sub = Sub - PI2;

			return Sub;
		}
	private:
		T a; //角度の値
	};

	//比率型（値を 0.0～1.0 の範囲に限定する）
	class RATIO
	{
	public:
		RATIO() { this->val = 0.0; } //コンストラクタ
		RATIO(const double &val) { this->val = val; } //コンストラクタ
		~RATIO() {} //デストラクタ

		double val;

		RATIO operator +(const RATIO &obj) const { return RATIO(Clamp(this->val + obj.val, 0.0, 1.0)); }
		RATIO& operator +=(const RATIO &obj) { this->val = Clamp(this->val + obj.val, 0.0, 1.0); return *this; }
		RATIO operator -(const RATIO &obj) const { return RATIO(Clamp(this->val - obj.val, 0.0, 1.0)); }
		RATIO& operator -=(const RATIO &obj) { this->val = Clamp(this->val - obj.val, 0.0, 1.0); return *this; }
		RATIO operator *(const RATIO &obj) const { return RATIO(Clamp(this->val * obj.val, 0.0, 1.0)); }
		RATIO& operator *=(const RATIO &obj) { this->val = Clamp(this->val * obj.val, 0.0, 1.0); return *this; }
		RATIO operator /(const RATIO &obj) const { return RATIO(Clamp(this->val / obj.val, 0.0, 1.0)); }
		RATIO& operator /=(const RATIO &obj) { this->val = Clamp(this->val / obj.val, 0.0, 1.0); return *this; }

		double operator ()() const { return val; }
	};

	int GetVectorPosFromXY(int Width, int X, int Y); //1次元配列を2次元配列と見なした場合に、XとYの座標から1次元配列上の要素数を得る
	PT<int> GetXYFromVectorPos(int Width, int VectorPos); //1次元配列を2次元配列と見なした場合に、1次元配列上の要素数からXとYの座標を得る

	//sin関数テーブル
	class SinTable
	{
	public:
		static const float Ratio; // 65536 / (2 * PI)
		static float Table[65536]; //テーブルデータ

		static BOOL Init(); //初期化
		static BOOL Dest(); //終了処理
	};

	class PLANE //一般平面方程式
	{
	public:
		enum Param { X, Y, Z }; //係数の項を表す列挙体

		double a, b, c, d; //ax + by + cz + d = 0 の形となる

		PLANE() {} //コンストラクタ
		PLANE(const VECTOR3DOUBLE &vec1, const VECTOR3DOUBLE &vec2, const VECTOR3DOUBLE &vec3); //コンストラクタ（3点の座標から一般平面方程式を求める）
		PLANE(const double &a, const double &b, const double &c, const double &d); //コンストラクタ（パラメータを直接指定）
		PLANE(const D3DXVECTOR3 &vecNormal, const double &d); //コンストラクタ（法線ベクトルと原点からの距離）

		VECTOR3DOUBLE GetNormalLine() const;//平面方程式から平面の法線ベクトルを得る
		bool GetCrossPointFromLine(VECTOR3DOUBLE &out, const VECTOR3DOUBLE &vecPt1, const VECTOR3DOUBLE &vecPt2) const; //2つの頂点で示される直線との交点を求める
		Param GetMaxAbsParam(); //X, Y, Z の中から係数の絶対値が最も大きい項がどれかを調べる
		double GetOneAxisMoveVector(Param InVector, const double &In, Param OutVector) const; //平面上で任意の軸上を任意の量動かした場合に、別のある任意の軸の動く量を得る

		static bool GetIntersectionLine(VECTOR3DOUBLE &vecDirection, VECTOR3DOUBLE &vecPoint, const PLANE &pln1, const PLANE &pln2); //2つの平面方程式の交線を求める
	};

	/* 線形グラフは、複数の点で構成される2次元のグラフである。
	Add() で要素を追加していくと、Get() で任意のx値に対応する
	yの値を線形補間しながら得ることができる。*/
	template <class T, class U> class LinearGraph //線形グラフ
	{
	public:
		struct POINT //一要素を表す構造体
		{
			T x; //横軸の値
			U y; //縦軸の値
		};
	public:
		LinearGraph(const U &Default) : Default(Default), Zero(Default) {} //コンストラクタ
		LinearGraph(const U &Default, const U &Zero) : Default(Default), Zero(Zero) {} //コンストラクタ
		~LinearGraph() {} //デストラクタ

		//要素を追加
		void Add(const T &x, const U &y)
		{
			for (std::list<POINT>::iterator itr = listPoint.begin(); itr != listPoint.end();) //リストを走査
			{
				if (itr->x == x) //x値が重複する場合
					itr = listPoint.erase(itr); //リスト内の要素を削除
				else
					itr++;
			}

			POINT Point;
			Point.x = x;
			Point.y = y;

			listPoint.push_back(Point); //リストに追加

			//x値の小さい順にソート
			listPoint.sort([](const POINT &Point1, const POINT &Point2) { return Point1.x < Point2.x; });

			return;
		}

		//任意のxに対応するyの値を取得
		U Get(const T &x) const
		{
			if (!listPoint.size()) //要素が存在しない場合
				return Default; //デフォルト値が返る

			for (std::list<POINT>::const_iterator itr = listPoint.begin(); itr != listPoint.end(); itr++) //リストを走査
			{
				if (x < itr->x) //走査中の要素よりx値が小さくなった場合
				{
					if (itr == listPoint.begin()) //最初の要素の場合
						return itr->y;

					//現在の一つ前のイテレータを取得
					std::list<POINT>::const_iterator itrPrev = itr;
					itrPrev--;

					double Ratio = double(x - itrPrev->x) / double(itr->x - itrPrev->x); //2つの要素間の比率

					return itrPrev->y + (itr->y - itrPrev->y) * Ratio; //線形補間
				}
			}

			//全ての要素よりxが大きい場合
			std::list<POINT>::const_iterator itr = listPoint.end();
			itr--;
			return itr->y; //最後の要素のy値を返しておく
		}
		//最初のポイントを得る
		ho::Enable<POINT> GetFrontPoint() const
		{
			if (listPoint.size() == 0) //ポイントが存在しない場合
				return ho::Enable<POINT>(); //無効値を返しておく

			return ho::Enable<POINT>(listPoint.front());
		}
		//最後のポイントを得る
		ho::Enable<POINT> GetBackPoint() const
		{
			if (listPoint.size() == 0) //ポイントが存在しない場合
				return ho::Enable<POINT>(); //無効値を返しておく

			return ho::Enable<POINT>(listPoint.back());
		}
		//区間積分を行う
		U GetIntervalIntegration(const T &Begin, const T &End) const
		{
			U Area = Zero; //面積

			try
			{
				if (Begin > End) //始点の方が大きい場合
					throw(TEXT("積分区間が不正です"), __WFILE__, __LINE__, false);
				if (Begin == End) //始点と終点が同じ位置の場合
					return Zero;

				if (!listPoint.size()) //要素が存在しない場合
					return Default * (End - Begin); //デフォルト値で積分した値が返る

				bool Integration = false; //積分中かどうか
				for (std::list<POINT>::const_iterator itr = listPoint.begin(); itr != listPoint.end(); itr++) //リストを走査
				{
					if (Integration) //積分中の場合
					{
						if (End <= itr->x) //走査中の要素より終わりの値が小さくなった場合
						{
							std::list<POINT>::const_iterator itrPrev = itr;
							itrPrev--;
							Area += GetArea(*itrPrev, *itr, ho::Enable<T>(), ho::Enable<T>(End)); //面積を計算
							Integration = false; //積分を終了
							break;
						} else { //始まりの値も終わりの値も含まれない場合
							std::list<POINT>::const_iterator itrPrev = itr;
							itrPrev--;
							Area += GetArea(*itrPrev, *itr, ho::Enable<T>(), ho::Enable<T>()); //面積を計算
						}
					} else { //まだ積分が始まっていない場合
						if (Begin <= itr->x) //走査中の要素より始まりの値が小さくなった場合
						{
							if (End <= itr->x) //走査中の要素より終わりの値も小さくなった場合
							{
								if (itr == listPoint.begin()) //最初の要素の場合
								{
									Area += GetArea(ho::Enable<POINT>(), *itr, ho::Enable<T>(Begin), ho::Enable<T>(End)); //面積を計算
									break;
								} else {
									std::list<POINT>::const_iterator itrPrev = itr;
									itrPrev--;
									Area += GetArea(*itrPrev, *itr, ho::Enable<T>(Begin), ho::Enable<T>(End)); //面積を計算
									break;
								}
							} else { //終わりの値は含まれていない場合
								if (itr == listPoint.begin()) //最初の要素の場合
								{
									Area += GetArea(ho::Enable<POINT>(), *itr, ho::Enable<T>(Begin), ho::Enable<T>()); //面積を計算
								} else {
									std::list<POINT>::const_iterator itrPrev = itr;
									itrPrev--;
									Area += GetArea(*itrPrev, *itr, ho::Enable<T>(Begin), ho::Enable<T>()); //面積を計算
								}
								Integration = true; //積分中とする
							}
						}
					}
				}

				if (Integration) //まだ終ライン分まで積分していないのにリストが終わってしまった場合
				{
					std::list<POINT>::const_iterator itr = listPoint.end();
					itr--; //最後の要素のイテレータを取得
					Area += GetArea(*itr, ho::Enable<POINT>(), ho::Enable<T>(), ho::Enable<T>(End)); //面積を計算
				}
			}
			catch(const ho::Exception &e)
			{
				ER(e.strMsg.c_str(), e.pFile, e.Line, e.Continue);
			}

			return Area;
		}

		//アクセッサ
		const std::list<POINT> &GetlistPoint() const { return listPoint; }
	private:
		const U Default; //要素が存在しない場合に返すデフォルトy値
		const U Zero; //計算開始時などに0初期化したい場合の0に該当するy値
		std::list<POINT> listPoint; //要素リスト

		//BeginPointとEndPointで構成される台形の、BeginからEndまでの面積を計算する
		U GetArea(const ho::Enable<POINT> &BeginPoint, const ho::Enable<POINT> &EndPoint, const ho::Enable<T> &BeginLine, const ho::Enable<T> &EndLine) const
		{
			//ライン位置のポイントを返すラムダ式
			auto fGetPointFromLine = [&](const ho::Enable<T> &Line)->POINT
			{
				POINT Point;
				Point.x = Line();
				Point.y = this->Get(Line());

				return Point;
			};

			POINT Left, Right; //台形を構成する左と右の計算用ポイント

			try
			{
				if (BeginLine.operator!() && EndLine.operator!()) //始まりラインと終わりラインが指定されている場合
					if (BeginLine() >= EndLine()) //始まりラインが終わりラインと同じか右側の場合
						throw(TEXT("積分区間が不正です。"), __WFILE__, __LINE__, false);
	
				//左の計算用ポイントに関する処理
				if (BeginPoint.operator!()) //左のポイントが有効の場合
				{
					if (EndLine.operator!()) //右のラインが有効な場合
						if (EndLine() < BeginPoint().x) //右のラインが左のポイントよりも左にある場合
							throw(TEXT("積分区間が不正です。"), __WFILE__, __LINE__, false);
					if (BeginLine.operator!()) //左のラインも有効の場合
					{
						if (BeginLine() < BeginPoint().x) //ラインがポイントより左にある場合
						{
							Left = BeginPoint(); //ポイントを始点とする
						} else {
							Left = fGetPointFromLine(BeginLine); //ライン位置を始点とする
						}
					} else { //左のラインが無効の場合
						Left = BeginPoint(); //ポイントを始点とする
					}
				} else { //左のポイントが無効の場合
					if (BeginLine.operator!()) //左のラインが有効の場合
					{
						Left = fGetPointFromLine(BeginLine); //ライン位置を始点とする
					} else { //左のラインが無効の場合
						throw(TEXT("積分区間が不正です。"), __WFILE__, __LINE__, false);
					}
				}
	
				//右の計算用ポイントに関する処理
				if (EndPoint.operator!()) //右のポイントが有効の場合
				{
					if (BeginLine.operator!()) //左のラインが有効な場合
						if (BeginLine() > EndPoint().x) //左のラインが右のポイントよりも右にある場合
							throw(TEXT("積分区間が不正です。"), __WFILE__, __LINE__, false);
					if (EndLine.operator!()) //右のラインも有効の場合
					{
						if (EndLine() > EndPoint().x) //ラインがポイントより右にある場合
						{
							Right = EndPoint(); //ポイントを始点とする
						} else {
							Right = fGetPointFromLine(EndLine); //ライン位置を終点とする
						}
					} else { //右のラインが無効の場合
						Right = EndPoint(); //ポイントを終点とする
					}
				} else { //右のポイントが無効の場合
					if (EndLine.operator!()) //右のラインが有効の場合
					{
						Right = fGetPointFromLine(EndLine); //ライン位置を終点とする
					} else { //右のラインが無効の場合
						throw(TEXT("積分区間が不正です。"), __WFILE__, __LINE__, false);
					}
				}
			}
			catch(const ho::Exception &e)
			{
				ER(e.strMsg.c_str(), e.pFile, e.Line, e.Continue);
			}

			return (Left.y + Right.y) * (Right.x - Left.x) * 0.5;
		}
	};

	//線形2次元グラフを即席で作成し、そこから値を得る
	template <class T, class U> U GetLinearGraph(const T &x, DWORD Num, ...)
	{
		/* 引数 x は取得したい値のグラフ上のx軸
		Num はグラフ上の点の数
		以後の可変長引数は、x 軸 y軸 の2つの変数を Num 個分続けて入力する。
		class U に unsigned な型を指定すると計算過程でマイナスを扱う都合上おかしくなるので不可。
		*/

		va_list arg;
		va_start(arg, Num);

		T X[2];
		U Y[2];
		for (DWORD i = 0; i < Num; i++)
		{
			X[i % 2] = va_arg(arg, T);
			Y[i % 2] = va_arg(arg, U);
			if (x <= X[i % 2])
			{
				va_end(arg);

				if (i == 0)
				{
					return Y[0];
				} else {
					if (X[i % 2] - X[(i - 1) % 2] <= 0) //X軸の並びが適正ではない場合
						return U(Y[i % 2]); //適当に返す
					return U(Y[(i - 1) % 2] + (Y[i % 2] - Y[(i - 1) % 2]) * ((x - X[(i - 1) % 2]) / double(X[i % 2] - X[(i - 1) % 2])));
				}
			}
		}

		va_end(arg);

		return Y[(Num - 1) % 2];
	}
	template <> void GetLinearGraph<double, DWORD>(const double &x, DWORD Num, ...); //指定してはいけない型を特殊化
	template <> void GetLinearGraph<float, DWORD>(const float &x, DWORD Num, ...); //指定してはいけない型を特殊化
	template <> void GetLinearGraph<int, DWORD>(const int &x, DWORD Num, ...); //指定してはいけない型を特殊化
	template <> void GetLinearGraph<unsigned int, DWORD>(const unsigned int &x, DWORD Num, ...); //指定してはいけない型を特殊化
	template <> void GetLinearGraph<DWORD, DWORD>(const DWORD &x, DWORD Num, ...); //指定してはいけない型を特殊化

	//水平な線分のみで構成される2次元グラフを即席で作成し、そこから値を得る
	namespace eJust //xの値が点と一致した場合の挙動を表す列挙対
	{
		enum e
		{
			Truncation,	//切捨て
			Conclusion,	//切上げ
			Average,	//平均値
		};
	};
	template <class T, class U> U GetHorizontalLinearGraph(T x, eJust::e Just, DWORD Num, ...)
	{
		/* 引数 x は取得したい値のグラフ上のx軸
		Num はグラフ上の点の数
		以後の可変長引数は、y軸の高さ、x軸の区切り、y軸の高さの順で入れる。
		引数 Num の数だけ x軸の区切りが入り、最後は必ずy軸の高さで終わる。
		*/

		va_list arg;
		va_start(arg, Num);

		T X;
		U Y;

		switch (Just)
		{
		case eJust::Truncation: //切捨て
			for (DWORD i = 0; i < Num; i++)
			{
				Y = va_arg(arg, U);
				X = va_arg(arg, T);
				if (x <= X)
				{
					va_end(arg);
					return Y;
				}
			}
			Y = va_arg(arg, U);
			va_end(arg);
			return Y;
			break;
		case eJust::Conclusion: //切上げ
			for (DWORD i = 0; i < Num; i++)
			{
				Y = va_arg(arg, U);
				X = va_arg(arg, T);
				if (x < X)
				{
					va_end(arg);
					return Y;
				}
			}
			Y = va_arg(arg, U);
			va_end(arg);
			return Y;
			break;
		case eJust::Average: //平均
			for (DWORD i = 0; i < Num; i++)
			{
				Y = va_arg(arg, U);
				X = va_arg(arg, T);
				if (x <= X)
				{
					va_end(arg);
					return Y;
				} else if (x == X)
				{
					U Y2 = va_arg(arg, U);
					va_end(arg);
					return U((Y + Y2) * 0.5);
				}
			}
			Y = va_arg(arg, U);
			va_end(arg);
			return Y;
			break;
		}

		ER(TEXT("値が一致した場合の処理が指定されていませんでした。"), __WFILE__, __LINE__, false);

		return 0;
	}
}

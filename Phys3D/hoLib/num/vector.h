//ベクトル関連のクラス
#pragma once
#include <math.h>
#include "../Direction.h"
#include "Matrix.h"

#define USE_CONVERT_DIRECTX_MATH 
/*DirectXMath へのコンバータを含むオプション。
DirectXMath を使用しない環境の場合はこの定義をコメントアウトする。*/


#ifdef USE_CONVERT_DIRECTX_MATH
#include <DirectXMath.h>
using namespace DirectX;
#endif

namespace ho
{
	namespace num
	{
		//前方宣言
		class vec3d;
		class Matrix;

		//double型の2次元ベクトル
		class vec2d
		{
		public:
			//コンストラクタ
			vec2d() { this->x = this->y = 0.0; }
			vec2d(const double &x, const double &y) { this->x = x; this->y = y; }

			double x, y; //メンバ変数

			//演算子
			vec2d operator +(const vec2d &obj) const { return vec2d(this->x + obj.x, this->y + obj.y); }
			vec2d& operator +=(const vec2d &obj) { this->x += obj.x; this->y += obj.y; return *this; }
			static void Add(vec2d &out, const vec2d &vec1, const vec2d &vec2) { out.x = vec1.x + vec2.x; out.y = vec1.y + vec2.y; }
			vec2d operator -(const vec2d &obj) const { return vec2d(this->x - obj.x, this->y - obj.y); }
			vec2d& operator -=(const vec2d &obj) { this->x -= obj.x; this->y -= obj.y; return *this; }
			static void Sub(vec2d &out, const vec2d &vec1, const vec2d &vec2) { out.x = vec1.x - vec2.x; out.y = vec1.y - vec2.y; }
			vec2d operator *(const double &value) const { return vec2d(this->x * value, this->y * value); }
			vec2d& operator *=(const double &value) { this->x *= value; this->y *= value; return *this; }
			static void Mul(vec2d &out, const vec2d &vec1, const vec2d &vec2) { out.x = vec1.x * vec2.x; out.y = vec1.y * vec2.y; }
			vec2d operator /(const double &value) const { return vec2d(this->x / value, this->y / value); }
			vec2d& operator /=(const double &value) { this->x /= value; this->y /= value; return *this; }
			static void Div(vec2d &out, const vec2d &vec1, const vec2d &vec2) { out.x = vec1.x / vec2.x; out.y = vec1.y / vec2.y; }
			vec2d operator -() { return vec2d(-this->x, -this->y); }
			bool operator ==(const vec2d obj) const { return(this->x == obj.x && this->y == obj.y); }
			bool operator !=(const vec2d obj) const { return(!(*this == obj)); }

#ifdef USE_CONVERT_DIRECTX_MATH
			//DirectXMath とのコンバータ
			vec2d(const XMFLOAT2 &obj) { this->x = obj.x; this->y = obj.y; }
			operator XMFLOAT2() const { return XMFLOAT2(float(this->x), float(this->y)); }
#endif

			//正規化したベクトルを得る
			vec2d GetNormalize() const
			{
				double Denominator = sqrt(x * x + y * y); //分母を計算

				if (!Denominator) //分母が0の場合
					return vec2d(0, 0);

				Denominator = 1.0 / Denominator; //逆数を計算しておく（後の除算を1回にするため）

				return vec2d(x * Denominator, y * Denominator);
			}

			double GetPower() const { return sqrt(x * x + y * y); } //ベクトルの量を計算する
			void clear() { this->x = this->y = 0.0; } //0に初期化

			//回転させる（PC座標系では時計回り、数学座標系では反時計回り）
			vec2d GetRotation(const double &Angle) const
			{
				return vec2d(x * cos(Angle) - y * sin(Angle), x * sin(Angle) + y * cos(Angle));
			}

			//ヘロンの公式による面積を求める
			static double GetArea(const vec2d &v1, const vec2d &v2, const vec2d &v3)
			{
				double Len[3] = { GetDistance(v1, v2), GetDistance(v2, v3), GetDistance(v3, v1) }; //頂点間の距離
				double s = 0.5 * (Len[0] + Len[1] + Len[2]);
				double ToSqrt = s * (s - Len[0]) * (s - Len[1]) * (s - Len[2]); //sqrt関数に入れる前段階の値
				if (ToSqrt < 0) //sqrtに負の値を入れると正常に求まらないため
					return 0;
				return sqrt(ToSqrt);
			}

			//右方向を0度とした場合の反時計回りの角度を求める
			double GetAngle() const { return atan2(this->y, this->x); }

			//任意の方向ベクトルを0度とした場合の反時計回りの角度を求める
			double GetAngle(const vec2d &vec) const
			{
				return GetAngle() - vec.GetAngle();
			}

			vec3d Getvec3d_XY(const double &z = 0.0) const; //XY平面上のvec3d型に変換する
			vec3d Getvec3d_XZ(const double &y = 0.0) const; //XZ平面上のvec3d型に変換する
			vec2d GetPerpendicular(const e2LR::e LR) const; //垂直な方向のベクトルを計算して得る

			static double GetDistance(const vec2d &vec1, const vec2d &vec2) { return sqrt(pow(vec2.x - vec1.x, 2.0) + pow(vec2.y - vec1.y, 2.0)); } //2つのベクトル間の距離を返す
			static double GetInnerProduct(const vec2d &vec1, const vec2d &vec2) { return vec1.x * vec2.x + vec1.y * vec2.y; } //内積を計算する
			static double GetCrossProduct(const vec2d &vec1, const vec2d &vec2) { return vec1.x * vec2.y - vec2.x * vec1.y; } //外積を計算する
			static bool GetAngle(double &out, const vec2d &vec1, const vec2d &vec2); //2つのベクトルの成す角度を求める
			static vec2d GetCrossReverse(const vec2d &vec, const double &z) { return vec2d(-vec.y * z, vec.x * z); }
			static vec2d GetCross(const vec2d &vec, const double &z) { return vec2d(vec.y * z, -vec.x * z); }
			//static bool GetCrossPoint(const vec2d &vecLine1Begin, const vec2d &vecLine1End, const vec2d &vecLine2Begin, const vec2d &vecLine2End, ho::Enable<ho::vec2d> &vecCrossPos, bool &Inner); //2つの直線の交点を求める
		};

		class vec3d //double型の3次元ベクトル
		{
		public:
			//コンストラクタ
			vec3d() { this->x = this->y = this->z = 0.0; }
			vec3d(const double &x, const double &y, const double &z) { this->x = x; this->y = y; this->z = z; }

			double x, y, z; //メンバ変数

			//演算子
			vec3d operator +(const vec3d &obj) const { return vec3d(this->x + obj.x, this->y + obj.y, this->z + obj.z); }
			vec3d& operator +=(const vec3d &obj) { this->x += obj.x; this->y += obj.y; this->z += obj.z; return *this; }
			static void Add(vec3d &out, const vec3d &vec1, const vec3d &vec2) { out.x = vec1.x + vec2.x; out.y = vec1.y + vec2.y; out.z = vec1.z + vec2.z; }
			vec3d operator -(const vec3d &obj) const { return vec3d(this->x - obj.x, this->y - obj.y, this->z - obj.z); }
			vec3d& operator -=(const vec3d &obj) { this->x -= obj.x; this->y -= obj.y; this->z -= obj.z; return *this; }
			static void Sub(vec3d &out, const vec3d &vec1, const vec3d &vec2) { out.x = vec1.x - vec2.x; out.y = vec1.y - vec2.y; out.z = vec1.z - vec2.z; }
			vec3d operator *(const double &value) const { return vec3d(this->x * value, this->y * value, this->z * value); }
			vec3d& operator *=(const double &value) { this->x *= value; this->y *= value; this->z *= value; return *this; }
			static void Mul(vec3d &out, const vec3d &vec1, const vec3d &vec2) { out.x = vec1.x * vec2.x; out.y = vec1.y * vec2.y; out.z = vec1.z * vec2.z; }
			vec3d operator /(const double &value) const { return vec3d(this->x / value, this->y / value, this->z / value); }
			vec3d& operator /=(const double &value) { this->x /= value; this->y /= value; this->z /= value; return *this; }
			vec3d operator *(const vec3d &obj) const { return vec3d(this->x * obj.x, this->y * obj.y, this->z * obj.z); }
			vec3d operator /(const vec3d &obj) const { return vec3d(this->x / obj.x, this->y / obj.y, this->z / obj.z); }
			vec3d operator -() { return vec3d(-this->x, -this->y, -this->z); }
			vec3d operator *(const Matrix &mtrx) const;
			bool operator ==(const vec3d obj) const { return(this->x == obj.x && this->y == obj.y && this->z == obj.z); }
			bool operator !=(const vec3d obj) const { return(!(*this == obj)); }

#ifdef USE_CONVERT_DIRECTX_MATH
			//DirectXMath とのコンバータ
			vec3d(const XMFLOAT3 &obj) { this->x = obj.x; this->y = obj.y; this->z = obj.z; }
			operator XMFLOAT3() const { return XMFLOAT3(float(this->x), float(this->y), float(this->z)); }
#endif

			//正規化したベクトルを得る
			vec3d GetNormalize() const
			{
				double Denominator = sqrt(x * x + y * y + z * z); //分母を計算

				if (!Denominator) //分母が0の場合
					return vec3d(0, 0, 0);

				Denominator = 1.0 / Denominator; //逆数

				return vec3d(x * Denominator, y * Denominator, z * Denominator);
			}

			double GetPower() const { return sqrt(x * x + y * y + z * z); } //ベクトルの量を返す
			void clear() { this->x = this->y = this->z = 0.0; } //0に初期化
			vec2d Getvec2d_XY() const { return vec2d(this->x, this->y); } //XY平面上の座標でvec2dに変換する
			vec2d Getvec2d_XZ() const { return vec2d(this->x, this->z); } //XZ平面上の座標でvec2dに変換する

			static double GetDistance(const vec3d &vec1, const vec3d &vec2) { return sqrt(pow(vec2.x - vec1.x, 2.0) + pow(vec2.y - vec1.y, 2.0) + pow(vec2.z - vec1.z, 2.0)); } //ベクトル間の距離を返す
			static double GetDot(const vec3d &vec1, const vec3d &vec2) { return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z; } //内積を求める
			static vec3d GetCross(const vec3d &vec1, const vec3d &vec2) { return vec3d(vec1.y * vec2.z - vec1.z * vec2.y, vec1.z * vec2.x - vec1.x * vec2.z, vec1.x * vec2.y - vec1.y * vec2.x); } //外積を求める
			static bool GetAngle(double &out, const vec3d &vec1, const vec3d &vec2); //2つのベクトル間の成す角の角度を返す
			static bool GetAngleAndAxis(double &Angle, vec3d &vecAxis, const vec3d &vec1, const vec3d &vec2); //2つのベクトル間の成す角の角度と回転軸ベクトルを得る
			//static bool GetCrossPointsFromLines(std::vector<vec3d> &vecOut, vec3d pIn[4]); //2つのベクトルからなる直線同士の交点または一番接近する点を求める
		};
	}
}

/*
オーバーロードされた演算子の加算などは、コンストラクタが呼ばれるために遅くなる。
+ の代わりに += や Add() や Sub() 等の演算メソッドを使うと高速になるが、これらは
計算結果で上書きされる（+=）ことや、計算結果格納変数を作る必要がある（Addメソッドなど）ため、
効率良く使えるための配慮が必要である。
*/
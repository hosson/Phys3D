//クォータニオン
#pragma once
#include "vector.h"
//#include <d3dx9math.h>
#include "Matrix.h"

namespace ho
{
	namespace num
	{
		class vec3d;
		class Matrix;

		class Quaternion
		{
		public:
			Quaternion() {}
			Quaternion(double w, double x, double y, double z) { this->w = w; this->x = x; this->y = y; this->z = z; }
			Quaternion(vec3d vecAxis, const double &Radian); //コンストラクタ
			Quaternion(Matrix Matrix); //コンストラクタ（3X3回転行列から作成）
			~Quaternion(); //デストラクタ

			Matrix GetMatrix3x3() const; //3x3 の回転成分のみの座標変換行列に変換
			Matrix GetMatrix4x4() const; //4x4 の平行移動成分も含めた座標変換行列に変換
			void TransformVector(vec3d &vec) const; //座標を変換
			void TransformVector(vec3d &vecOut, const vec3d &vecIn) const; //座標を変換
			void CalcAxisAndAngle(vec3d &vecAxis, double &Angle); //回転軸と回転角度を計算する
			void Normalize(); //正規化する
			Quaternion GetConjugate() const { return Quaternion(w, -x, -y, -z); } //共役四元数となるクォータニオンを返す

			static bool Slerp(Quaternion &Out, const Quaternion &qt1, const Quaternion &qt2, double t); //2つのクォータニオンを球面線形補間する

			//演算子
			Quaternion operator +(const Quaternion &qt);
			Quaternion operator +=(const Quaternion &qt);
			Quaternion operator -(const Quaternion &qt);
			Quaternion operator -=(const Quaternion &qt);
			Quaternion operator *(const Quaternion &qt) const;
			Quaternion operator *=(const Quaternion &qt);
			friend Quaternion operator *(double s, const Quaternion &qt);
			Quaternion operator *(const double s);
			Quaternion operator *=(const double s);
			bool operator ==(const Quaternion &obj) const;
		private:
			double w, x, y, z;
		};
	}
}

/*クォータニオンを使うと、回転の状態を記述できる。
回転行列等に比べて少ない変数と少ない計算量で済み、
2つのクォータニオンの間を線形球面補間することもできる。

使い方としては、
Quaternion(double Radian, VECTOR3DOUBLE *pvecN)
の方のコンストラクタで回転軸ベクトルと角度を入れると、
任意軸に任意角度回転した状態を表すクォータニオンができる。
このときベクトルは正規化したものを入力しなければならない。

2つ回転を合成するときには積算（*演算子）を行う。
これは順番によって違う結果となる。
qt3 = qt1 * qt2;
と記述すると、qt1の回転を行ったあとにqt2の回転を行った状態の回転が
qt3の中に入る。

実際に座標を回転させるには、CalcD3DXMatrix等で座標変換行列を得て
座標を行列で変換させる。または、TransformVectorを使えば、
直接に変換することもできる。
*/
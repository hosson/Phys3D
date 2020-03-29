#include "Math.h"
#include <Windows.h>
#include "Vector.h"

namespace ho {
	//奇数かどうかを判定
	bool Odd(const int &val) { return (val % 2) != 0; }
	bool Odd(const unsigned int &val) { return (val % 2) != 0; }

	//偶数かどうかを判定
	bool Even(const int &val) { return (val % 2) == 0; }
	bool Even(const unsigned int &val) { return (val % 2) == 0; }

	//値を 0.0～2π までの位相に変換する
	float PhaseLimit(const float &Phase)
	{
		if (Phase > PI * 2.0)
			return fmod(Phase, float(PI * 2.0));
		if (Phase < 0.0)
			return float(PI * 2.0) + fmod(Phase, float(PI * 2.0));
		return Phase;
	}
	void PhaseLimit(float &Phase)
	{
		if (Phase > PI * 2.0)
			Phase = fmod(Phase, float(PI * 2.0));
		else if (Phase < 0.0)
			Phase = float(PI * 2.0) + fmod(Phase, float(PI * 2.0));

		return;
	}
	double PhaseLimit(const double &Phase)
	{
		if (Phase > PI * 2.0)
			return fmod(Phase, PI * 2.0);
		if (Phase < 0.0)
			return PI * 2.0 + fmod(Phase, PI * 2.0);
		return Phase;
	}
	void PhaseLimit(double &Phase)
	{
		if (Phase > PI * 2.0)
			Phase = fmod(Phase, PI * 2.0);
		else if (Phase < 0.0)
			Phase = PI * 2.0 + fmod(Phase, PI * 2.0);

		return;
	}


	//1次元配列を2次元配列と見なした場合に、XとYの座標から1次元配列上の要素数を得る
	inline int GetVectorPosFromXY(int Width, int X, int Y)
	{
		return Y * Width + X;
	}

	//1次元配列を2次元配列と見なした場合に、1次元配列上の要素数からXとYの座標を得る
	inline PT<int> GetXYFromVectorPos(int Width, int VectorPos)
	{
		return PT<int>(VectorPos % Width, VectorPos / Width);
	}






	const float SinTable::Ratio = 10430.378350470452724949566316381f; // 65536 / (2 * PI)
	float SinTable::Table[65536];

	BOOL SinTable::Init() //初期化
	{
		int i;

		//テーブルデータを計算
		for (i = 0; i < 65536; i++)
			Table[i] = sin(float(i) / Ratio);

		return TRUE;
	}

	BOOL SinTable::Dest() //終了処理
	{
		return TRUE;
	}










	//コンストラクタ（3点から一般平面方程式を求める）
	PLANE::PLANE(const VECTOR3DOUBLE &vec1, const VECTOR3DOUBLE &vec2, const VECTOR3DOUBLE &vec3)
	{
		VECTOR3DOUBLE v1 = vec2 - vec1, v2 = vec3 - vec1;
		VECTOR3DOUBLE vecN = VECTOR3DOUBLE::GetCrossProduct(v1, v2); //外積ベクトルを求める

		/*点P(x0, y0, z0)を通り，法線ベクトルが ↑N(a, b, c)の平面の方程式は、
		a(x-x0)+b(y-y0)+c(z-z0)=0となる。
		一般平面方程式 ax+by+cz+d=0 では、法線ベクトルが(a, b, c)となる。*/
	
		this->a = vecN.x;
		this->b = vecN.y;
		this->c = vecN.z;
		this->d = -(vec1.x * vecN.x + vec1.y * vecN.y + vec1.z * vecN.z);
	}

	//コンストラクタ（パラメータを直接指定）
	PLANE::PLANE(const double &a, const double &b, const double &c, const double &d) 
	{
		/*a, b, c は平面の法線ベクトルとなる。
		d は原点からの距離となる。*/

		this->a = a;
		this->b = b;
		this->c = c;
		this->d = d;
	}

	//コンストラクタ（法線ベクトルと原点からの距離）
	PLANE::PLANE(const D3DXVECTOR3 &vecNormal, const double &d) 
	{
		this->a = vecNormal.x;
		this->b = vecNormal.y;
		this->c = vecNormal.z;
		this->d = d;
	}

	//平面方程式から平面の法線ベクトルを得る
	VECTOR3DOUBLE PLANE::GetNormalLine() const
	{
		return VECTOR3DOUBLE(this->a, this->b, this->c);
	}

	//2つの頂点で示される直線との交点を求める
	bool PLANE::GetCrossPointFromLine(VECTOR3DOUBLE &out, const VECTOR3DOUBLE &vecPt1, const VECTOR3DOUBLE &vecPt2) const
	{
		/*
		平面Ｆを a x + b y + c z = d　・・・式①
		線分ABを、媒介変数tを用いて、
		P = A + t e　・・・式②
	　	 A = ( Ax, Ay, Az )
	　	 e = [ Bx-Ax By-Ay Bz-Az ]
	　　	  = [ Ex Ey Ez ]
		とします。
	
		式②を成分に分解します。
		x = Ax + t Ex
		y = Ay + t Ey
		z = Az + t Ez

		式①に代入します。
	　	 a x + b y + c z = d
		⇔a (Ax + t Ex) + b ( Ay + t Ey ) + c ( Az + t Ez ) = d
		⇔a Ax + t a Ex + b Ay + t b Ey + c Az + t c Ez = d
		⇔t ( a Ex + b Ey + c Ez ) = d - a Ax - b Ay - c Az
		⇔t = { d - ( a Ax + b Ay + c Az ) } / ( a Ex + b Ey + c Ez )

		DE = a Ex + b Ey + c Ez
		とします。

		DEがゼロなとき
	　	 交点はありません（平面Ａと線分ＡＢは平行です）
		DEがゼロでないとき
	　	 平面Ａと直線ＡＢとの交点は、
	　	 P = A + t e
	　　	  A = ( Ax, Ay, Az )
	　　	  e = [ Bx-Ax By-Ay Bz-Az ]
	　　	  t = { d - ( a Ax + b Ay + c Az ) } / ( a Ex + b Ey + c Ez )
	　	 e < 0 : 点Ｐは点Ａより手前
	　	 e > 1 : 点Ｐは点Ａより奥
		 */

		VECTOR3DOUBLE vecE = vecPt2 - vecPt1; //点1から点2までのベクトル

		if (a * vecE.x + b * vecE.y + c * vecE.z == 0.0) //平面と直線は平行なので交点は無い場合
			return false;

		//媒介変数tを求める
		double t = (-d - (a * vecPt1.x + b * vecPt1.y + c * vecPt1.z)) / (a * vecE.x + b * vecE.y + c * vecE.z);

		out = vecPt1 + vecE * t; //P=A+te（x=Ax+tEx, y=Ay+tEy, z=Az+tEz）を計算

		return true;
	}

	//X, Y, Z の中から係数の絶対値が最も大きい項がどれかを調べる
	PLANE::Param PLANE::GetMaxAbsParam() 
	{
		double a = abs(this->a), b = abs(this->b), c = abs(this->c); //絶対値に変換

		if (a >= b && a >= c)
			return X;
		if (b >= a && b >= c)
			return Y;
		if (c >= a && c >= b)
			return Z;

		return Param(-1); //エラー
	}

	//平面上で任意の軸上を任意の量動かした場合に、別のある任意の軸の動く量を得る
	double PLANE::GetOneAxisMoveVector(Param InVector, const double &In, Param OutVector) const
	{
		double abc[] = {a, b, c}; //項の係数を配列に格納

		if (abc[InVector] == 0.0 || abc[OutVector] == 0.0) //どちらかの任意軸が固定された平面だった場合
			return 0; //移動できない

		//a(x + 0) + b(y + 0) + c(z + 0) = 0 の平面式を結果を得たい軸を=の反対側に移行し、0 の部分の変化量を計算する
		return (abc[InVector] * -In) / abc[OutVector];
	}

	//2つの平面方程式から交線（ベクトルと通る1点の座標）を求める
	bool PLANE::GetIntersectionLine(VECTOR3DOUBLE &vecDirection, VECTOR3DOUBLE &vecPoint, const PLANE &pln1, const PLANE &pln2)
	{
		//参考URL:http://www.hiramine.com/programming/graphics/3d_planeintersection.html
		//注:D3DXPLANEではdが左辺にあるが、上記URLでは右辺にあるため、この関数内で使われるdには逐一-を付けてあわせてある

		/*交線の方向ベクトルを求める。
		交線の方向ベクトル は、2つの平面の法線ベクトルと垂直なベクトル。
		2つのベクトルと垂直なベクトルは、外積計算により求まる。*/
		vecDirection.x = pln1.b * pln2.c - pln1.c * pln2.b;
		vecDirection.y = pln1.c * pln2.a - pln1.a * pln2.c;
		vecDirection.z = pln1.a * pln2.b - pln1.b * pln2.a;

		/*交線が通る任意の点を求める。
		2つの平面方程式を連立させる。*/
		if (abs(vecDirection.z) >= abs(vecDirection.x) && abs(vecDirection.z) >= abs(vecDirection.y) && vecDirection.z != 0.0f) //方向ベクトルのZ軸成分の絶対値が一番大きく0以外の場合
		{
			vecPoint.x = (-pln1.d * pln2.b - -pln2.d * pln1.b) / vecDirection.z;
			vecPoint.y = (-pln1.d * pln2.a - -pln2.d * pln1.a) / -vecDirection.z;
			vecPoint.z = 0;
		} else if (abs(vecDirection.y) >= abs(vecDirection.x) && abs(vecDirection.y) >= abs(vecDirection.z) && vecDirection.y != 0.0f) //方向ベクトルのY軸成分の絶対値が一番大きく0以外の場合
		{
			vecPoint.x = (-pln1.d * pln2.c - -pln2.d * pln1.c) / -vecDirection.y;
			vecPoint.z = (-pln1.d * pln2.a - -pln2.d * pln1.a) / vecDirection.y;
			vecPoint.y = 0;
		} else if (abs(vecDirection.x) >= abs(vecDirection.y) && abs(vecDirection.x) >= abs(vecDirection.z) && vecDirection.x != 0.0f) //方向ベクトルのX軸成分の絶対値が一番大きく0以外の場合
		{
			vecPoint.y = (-pln1.d * pln2.c - -pln2.d * pln1.c) / vecDirection.x;
			vecPoint.z = (-pln1.d * pln2.b - -pln2.d * pln1.b) / -vecDirection.x;
			vecPoint.x = 0;
		} else { //方向ベクトルの全成分が0の場合は2つの平面は平行で交線がない
			return false;
		}

		vecDirection = vecDirection.GetNormalize(); //方向ベクトルを正規化する

		return true;
	}
}

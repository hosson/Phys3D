//行列クラス
#pragma once
#include <vector>
#include <stdarg.h>
#include "Math.h"
#include "vector.h"

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
		class vec3d;
		class Quaternion;

		class Matrix
		{
		public:
			Matrix() {}
			Matrix(int xSize, int ySize); //コンストラクタ
			Matrix(const ho::num::vec3d &vecAxis, const double &Angle); //コンストラクタ（左手座標系の3X3回転行列を作成）
			~Matrix(); //デストラクタ

			void InitZero(); //全ての要素を0で初期化する
			void Resize(int xSize, int ySize); //サイズを変更する
			void BulkAssignment(double value, ...); //一括代入
			Matrix CalcTransposedMatrix(); //転置行列を計算する
			Matrix CalcInverseMatrix(); //逆行列を計算する
			Matrix CalcCofactorMatrix(); //余因子行列を計算する
			double CalcDeterminant(); //行列式の値を計算する

			//アクセッサ
			int GetxSize() { return xSize; }
			int GetySize() { return ySize; }
			const double &get(const int x, const int y) const { return vectorMatrix.at(y * xSize + x); }

			//演算子
			Matrix operator +(const Matrix &Mtrx) const;
			Matrix operator *(const Matrix &Mtrx) const;
			vec3d operator *(const vec3d &vec) const;
			Matrix operator *(const double d) const;
			double& operator ()(const int x, const int y); //値の取り出し、代入

#ifdef USE_CONVERT_DIRECTX_MATH
			//DirectXMath とのコンバータ
			Matrix(const XMMATRIX &obj)
			{
				this->Resize(4, 4);
				for (int i = 0; i < 4; i++)
					for (int j = 0; j < 4; j++)
						this->vectorMatrix.at(i * 4 + j) = obj.r[i].m128_f32[j];
			}
			operator XMMATRIX() const
			{
				return XMMATRIX(
					(float)this->vectorMatrix.at(0), (float)this->vectorMatrix.at(1), (float)this->vectorMatrix.at(2), (float)this->vectorMatrix.at(3),
					(float)this->vectorMatrix.at(4), (float)this->vectorMatrix.at(5), (float)this->vectorMatrix.at(6), (float)this->vectorMatrix.at(7),
					(float)this->vectorMatrix.at(8), (float)this->vectorMatrix.at(9), (float)this->vectorMatrix.at(10), (float)this->vectorMatrix.at(11),
					(float)this->vectorMatrix.at(12), (float)this->vectorMatrix.at(13), (float)this->vectorMatrix.at(14), (float)this->vectorMatrix.at(15));
			}
#endif
		private:
			int xSize; //行列のX方向サイズ
			int ySize; //行列のY方向サイズ
			std::vector<double> vectorMatrix; //行列のデータ
		};

		/*
		//独自に継承して機能を追加した構造体
		struct D3DXMATRIX : public ::D3DXMATRIX
		{
		public:
			D3DXMATRIX(D3DXVECTOR3 &vecAxis, float Angle); //コンストラクタ（任意軸の回転行列を作成）
		};
		*/
	}
}

#include "Matrix.h"
#include <Windows.h>
#include "Math.h"

namespace ho
{
	namespace num
	{
		//コンストラクタ
		Matrix::Matrix(int xSize, int ySize)
		{
			this->xSize = xSize;
			this->ySize = ySize;
			vectorMatrix.resize(xSize * ySize);
		}

		//コンストラクタ（左手座標系の3X3回転行列を作成）
		Matrix::Matrix(const vec3d &vecAxis, const double &Angle)
		{
			Resize(3, 3);

			double Cos = cos(Angle), Sin = sin(Angle);
			BulkAssignment(vecAxis.x * vecAxis.x * (1.0 - Cos) + Cos, vecAxis.x * vecAxis.y * (1.0 - Cos) - vecAxis.z * Sin, vecAxis.z * vecAxis.x * (1.0 - Cos) + vecAxis.y * Sin,
				vecAxis.x * vecAxis.y * (1.0 - Cos) + vecAxis.z * Sin, vecAxis.y * vecAxis.y * (1.0 - Cos) + Cos, vecAxis.y * vecAxis.z * (1.0f - Cos) - vecAxis.x * Sin,
				vecAxis.z * vecAxis.x * (1.0 - Cos) - vecAxis.y * Sin, vecAxis.y * vecAxis.z * (1.0 - Cos) + vecAxis.x * Sin, vecAxis.z * vecAxis.z * (1.0 - Cos) + Cos
				);
		}

		//デストラクタ
		Matrix::~Matrix()
		{
		}

		//全ての要素を0で初期化する
		void Matrix::InitZero()
		{
			ZeroMemory(&vectorMatrix.at(0), sizeof(double) * vectorMatrix.size());

			return;
		}

		//サイズを変更する
		void Matrix::Resize(int xSize, int ySize)
		{
			this->xSize = xSize;
			this->ySize = ySize;
			vectorMatrix.resize(xSize * ySize);
		}

		//一括代入
		void Matrix::BulkAssignment(double value, ...)
		{
			va_list list;
			va_start(list, value);

			vectorMatrix.at(0) = value;
			for (DWORD i = 1; i < vectorMatrix.size(); i++)
				vectorMatrix.at(i) = va_arg(list, double);

			va_end(list);

			return;
		}

		//転置行列を計算する
		Matrix Matrix::CalcTransposedMatrix()
		{
			Matrix mtrxNew(this->ySize, this->xSize);
			for (int i = 0; i < this->ySize; i++)
				for (int j = 0; j < this->xSize; j++)
					mtrxNew(i, j) = (*this)(j, i);

			return mtrxNew;
		}

		//逆行列を計算する
		Matrix Matrix::CalcInverseMatrix()
		{
			if (this->xSize != this->ySize) //正方行列ではない場合
				return Matrix(0, 0);
			double Determinant = CalcDeterminant(); //行列式の値を計算する
			if (Determinant == 0.0) //行列式が0の場合
				return Matrix(0, 0); //逆行列は存在しない

			Matrix mtrxCofactor = CalcCofactorMatrix(); //余因子行列を計算

			return mtrxCofactor * (1.0 / Determinant); //余因子行列を行列式の値で割る
		}

		//余因子行列を計算する
		Matrix Matrix::CalcCofactorMatrix()
		{
			Matrix mtrxNew(this->xSize, this->ySize);

			int Sign1 = 1, Sign2;
			for (int i = 0; i < this->ySize; i++)
			{
				Sign2 = Sign1;
				for (int j = 0; j < this->xSize; j++)
				{
					int X = 0, Y = 0; //現在の行列の中の参照位置
					Matrix mtrxCofactor(this->xSize - 1, this->ySize - 1); //余因子の値となる行列式を求めるための行列
					for (int k = 0; k < mtrxCofactor.ySize; k++)
					{
						if (Y == j)
							Y++;
						X = 0;
						for (int l = 0; l < mtrxCofactor.xSize; l++)
						{
							if (X == i)
								X++;
							mtrxCofactor(l, k) = (*this)(X, Y);
							X++;
						}
						Y++;
					}
					mtrxNew(j, i) = mtrxCofactor.CalcDeterminant() * Sign2; //行列から余因子の値を計算

					Sign2 *= -1;
				}
				Sign1 *= -1;
			}

			return mtrxNew;
		}


		//行列式の値を計算する
		double Matrix::CalcDeterminant()
		{
			if (this->xSize != this->ySize) //正方行列ではない場合
				return 0;
			if (this->xSize == 1) //1X1の場合
				return (*this)(0, 0);
			if (this->xSize == 2) //2X2の場合
				return (*this)(0, 0) * (*this)(1, 1) - (*this)(1, 0) * (*this)(0, 1);

			//行列式を分解
			double Result = 0;
			int Sign = 1; //符号
			for (int i = 0; i < this->xSize; i++)
			{
				Matrix mtrxNew(this->xSize - 1, this->ySize - 1); //一回り小さい行列を作成
				int Y = 0; //元の行列の行
				for (int j = 0; j < mtrxNew.ySize; j++)
				{
					for (int k = 0; k < mtrxNew.xSize; k++)
					{
						if (Y == i)
							Y++;
						mtrxNew(k, j) = (*this)(1 + k, Y);
					}
					Y++;
				}
				Result += Sign * (*this)(0, i) * mtrxNew.CalcDeterminant(); //再帰的に行列式を計算

				Sign *= -1; //符号を反転
			}

			return Result;
		}

		Matrix Matrix::operator +(const Matrix &Mtrx) const
		{
			if (this->xSize != Mtrx.ySize) //サイズが合わない場合
				return Matrix(0, 0);

			Matrix mtrxNew(this->xSize, this->ySize);
			for (int i = 0; i < this->ySize; i++)
				for (int j = 0; j < this->xSize; j++)
					mtrxNew(j, i) = (*this).get(j, i) + Mtrx.get(j, i);

			return mtrxNew;
		}

		Matrix Matrix::operator *(const Matrix &Mtrx) const
		{
			if (this->xSize != Mtrx.ySize) //サイズが合わない場合
				return Matrix(0, 0);

			Matrix mtrxNew(Mtrx.xSize, this->ySize);
			double s;
			for (int i = 0; i < this->ySize; i++)
			{
				for (int j = 0; j < Mtrx.xSize; j++)
				{
					s = 0;
					for (int k = 0; k < this->xSize; k++)
					{
						s += (*this).get(k, i) * Mtrx.get(j, k);
					}
					mtrxNew(j, i) = s;
				}
			}

			return mtrxNew;
		}


		vec3d Matrix::operator *(const vec3d &vec) const
		{
			//ベクトルを1X3の行列に変換
			Matrix mtrxVec(1, 3);
			mtrxVec(0, 0) = vec.x;
			mtrxVec(0, 1) = vec.y;
			mtrxVec(0, 2) = vec.z;

			Matrix mtrxNew = (*this) * mtrxVec;

			return vec3d(mtrxNew(0, 0), mtrxNew(0, 1), mtrxNew(0, 2));
		}

		Matrix Matrix::operator *(const double d) const
		{
			Matrix mtrxNew(this->xSize, this->ySize);
			for (int i = 0; i < this->ySize; i++)
				for (int j = 0; j < this->xSize; j++)
					mtrxNew(j, i) = (*this).get(j, i) * d;

			return mtrxNew;
		}

		//値の取り出し、代入
		double& Matrix::operator ()(const int x, const int y)
		{
			return vectorMatrix.at(y * xSize + x);
		}




		/*

		D3DXMATRIX::D3DXMATRIX(D3DXVECTOR3 &vecAxis, float Angle) //コンストラクタ（任意軸の回転行列を作成）
		{
		//注：vecAxis は正規化しておく必要がある

		float Cos = cos(Angle), Sin = sin(Angle);

		this->_11 = vecAxis.x * vecAxis.x * (1.0f - Cos) + Cos;
		this->_12 = vecAxis.x * vecAxis.y * (1.0f - Cos) - vecAxis.z * Sin;
		this->_13 = vecAxis.z * vecAxis.x * (1.0f - Cos) + vecAxis.y * Sin;
		this->_14 = 0;

		this->_21 = vecAxis.x * vecAxis.y * (1.0f - Cos) + vecAxis.z * Sin;
		this->_22 = vecAxis.y * vecAxis.y * (1.0f - Cos) + Cos;
		this->_23 = vecAxis.y * vecAxis.z * (1.0f - Cos) - vecAxis.x * Sin;
		this->_24 = 0;

		this->_31 = vecAxis.z * vecAxis.x * (1.0f - Cos) - vecAxis.y * Sin;
		this->_32 = vecAxis.y * vecAxis.z * (1.0f - Cos) + vecAxis.x * Sin;
		this->_33 = vecAxis.z * vecAxis.z * (1.0f - Cos) + Cos;
		this->_34 = 0;

		this->_41 = this->_42 = this->_43 = 0;
		this->_44 = 1.0;
		}
		*/
	}
}

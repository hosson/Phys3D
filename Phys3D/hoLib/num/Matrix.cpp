#include "Matrix.h"
#include <Windows.h>
#include "Math.h"

namespace ho
{
	namespace num
	{
		//�R���X�g���N�^
		Matrix::Matrix(int xSize, int ySize)
		{
			this->xSize = xSize;
			this->ySize = ySize;
			vectorMatrix.resize(xSize * ySize);
		}

		//�R���X�g���N�^�i������W�n��3X3��]�s����쐬�j
		Matrix::Matrix(const vec3d &vecAxis, const double &Angle)
		{
			Resize(3, 3);

			double Cos = cos(Angle), Sin = sin(Angle);
			BulkAssignment(vecAxis.x * vecAxis.x * (1.0 - Cos) + Cos, vecAxis.x * vecAxis.y * (1.0 - Cos) - vecAxis.z * Sin, vecAxis.z * vecAxis.x * (1.0 - Cos) + vecAxis.y * Sin,
				vecAxis.x * vecAxis.y * (1.0 - Cos) + vecAxis.z * Sin, vecAxis.y * vecAxis.y * (1.0 - Cos) + Cos, vecAxis.y * vecAxis.z * (1.0f - Cos) - vecAxis.x * Sin,
				vecAxis.z * vecAxis.x * (1.0 - Cos) - vecAxis.y * Sin, vecAxis.y * vecAxis.z * (1.0 - Cos) + vecAxis.x * Sin, vecAxis.z * vecAxis.z * (1.0 - Cos) + Cos
				);
		}

		//�f�X�g���N�^
		Matrix::~Matrix()
		{
		}

		//�S�Ă̗v�f��0�ŏ���������
		void Matrix::InitZero()
		{
			ZeroMemory(&vectorMatrix.at(0), sizeof(double) * vectorMatrix.size());

			return;
		}

		//�T�C�Y��ύX����
		void Matrix::Resize(int xSize, int ySize)
		{
			this->xSize = xSize;
			this->ySize = ySize;
			vectorMatrix.resize(xSize * ySize);
		}

		//�ꊇ���
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

		//�]�u�s����v�Z����
		Matrix Matrix::CalcTransposedMatrix()
		{
			Matrix mtrxNew(this->ySize, this->xSize);
			for (int i = 0; i < this->ySize; i++)
				for (int j = 0; j < this->xSize; j++)
					mtrxNew(i, j) = (*this)(j, i);

			return mtrxNew;
		}

		//�t�s����v�Z����
		Matrix Matrix::CalcInverseMatrix()
		{
			if (this->xSize != this->ySize) //�����s��ł͂Ȃ��ꍇ
				return Matrix(0, 0);
			double Determinant = CalcDeterminant(); //�s�񎮂̒l���v�Z����
			if (Determinant == 0.0) //�s�񎮂�0�̏ꍇ
				return Matrix(0, 0); //�t�s��͑��݂��Ȃ�

			Matrix mtrxCofactor = CalcCofactorMatrix(); //�]���q�s����v�Z

			return mtrxCofactor * (1.0 / Determinant); //�]���q�s����s�񎮂̒l�Ŋ���
		}

		//�]���q�s����v�Z����
		Matrix Matrix::CalcCofactorMatrix()
		{
			Matrix mtrxNew(this->xSize, this->ySize);

			int Sign1 = 1, Sign2;
			for (int i = 0; i < this->ySize; i++)
			{
				Sign2 = Sign1;
				for (int j = 0; j < this->xSize; j++)
				{
					int X = 0, Y = 0; //���݂̍s��̒��̎Q�ƈʒu
					Matrix mtrxCofactor(this->xSize - 1, this->ySize - 1); //�]���q�̒l�ƂȂ�s�񎮂����߂邽�߂̍s��
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
					mtrxNew(j, i) = mtrxCofactor.CalcDeterminant() * Sign2; //�s�񂩂�]���q�̒l���v�Z

					Sign2 *= -1;
				}
				Sign1 *= -1;
			}

			return mtrxNew;
		}


		//�s�񎮂̒l���v�Z����
		double Matrix::CalcDeterminant()
		{
			if (this->xSize != this->ySize) //�����s��ł͂Ȃ��ꍇ
				return 0;
			if (this->xSize == 1) //1X1�̏ꍇ
				return (*this)(0, 0);
			if (this->xSize == 2) //2X2�̏ꍇ
				return (*this)(0, 0) * (*this)(1, 1) - (*this)(1, 0) * (*this)(0, 1);

			//�s�񎮂𕪉�
			double Result = 0;
			int Sign = 1; //����
			for (int i = 0; i < this->xSize; i++)
			{
				Matrix mtrxNew(this->xSize - 1, this->ySize - 1); //���菬�����s����쐬
				int Y = 0; //���̍s��̍s
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
				Result += Sign * (*this)(0, i) * mtrxNew.CalcDeterminant(); //�ċA�I�ɍs�񎮂��v�Z

				Sign *= -1; //�����𔽓]
			}

			return Result;
		}

		Matrix Matrix::operator +(const Matrix &Mtrx) const
		{
			if (this->xSize != Mtrx.ySize) //�T�C�Y������Ȃ��ꍇ
				return Matrix(0, 0);

			Matrix mtrxNew(this->xSize, this->ySize);
			for (int i = 0; i < this->ySize; i++)
				for (int j = 0; j < this->xSize; j++)
					mtrxNew(j, i) = (*this).get(j, i) + Mtrx.get(j, i);

			return mtrxNew;
		}

		Matrix Matrix::operator *(const Matrix &Mtrx) const
		{
			if (this->xSize != Mtrx.ySize) //�T�C�Y������Ȃ��ꍇ
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
			//�x�N�g����1X3�̍s��ɕϊ�
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

		//�l�̎��o���A���
		double& Matrix::operator ()(const int x, const int y)
		{
			return vectorMatrix.at(y * xSize + x);
		}




		/*

		D3DXMATRIX::D3DXMATRIX(D3DXVECTOR3 &vecAxis, float Angle) //�R���X�g���N�^�i�C�ӎ��̉�]�s����쐬�j
		{
		//���FvecAxis �͐��K�����Ă����K�v������

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

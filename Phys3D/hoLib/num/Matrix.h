//�s��N���X
#pragma once
#include <vector>
#include <stdarg.h>
#include "Math.h"
#include "vector.h"

#define USE_CONVERT_DIRECTX_MATH 
/*DirectXMath �ւ̃R���o�[�^���܂ރI�v�V�����B
DirectXMath ���g�p���Ȃ����̏ꍇ�͂��̒�`���R�����g�A�E�g����B*/


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
			Matrix(int xSize, int ySize); //�R���X�g���N�^
			Matrix(const ho::num::vec3d &vecAxis, const double &Angle); //�R���X�g���N�^�i������W�n��3X3��]�s����쐬�j
			~Matrix(); //�f�X�g���N�^

			void InitZero(); //�S�Ă̗v�f��0�ŏ���������
			void Resize(int xSize, int ySize); //�T�C�Y��ύX����
			void BulkAssignment(double value, ...); //�ꊇ���
			Matrix CalcTransposedMatrix(); //�]�u�s����v�Z����
			Matrix CalcInverseMatrix(); //�t�s����v�Z����
			Matrix CalcCofactorMatrix(); //�]���q�s����v�Z����
			double CalcDeterminant(); //�s�񎮂̒l���v�Z����

			//�A�N�Z�b�T
			int GetxSize() { return xSize; }
			int GetySize() { return ySize; }
			const double &get(const int x, const int y) const { return vectorMatrix.at(y * xSize + x); }

			//���Z�q
			Matrix operator +(const Matrix &Mtrx) const;
			Matrix operator *(const Matrix &Mtrx) const;
			vec3d operator *(const vec3d &vec) const;
			Matrix operator *(const double d) const;
			double& operator ()(const int x, const int y); //�l�̎��o���A���

#ifdef USE_CONVERT_DIRECTX_MATH
			//DirectXMath �Ƃ̃R���o�[�^
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
			int xSize; //�s���X�����T�C�Y
			int ySize; //�s���Y�����T�C�Y
			std::vector<double> vectorMatrix; //�s��̃f�[�^
		};

		/*
		//�Ǝ��Ɍp�����ċ@�\��ǉ������\����
		struct D3DXMATRIX : public ::D3DXMATRIX
		{
		public:
			D3DXMATRIX(D3DXVECTOR3 &vecAxis, float Angle); //�R���X�g���N�^�i�C�ӎ��̉�]�s����쐬�j
		};
		*/
	}
}

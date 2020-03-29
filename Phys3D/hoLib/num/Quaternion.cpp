#include "Quaternion.h"
#include "../Error.h"

namespace ho
{
	namespace num
	{
		//�R���X�g���N�^
		Quaternion::Quaternion(vec3d vecAxis, const double &Radian)
		{
			vecAxis = vecAxis.GetNormalize();
			double Radian2 = Radian * 0.5;
			double SinRadian2 = sin(Radian2);
			this->w = cos(Radian2);
			this->x = vecAxis.x * SinRadian2;
			this->y = vecAxis.y * SinRadian2;
			this->z = vecAxis.z * SinRadian2;
		}

		//�R���X�g���N�^�i3X3��]�s�񂩂�쐬�j
		Quaternion::Quaternion(Matrix Matrix)
		{
			//�ő听��������
			double elem[4]; //0:x, 1:y, 2:z, 3:w
			elem[0] = Matrix(0, 0) - Matrix(1, 1) - Matrix(2, 2) + 1.0;
			elem[1] = -Matrix(0, 0) + Matrix(1, 1) - Matrix(2, 2) + 1.0;
			elem[2] = -Matrix(0, 0) - Matrix(1, 1) + Matrix(2, 2) + 1.0;
			elem[3] = Matrix(0, 0) + Matrix(1, 1) + Matrix(2, 2) + 1.0;

			unsigned biggestIndex = 0;
			for (int i = 1; i < 4; i++)
				if (elem[i] > elem[biggestIndex])
					biggestIndex = i;

			if (elem[biggestIndex] < 0.0)
				ERR(true, TEXT("�����̍s�񂪐���������܂���B"), __WFILE__, __LINE__);

			//�ő�v�f�̒l���Z�o
			double *q[4] = { &this->x, &this->y, &this->z, &this->w };
			double v = sqrt(elem[biggestIndex]) * 0.5;
			*q[biggestIndex] = v;
			double mult = 0.25 / v;

			switch (biggestIndex)
			{
			case 0: // x
				*q[1] = (Matrix(0, 1) + Matrix(1, 0)) * mult;
				*q[2] = (Matrix(2, 0) + Matrix(0, 2)) * mult;
				*q[3] = (Matrix(1, 2) - Matrix(2, 1)) * mult;
				break;
			case 1: // y
				*q[0] = (Matrix(0, 1) + Matrix(1, 0)) * mult;
				*q[2] = (Matrix(1, 2) + Matrix(2, 1)) * mult;
				*q[3] = (Matrix(2, 0) - Matrix(0, 2)) * mult;
				break;
			case 2: // z
				*q[0] = (Matrix(2, 0) + Matrix(0, 2)) * mult;
				*q[1] = (Matrix(1, 2) + Matrix(2, 1)) * mult;
				*q[3] = (Matrix(0, 1) - Matrix(1, 0)) * mult;
				break;
			case 3: // w
				*q[0] = (Matrix(1, 2) - Matrix(2, 1)) * mult;
				*q[1] = (Matrix(2, 0) - Matrix(0, 2)) * mult;
				*q[2] = (Matrix(0, 1) - Matrix(1, 0)) * mult;
				break;
			}
		}



		//�f�X�g���N�^
		Quaternion::~Quaternion()
		{
		}

		//3x3 �̉�]�����݂̂̍��W�ϊ��s��ɕϊ�
		Matrix Quaternion::GetMatrix3x3() const
		{
			Matrix mtrx(3, 3);

			mtrx(0, 0) = 1.0 - 2.0 * y * y - 2.0 * z * z;
			mtrx(1, 0) = 2.0 * x * y + 2.0 * w * z;
			mtrx(2, 0) = 2.0 * x * z - 2.0 * w * y;
			mtrx(0, 1) = 2.0 * x * y - 2.0 * w * z;
			mtrx(1, 1) = 1.0 - 2.0 * x * x - 2.0 * z * z;
			mtrx(2, 1) = 2.0 * y * z + 2.0 * w * x;
			mtrx(0, 2) = 2.0 * x * z + 2.0 * w * y;
			mtrx(1, 2) = 2.0 * y * z - 2.0 * w * x;
			mtrx(2, 2) = 1.0 - 2.0 * x * x - 2.0 * y * y;
			return mtrx;
		}

		//4x4 �̕��s�ړ��������܂߂����W�ϊ��s��ɕϊ�
		Matrix Quaternion::GetMatrix4x4() const
		{
			Matrix mtrx(4, 4);

			mtrx(0, 0) = 1.0 - 2.0 * y * y - 2.0 * z * z;
			mtrx(1, 0) = 2.0 * x * y + 2.0 * w * z;
			mtrx(2, 0) = 2.0 * x * z - 2.0 * w * y;
			mtrx(3, 0) = 0;
			mtrx(0, 1) = 2.0 * x * y - 2.0 * w * z;
			mtrx(1, 1) = 1.0 - 2.0 * x * x - 2.0 * z * z;
			mtrx(2, 1) = 2.0 * y * z + 2.0 * w * x;
			mtrx(3, 1) = 0;
			mtrx(0, 2) = 2.0 * x * z + 2.0 * w * y;
			mtrx(1, 2) = 2.0 * y * z - 2.0 * w * x;
			mtrx(2, 2) = 1.0 - 2.0 * x * x - 2.0 * y * y;
			mtrx(3, 2) = 0;
			mtrx(0, 3) = 0;
			mtrx(1, 3) = 0;
			mtrx(2, 3) = 0;
			mtrx(3, 3) = 1.0;

			return mtrx;
		}

		//���W��ϊ�
		void Quaternion::TransformVector(vec3d &vec) const
		{
			Quaternion qtConjugate(w, -x, -y, -z); //�����l�����ƂȂ�N�H�[�^�j�I�����쐬
			Quaternion qtVec(0, vec.x, vec.y, vec.z); //�x�N�g�����N�H�[�^�j�I���ŕ\��

			Quaternion qtResult = *this * qtVec * qtConjugate; //�x�N�g������]

			vec.x = qtResult.x;
			vec.y = qtResult.y;
			vec.z = qtResult.z;

			return;
		}

		//���W��ϊ�
		void Quaternion::TransformVector(vec3d &vecOut, const vec3d &vecIn) const
		{
			Quaternion qtConjugate(w, -x, -y, -z); //�����l�����ƂȂ�N�H�[�^�j�I�����쐬
			Quaternion qtVec(0, vecIn.x, vecIn.y, vecIn.z); //�x�N�g�����N�H�[�^�j�I���ŕ\��

			Quaternion qtResult = *this * qtVec * qtConjugate; //�x�N�g������]

			vecOut.x = qtResult.x;
			vecOut.y = qtResult.y;
			vecOut.z = qtResult.z;

			return;
		}

		//��]���Ɖ�]�p�x���v�Z����
		void Quaternion::CalcAxisAndAngle(vec3d &vecAxis, double &Angle)
		{
			Angle = acos(w) * 2.0;
			double temp = sin(Angle * 0.5);
			vecAxis.x = x / temp;
			vecAxis.y = y / temp;
			vecAxis.z = z / temp;

			return;
		}

		//���K������
		void Quaternion::Normalize()
		{
			double Len = sqrt(w * w + x * x + y * y + z * z);
			w /= Len;
			x /= Len;
			y /= Len;
			z /= Len;

			return;
		}

		//2�̃N�H�[�^�j�I�������ʐ��`��Ԃ���
		bool Quaternion::Slerp(Quaternion &Out, const Quaternion &qt1, const Quaternion &qt2, double t)
		{
			if (qt1 == qt2) //�S������̃N�H�[�^�j�I�����n���ꂽ�ꍇ
			{
				Out = qt1; //�R�s�[����̂�
				return true;
			}

			const double len1 = sqrt(qt1.w * qt1.w + qt1.x * qt1.x + qt1.y * qt1.y + qt1.z * qt1.z);
			const double len2 = sqrt(qt2.w * qt2.w + qt2.x * qt2.x + qt2.y * qt2.y + qt2.z * qt2.z);

			if (len1 == 0.0 || len2 == 0.0)
			{
				ERR(true, TEXT("�s���ȃN�H�[�^�j�I���ŋ��ʐ��`��Ԃ��悤�Ƃ��܂����B"), __WFILE__, __LINE__);
				return false; //�s���ȃN�H�[�^�j�I��
			}

			const double cos_val = (qt1.w * qt2.w + qt1.x * qt2.x + qt1.y * qt2.y + qt1.z * qt2.z) / (len1 * len2);
			const double w = acos(cos_val);

			//���ʐ��`���
			const double sin_w = sin(w);
			const double sin_t_w = sin(t * w);
			const double sin_inv_t_w = sin((1.0 - t) * w);
			const double mult_q1 = sin_inv_t_w / sin_w;
			const double mult_q2 = sin_t_w / sin_w;

			Out.w = mult_q1 * qt1.w + mult_q2 * qt2.w;
			Out.x = mult_q1 * qt1.x + mult_q2 * qt2.x;
			Out.y = mult_q1 * qt1.y + mult_q2 * qt2.y;
			Out.z = mult_q1 * qt1.z + mult_q2 * qt2.z;

			return true;
		}

		Quaternion Quaternion::operator +(const Quaternion &qt)
		{
			return Quaternion(this->w + qt.w, this->x + qt.x, this->y + qt.y, this->z + qt.z);
		}

		Quaternion Quaternion::operator +=(const Quaternion &qt)
		{
			this->w += qt.w;
			this->x += qt.x;
			this->y += qt.y;
			this->z += qt.z;

			return *this;
		}

		Quaternion Quaternion::operator -(const Quaternion &qt)
		{
			return Quaternion(this->w - qt.w, this->x - qt.x, this->y - qt.y, this->z - qt.z);
		}

		Quaternion Quaternion::operator -=(const Quaternion &qt)
		{
			this->w -= qt.w;
			this->x -= qt.x;
			this->y -= qt.y;
			this->z -= qt.z;

			return *this;
		}

		Quaternion Quaternion::operator *(const Quaternion &qt) const
		{
			return Quaternion(this->w * qt.w - this->x * qt.x - this->y * qt.y - this->z * qt.z,
				this->w * qt.x + this->x * qt.w + this->y * qt.z - this->z * qt.y,
				this->w * qt.y + this->y * qt.w + this->z * qt.x - this->x * qt.z,
				this->w * qt.z + this->z * qt.w + this->x * qt.y - this->y * qt.x);
		}

		Quaternion Quaternion::operator *=(const Quaternion &qt)
		{
			double newW = this->w * qt.w - this->x * qt.x - this->y * qt.y - this->z * qt.z;
			double newX = this->w * qt.x + this->x * qt.w + this->y * qt.z - this->z * qt.y;
			double newY = this->w * qt.y + this->y * qt.w + this->z * qt.x - this->x * qt.z;
			double newZ = this->w * qt.z + this->z * qt.w + this->x * qt.y - this->y * qt.x;

			this->w = newW;
			this->x = newX;
			this->y = newY;
			this->z = newZ;

			return *this;
		}

		Quaternion operator *(double s, const Quaternion &qt)
		{
			return Quaternion(s * qt.w, s * qt.x, s * qt.y, s * qt.z);
		}


		Quaternion Quaternion::operator *(const double s)
		{
			return Quaternion(this->w * s, this->x * s, this->y * s, this->z * s);
		}

		Quaternion Quaternion::operator *=(const double s)
		{
			this->w *= s;
			this->x *= s;
			this->y *= s;
			this->z *= s;

			return *this;
		}

		bool Quaternion::operator ==(const Quaternion &obj) const
		{
			if (this->w != obj.w)
				return false;
			if (this->x != obj.x)
				return false;
			if (this->y != obj.y)
				return false;
			if (this->z != obj.z)
				return false;

			return true;
		}
	}
}
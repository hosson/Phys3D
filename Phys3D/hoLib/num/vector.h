//�x�N�g���֘A�̃N���X
#pragma once
#include <math.h>
#include "../Direction.h"
#include "Matrix.h"

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
		//�O���錾
		class vec3d;
		class Matrix;

		//double�^��2�����x�N�g��
		class vec2d
		{
		public:
			//�R���X�g���N�^
			vec2d() { this->x = this->y = 0.0; }
			vec2d(const double &x, const double &y) { this->x = x; this->y = y; }

			double x, y; //�����o�ϐ�

			//���Z�q
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
			//DirectXMath �Ƃ̃R���o�[�^
			vec2d(const XMFLOAT2 &obj) { this->x = obj.x; this->y = obj.y; }
			operator XMFLOAT2() const { return XMFLOAT2(float(this->x), float(this->y)); }
#endif

			//���K�������x�N�g���𓾂�
			vec2d GetNormalize() const
			{
				double Denominator = sqrt(x * x + y * y); //������v�Z

				if (!Denominator) //���ꂪ0�̏ꍇ
					return vec2d(0, 0);

				Denominator = 1.0 / Denominator; //�t�����v�Z���Ă����i��̏��Z��1��ɂ��邽�߁j

				return vec2d(x * Denominator, y * Denominator);
			}

			double GetPower() const { return sqrt(x * x + y * y); } //�x�N�g���̗ʂ��v�Z����
			void clear() { this->x = this->y = 0.0; } //0�ɏ�����

			//��]������iPC���W�n�ł͎��v���A���w���W�n�ł͔����v���j
			vec2d GetRotation(const double &Angle) const
			{
				return vec2d(x * cos(Angle) - y * sin(Angle), x * sin(Angle) + y * cos(Angle));
			}

			//�w�����̌����ɂ��ʐς����߂�
			static double GetArea(const vec2d &v1, const vec2d &v2, const vec2d &v3)
			{
				double Len[3] = { GetDistance(v1, v2), GetDistance(v2, v3), GetDistance(v3, v1) }; //���_�Ԃ̋���
				double s = 0.5 * (Len[0] + Len[1] + Len[2]);
				double ToSqrt = s * (s - Len[0]) * (s - Len[1]) * (s - Len[2]); //sqrt�֐��ɓ����O�i�K�̒l
				if (ToSqrt < 0) //sqrt�ɕ��̒l������Ɛ���ɋ��܂�Ȃ�����
					return 0;
				return sqrt(ToSqrt);
			}

			//�E������0�x�Ƃ����ꍇ�̔����v���̊p�x�����߂�
			double GetAngle() const { return atan2(this->y, this->x); }

			//�C�ӂ̕����x�N�g����0�x�Ƃ����ꍇ�̔����v���̊p�x�����߂�
			double GetAngle(const vec2d &vec) const
			{
				return GetAngle() - vec.GetAngle();
			}

			vec3d Getvec3d_XY(const double &z = 0.0) const; //XY���ʏ��vec3d�^�ɕϊ�����
			vec3d Getvec3d_XZ(const double &y = 0.0) const; //XZ���ʏ��vec3d�^�ɕϊ�����
			vec2d GetPerpendicular(const e2LR::e LR) const; //�����ȕ����̃x�N�g�����v�Z���ē���

			static double GetDistance(const vec2d &vec1, const vec2d &vec2) { return sqrt(pow(vec2.x - vec1.x, 2.0) + pow(vec2.y - vec1.y, 2.0)); } //2�̃x�N�g���Ԃ̋�����Ԃ�
			static double GetInnerProduct(const vec2d &vec1, const vec2d &vec2) { return vec1.x * vec2.x + vec1.y * vec2.y; } //���ς��v�Z����
			static double GetCrossProduct(const vec2d &vec1, const vec2d &vec2) { return vec1.x * vec2.y - vec2.x * vec1.y; } //�O�ς��v�Z����
			static bool GetAngle(double &out, const vec2d &vec1, const vec2d &vec2); //2�̃x�N�g���̐����p�x�����߂�
			static vec2d GetCrossReverse(const vec2d &vec, const double &z) { return vec2d(-vec.y * z, vec.x * z); }
			static vec2d GetCross(const vec2d &vec, const double &z) { return vec2d(vec.y * z, -vec.x * z); }
			//static bool GetCrossPoint(const vec2d &vecLine1Begin, const vec2d &vecLine1End, const vec2d &vecLine2Begin, const vec2d &vecLine2End, ho::Enable<ho::vec2d> &vecCrossPos, bool &Inner); //2�̒����̌�_�����߂�
		};

		class vec3d //double�^��3�����x�N�g��
		{
		public:
			//�R���X�g���N�^
			vec3d() { this->x = this->y = this->z = 0.0; }
			vec3d(const double &x, const double &y, const double &z) { this->x = x; this->y = y; this->z = z; }

			double x, y, z; //�����o�ϐ�

			//���Z�q
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
			//DirectXMath �Ƃ̃R���o�[�^
			vec3d(const XMFLOAT3 &obj) { this->x = obj.x; this->y = obj.y; this->z = obj.z; }
			operator XMFLOAT3() const { return XMFLOAT3(float(this->x), float(this->y), float(this->z)); }
#endif

			//���K�������x�N�g���𓾂�
			vec3d GetNormalize() const
			{
				double Denominator = sqrt(x * x + y * y + z * z); //������v�Z

				if (!Denominator) //���ꂪ0�̏ꍇ
					return vec3d(0, 0, 0);

				Denominator = 1.0 / Denominator; //�t��

				return vec3d(x * Denominator, y * Denominator, z * Denominator);
			}

			double GetPower() const { return sqrt(x * x + y * y + z * z); } //�x�N�g���̗ʂ�Ԃ�
			void clear() { this->x = this->y = this->z = 0.0; } //0�ɏ�����
			vec2d Getvec2d_XY() const { return vec2d(this->x, this->y); } //XY���ʏ�̍��W��vec2d�ɕϊ�����
			vec2d Getvec2d_XZ() const { return vec2d(this->x, this->z); } //XZ���ʏ�̍��W��vec2d�ɕϊ�����

			static double GetDistance(const vec3d &vec1, const vec3d &vec2) { return sqrt(pow(vec2.x - vec1.x, 2.0) + pow(vec2.y - vec1.y, 2.0) + pow(vec2.z - vec1.z, 2.0)); } //�x�N�g���Ԃ̋�����Ԃ�
			static double GetDot(const vec3d &vec1, const vec3d &vec2) { return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z; } //���ς����߂�
			static vec3d GetCross(const vec3d &vec1, const vec3d &vec2) { return vec3d(vec1.y * vec2.z - vec1.z * vec2.y, vec1.z * vec2.x - vec1.x * vec2.z, vec1.x * vec2.y - vec1.y * vec2.x); } //�O�ς����߂�
			static bool GetAngle(double &out, const vec3d &vec1, const vec3d &vec2); //2�̃x�N�g���Ԃ̐����p�̊p�x��Ԃ�
			static bool GetAngleAndAxis(double &Angle, vec3d &vecAxis, const vec3d &vec1, const vec3d &vec2); //2�̃x�N�g���Ԃ̐����p�̊p�x�Ɖ�]���x�N�g���𓾂�
			//static bool GetCrossPointsFromLines(std::vector<vec3d> &vecOut, vec3d pIn[4]); //2�̃x�N�g������Ȃ钼�����m�̌�_�܂��͈�Ԑڋ߂���_�����߂�
		};
	}
}

/*
�I�[�o�[���[�h���ꂽ���Z�q�̉��Z�Ȃǂ́A�R���X�g���N�^���Ă΂�邽�߂ɒx���Ȃ�B
+ �̑���� += �� Add() �� Sub() ���̉��Z���\�b�h���g���ƍ����ɂȂ邪�A������
�v�Z���ʂŏ㏑�������i+=�j���Ƃ�A�v�Z���ʊi�[�ϐ������K�v������iAdd���\�b�h�Ȃǁj���߁A
�����ǂ��g���邽�߂̔z�����K�v�ł���B
*/
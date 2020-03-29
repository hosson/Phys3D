//�N�H�[�^�j�I��
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
			Quaternion(vec3d vecAxis, const double &Radian); //�R���X�g���N�^
			Quaternion(Matrix Matrix); //�R���X�g���N�^�i3X3��]�s�񂩂�쐬�j
			~Quaternion(); //�f�X�g���N�^

			Matrix GetMatrix3x3() const; //3x3 �̉�]�����݂̂̍��W�ϊ��s��ɕϊ�
			Matrix GetMatrix4x4() const; //4x4 �̕��s�ړ��������܂߂����W�ϊ��s��ɕϊ�
			void TransformVector(vec3d &vec) const; //���W��ϊ�
			void TransformVector(vec3d &vecOut, const vec3d &vecIn) const; //���W��ϊ�
			void CalcAxisAndAngle(vec3d &vecAxis, double &Angle); //��]���Ɖ�]�p�x���v�Z����
			void Normalize(); //���K������
			Quaternion GetConjugate() const { return Quaternion(w, -x, -y, -z); } //�����l�����ƂȂ�N�H�[�^�j�I����Ԃ�

			static bool Slerp(Quaternion &Out, const Quaternion &qt1, const Quaternion &qt2, double t); //2�̃N�H�[�^�j�I�������ʐ��`��Ԃ���

			//���Z�q
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

/*�N�H�[�^�j�I�����g���ƁA��]�̏�Ԃ��L�q�ł���B
��]�s�񓙂ɔ�ׂď��Ȃ��ϐ��Ə��Ȃ��v�Z�ʂōς݁A
2�̃N�H�[�^�j�I���̊Ԃ���`���ʕ�Ԃ��邱�Ƃ��ł���B

�g�����Ƃ��ẮA
Quaternion(double Radian, VECTOR3DOUBLE *pvecN)
�̕��̃R���X�g���N�^�ŉ�]���x�N�g���Ɗp�x������ƁA
�C�ӎ��ɔC�ӊp�x��]������Ԃ�\���N�H�[�^�j�I�����ł���B
���̂Ƃ��x�N�g���͐��K���������̂���͂��Ȃ���΂Ȃ�Ȃ��B

2��]����������Ƃ��ɂ͐ώZ�i*���Z�q�j���s���B
����͏��Ԃɂ���ĈႤ���ʂƂȂ�B
qt3 = qt1 * qt2;
�ƋL�q����ƁAqt1�̉�]���s�������Ƃ�qt2�̉�]���s������Ԃ̉�]��
qt3�̒��ɓ���B

���ۂɍ��W����]������ɂ́ACalcD3DXMatrix���ō��W�ϊ��s��𓾂�
���W���s��ŕϊ�������B�܂��́ATransformVector���g���΁A
���ڂɕϊ����邱�Ƃ��ł���B
*/
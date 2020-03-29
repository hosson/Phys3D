#include "Math.h"
#include <Windows.h>
#include "Vector.h"

namespace ho {
	//����ǂ����𔻒�
	bool Odd(const int &val) { return (val % 2) != 0; }
	bool Odd(const unsigned int &val) { return (val % 2) != 0; }

	//�������ǂ����𔻒�
	bool Even(const int &val) { return (val % 2) == 0; }
	bool Even(const unsigned int &val) { return (val % 2) == 0; }

	//�l�� 0.0�`2�� �܂ł̈ʑ��ɕϊ�����
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


	//1�����z���2�����z��ƌ��Ȃ����ꍇ�ɁAX��Y�̍��W����1�����z���̗v�f���𓾂�
	inline int GetVectorPosFromXY(int Width, int X, int Y)
	{
		return Y * Width + X;
	}

	//1�����z���2�����z��ƌ��Ȃ����ꍇ�ɁA1�����z���̗v�f������X��Y�̍��W�𓾂�
	inline PT<int> GetXYFromVectorPos(int Width, int VectorPos)
	{
		return PT<int>(VectorPos % Width, VectorPos / Width);
	}






	const float SinTable::Ratio = 10430.378350470452724949566316381f; // 65536 / (2 * PI)
	float SinTable::Table[65536];

	BOOL SinTable::Init() //������
	{
		int i;

		//�e�[�u���f�[�^���v�Z
		for (i = 0; i < 65536; i++)
			Table[i] = sin(float(i) / Ratio);

		return TRUE;
	}

	BOOL SinTable::Dest() //�I������
	{
		return TRUE;
	}










	//�R���X�g���N�^�i3�_�����ʕ��ʕ����������߂�j
	PLANE::PLANE(const VECTOR3DOUBLE &vec1, const VECTOR3DOUBLE &vec2, const VECTOR3DOUBLE &vec3)
	{
		VECTOR3DOUBLE v1 = vec2 - vec1, v2 = vec3 - vec1;
		VECTOR3DOUBLE vecN = VECTOR3DOUBLE::GetCrossProduct(v1, v2); //�O�σx�N�g�������߂�

		/*�_P(x0, y0, z0)��ʂ�C�@���x�N�g���� ��N(a, b, c)�̕��ʂ̕������́A
		a(x-x0)+b(y-y0)+c(z-z0)=0�ƂȂ�B
		��ʕ��ʕ����� ax+by+cz+d=0 �ł́A�@���x�N�g����(a, b, c)�ƂȂ�B*/
	
		this->a = vecN.x;
		this->b = vecN.y;
		this->c = vecN.z;
		this->d = -(vec1.x * vecN.x + vec1.y * vecN.y + vec1.z * vecN.z);
	}

	//�R���X�g���N�^�i�p�����[�^�𒼐ڎw��j
	PLANE::PLANE(const double &a, const double &b, const double &c, const double &d) 
	{
		/*a, b, c �͕��ʂ̖@���x�N�g���ƂȂ�B
		d �͌��_����̋����ƂȂ�B*/

		this->a = a;
		this->b = b;
		this->c = c;
		this->d = d;
	}

	//�R���X�g���N�^�i�@���x�N�g���ƌ��_����̋����j
	PLANE::PLANE(const D3DXVECTOR3 &vecNormal, const double &d) 
	{
		this->a = vecNormal.x;
		this->b = vecNormal.y;
		this->c = vecNormal.z;
		this->d = d;
	}

	//���ʕ��������畽�ʂ̖@���x�N�g���𓾂�
	VECTOR3DOUBLE PLANE::GetNormalLine() const
	{
		return VECTOR3DOUBLE(this->a, this->b, this->c);
	}

	//2�̒��_�Ŏ�����钼���Ƃ̌�_�����߂�
	bool PLANE::GetCrossPointFromLine(VECTOR3DOUBLE &out, const VECTOR3DOUBLE &vecPt1, const VECTOR3DOUBLE &vecPt2) const
	{
		/*
		���ʂe�� a x + b y + c z = d�@�E�E�E���@
		����AB���A�}��ϐ�t��p���āA
		P = A + t e�@�E�E�E���A
	�@	 A = ( Ax, Ay, Az )
	�@	 e = [ Bx-Ax By-Ay Bz-Az ]
	�@�@	  = [ Ex Ey Ez ]
		�Ƃ��܂��B
	
		���A�𐬕��ɕ������܂��B
		x = Ax + t Ex
		y = Ay + t Ey
		z = Az + t Ez

		���@�ɑ�����܂��B
	�@	 a x + b y + c z = d
		��a (Ax + t Ex) + b ( Ay + t Ey ) + c ( Az + t Ez ) = d
		��a Ax + t a Ex + b Ay + t b Ey + c Az + t c Ez = d
		��t ( a Ex + b Ey + c Ez ) = d - a Ax - b Ay - c Az
		��t = { d - ( a Ax + b Ay + c Az ) } / ( a Ex + b Ey + c Ez )

		DE = a Ex + b Ey + c Ez
		�Ƃ��܂��B

		DE���[���ȂƂ�
	�@	 ��_�͂���܂���i���ʂ`�Ɛ����`�a�͕��s�ł��j
		DE���[���łȂ��Ƃ�
	�@	 ���ʂ`�ƒ����`�a�Ƃ̌�_�́A
	�@	 P = A + t e
	�@�@	  A = ( Ax, Ay, Az )
	�@�@	  e = [ Bx-Ax By-Ay Bz-Az ]
	�@�@	  t = { d - ( a Ax + b Ay + c Az ) } / ( a Ex + b Ey + c Ez )
	�@	 e < 0 : �_�o�͓_�`����O
	�@	 e > 1 : �_�o�͓_�`��艜
		 */

		VECTOR3DOUBLE vecE = vecPt2 - vecPt1; //�_1����_2�܂ł̃x�N�g��

		if (a * vecE.x + b * vecE.y + c * vecE.z == 0.0) //���ʂƒ����͕��s�Ȃ̂Ō�_�͖����ꍇ
			return false;

		//�}��ϐ�t�����߂�
		double t = (-d - (a * vecPt1.x + b * vecPt1.y + c * vecPt1.z)) / (a * vecE.x + b * vecE.y + c * vecE.z);

		out = vecPt1 + vecE * t; //P=A+te�ix=Ax+tEx, y=Ay+tEy, z=Az+tEz�j���v�Z

		return true;
	}

	//X, Y, Z �̒�����W���̐�Βl���ł��傫�������ǂꂩ�𒲂ׂ�
	PLANE::Param PLANE::GetMaxAbsParam() 
	{
		double a = abs(this->a), b = abs(this->b), c = abs(this->c); //��Βl�ɕϊ�

		if (a >= b && a >= c)
			return X;
		if (b >= a && b >= c)
			return Y;
		if (c >= a && c >= b)
			return Z;

		return Param(-1); //�G���[
	}

	//���ʏ�ŔC�ӂ̎����C�ӂ̗ʓ��������ꍇ�ɁA�ʂ̂���C�ӂ̎��̓����ʂ𓾂�
	double PLANE::GetOneAxisMoveVector(Param InVector, const double &In, Param OutVector) const
	{
		double abc[] = {a, b, c}; //���̌W����z��Ɋi�[

		if (abc[InVector] == 0.0 || abc[OutVector] == 0.0) //�ǂ��炩�̔C�ӎ����Œ肳�ꂽ���ʂ������ꍇ
			return 0; //�ړ��ł��Ȃ�

		//a(x + 0) + b(y + 0) + c(z + 0) = 0 �̕��ʎ������ʂ𓾂�������=�̔��Α��Ɉڍs���A0 �̕����̕ω��ʂ��v�Z����
		return (abc[InVector] * -In) / abc[OutVector];
	}

	//2�̕��ʕ������������i�x�N�g���ƒʂ�1�_�̍��W�j�����߂�
	bool PLANE::GetIntersectionLine(VECTOR3DOUBLE &vecDirection, VECTOR3DOUBLE &vecPoint, const PLANE &pln1, const PLANE &pln2)
	{
		//�Q�lURL:http://www.hiramine.com/programming/graphics/3d_planeintersection.html
		//��:D3DXPLANE�ł�d�����ӂɂ��邪�A��LURL�ł͉E�ӂɂ��邽�߁A���̊֐����Ŏg����d�ɂ͒���-��t���Ă��킹�Ă���

		/*����̕����x�N�g�������߂�B
		����̕����x�N�g�� �́A2�̕��ʂ̖@���x�N�g���Ɛ����ȃx�N�g���B
		2�̃x�N�g���Ɛ����ȃx�N�g���́A�O�όv�Z�ɂ�苁�܂�B*/
		vecDirection.x = pln1.b * pln2.c - pln1.c * pln2.b;
		vecDirection.y = pln1.c * pln2.a - pln1.a * pln2.c;
		vecDirection.z = pln1.a * pln2.b - pln1.b * pln2.a;

		/*������ʂ�C�ӂ̓_�����߂�B
		2�̕��ʕ�������A��������B*/
		if (abs(vecDirection.z) >= abs(vecDirection.x) && abs(vecDirection.z) >= abs(vecDirection.y) && vecDirection.z != 0.0f) //�����x�N�g����Z�������̐�Βl����ԑ傫��0�ȊO�̏ꍇ
		{
			vecPoint.x = (-pln1.d * pln2.b - -pln2.d * pln1.b) / vecDirection.z;
			vecPoint.y = (-pln1.d * pln2.a - -pln2.d * pln1.a) / -vecDirection.z;
			vecPoint.z = 0;
		} else if (abs(vecDirection.y) >= abs(vecDirection.x) && abs(vecDirection.y) >= abs(vecDirection.z) && vecDirection.y != 0.0f) //�����x�N�g����Y�������̐�Βl����ԑ傫��0�ȊO�̏ꍇ
		{
			vecPoint.x = (-pln1.d * pln2.c - -pln2.d * pln1.c) / -vecDirection.y;
			vecPoint.z = (-pln1.d * pln2.a - -pln2.d * pln1.a) / vecDirection.y;
			vecPoint.y = 0;
		} else if (abs(vecDirection.x) >= abs(vecDirection.y) && abs(vecDirection.x) >= abs(vecDirection.z) && vecDirection.x != 0.0f) //�����x�N�g����X�������̐�Βl����ԑ傫��0�ȊO�̏ꍇ
		{
			vecPoint.y = (-pln1.d * pln2.c - -pln2.d * pln1.c) / vecDirection.x;
			vecPoint.z = (-pln1.d * pln2.b - -pln2.d * pln1.b) / -vecDirection.x;
			vecPoint.x = 0;
		} else { //�����x�N�g���̑S������0�̏ꍇ��2�̕��ʂ͕��s�Ō�����Ȃ�
			return false;
		}

		vecDirection = vecDirection.GetNormalize(); //�����x�N�g���𐳋K������

		return true;
	}
}

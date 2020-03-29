//���w�֘A�̃��C�u����
#pragma once
//#include <d3dx9math.h>
#include "D3DVertex.h"
#include <stdlib.h>
#include <list>
#include <stdarg.h>
#include "Debug.h"

#define PI 3.14159265358979323846 //�~����
#define PI2 6.283185307179586476925286766559 //�~����*2
#define PI_HALF 1.5707963267948966192313216916398 //�~����*0.5
          
namespace ho {
	//�O���錾
	class VECTOR2DOUBLE;
	class VECTOR3DOUBLE;

	//�ϐ��̕�����Ԃ�
	template <typename T> int GetSign(const T &Value)
	{
		if (Value > 0)
			return 1;
		else if (Value < 0)
			return -1;

		return 0;
	}

	//�ϐ��̒l���w��͈͓��Ɏ��߂�
	template <typename T> void Clamp(T &Value, const T &Min, const T &Max)
	{
		if (Value < Min)
			Value = Min;
		else if (Value > Max)
			Value = Max;

		return;
	}

	//�ϐ��̒l���w��͈͓��Ɏ��߂�
	template <typename T> T Clamp(const T &Value, const T &Min, const T &Max)
	{
		if (Value < Min)
			return Min;
		else if (Value > Max)
			return Max;

		return Value;
	}

	//����lValue���n�_Begin�ƏI�_End�͈͓̔��ɑ��݂��邩�ǂ����𒲂ׂ�
	template <typename T> bool JudgeScope(const T &Begin, const T &End, const T &Value) 
	{
		if (Begin < End)
		{
			if (Value >= Begin && Value <= End)
				return true; //�͈͓�
		} else if (Begin > End) {
			if (Value >= End && Value <= Begin)
				return true; //�͈͓�
		} else { //Begin��End���������ꍇ
			if (Value == Begin) //2�̓������_���Value�����݂���ꍇ
				return true; //�͈͓��Ɖ���
		}

		return false; //�͈͊O
	}

	//����lValue��Min=0.0�AMax=1.0�Ƃ����Ƃ��A�ǂ̊����ɂ��邩�𓾂�
	template <typename T> double GetRatio(const T &Value, const T &Min, const T &Max)
	{
		return double(Value - Min) / double(Max - Min);
	}

	//�C�ӂ�Min��Max�̒l�̒�����A�����i0.0�`1.0�j��Ratio�Ƃ���Ƃ��̒l�𓾂�
	template <typename T> T GetValueFromRatio(const double &Ratio, const T &Min, const T &Max)
	{
		return T(Min + (Max - Min) * Ratio);
	}
	//��L��float��
	template <typename T> T GetValueFromRatioF(const float &Ratio, const T &Min, const T &Max)
	{
		return T(Min + (Max - Min) * Ratio);
	}

	//2�̕ϐ��̓��e����������
	template <typename T> void Swap(T &a, T &b)
	{
		T Buf = a;
		a = b;
		b = Buf;

		return;
	}

	//2�̕ϐ��̂����̑傫���ق��̒l�𓾂�
	template <typename T> T Max(const T &a, const T &b)
	{
		if (a > b)
			return a;
		return b;
	}

	//2�̕ϐ��̂����̏������ق��̒l�𓾂�
	template <typename T> T Min(const T &a, const T &b)
	{
		if (a < b)
			return a;
		return b;
	}

	//����ǂ����𔻒�
	bool Odd(const int &val);
	bool Odd(const unsigned int &val);

	//�������ǂ����𔻒�
	bool Even(const int &val);
	bool Even(const unsigned int &val);

	//�l�� 0.0�`2�� �܂ł̈ʑ��ɕϊ�����
	float PhaseLimit(const float &Phase);
	void PhaseLimit(float &Phase);
	double PhaseLimit(const double &Phase);
	void PhaseLimit(double &Phase);

	//�p�x�N���X
	//���̃N���X���g���Ɗp�x�̒l���K��0.0�`2�΂̊ԂɂȂ�
	template <class T> class ANGLE
	{
	public:
		ANGLE() {} //�R���X�g���N�^
		ANGLE(const T &Angle) //�R���X�g���N�^
		{
			this->a = PhaseLimit(Angle);
		}
		~ANGLE() {} //�f�X�g���N�^

		//�A�N�Z�b�T
		T Get() const { return a; }

		//���Z�q
		ANGLE operator +(const ANGLE &obj) const { return ANGLE(this->a + obj.a); }
		ANGLE& operator +=(const ANGLE &obj) { this->a = PhaseLimit(this->a + obj.a); return *this; }

		ANGLE operator -(const ANGLE &obj) const { return ANGLE(this->a - obj.a); }
		ANGLE& operator -=(const ANGLE &obj) { this->a = PhaseLimit(this->a - obj.a); return *this; }

		ANGLE operator *(const int &value) const { return ANGLE(this->a * value); }
		ANGLE operator *(const float &value) const { return ANGLE(this->a * value); }
		ANGLE operator *(const double &value) const { return ANGLE(this->a * value); }
		ANGLE& operator *=(const int &value) { this->a = PhaseLimit(this->a * value); return *this; }
		ANGLE& operator *=(const float &value) { this->a = PhaseLimit(this->a * value); return *this; }
		ANGLE& operator *=(const double &value) { this->a = PhaseLimit(this->a * value); return *this; }

		ANGLE operator /(const int &value) const { return ANGLE(this->a / value); }
		ANGLE operator /(const float &value) const { return ANGLE(this->a / value); }
		ANGLE operator /(const double &value) const { return ANGLE(this->a / value); }
		ANGLE& operator /=(const int &value) { this->a = PhaseLimit(this->a / value); return *this; }
		ANGLE& operator /=(const float &value) { this->a = PhaseLimit(this->a / value); return *this; }
		ANGLE& operator /=(const double &value) { this->a = PhaseLimit(this->a / value); return *this; }

		//���Z���A�Z�����œ��B�ł�������̊p�x��Ԃ�
		T operator |(const ANGLE &obj) const
		{
			T Sub = this->a - obj.a; //���Z���Ĉʑ������ɂ�����
			if (Sub < -PI) //0�x�̈ʒu���܂����ꍇ�̏��u
				PhaseLimit(Sub);
			else if (Sub > PI) //180�x���傫�Ȋp�x�ɂȂ��Ă��܂����ꍇ
				Sub = Sub - PI2;

			return Sub;
		}
	private:
		T a; //�p�x�̒l
	};

	//�䗦�^�i�l�� 0.0�`1.0 �͈̔͂Ɍ��肷��j
	class RATIO
	{
	public:
		RATIO() { this->val = 0.0; } //�R���X�g���N�^
		RATIO(const double &val) { this->val = val; } //�R���X�g���N�^
		~RATIO() {} //�f�X�g���N�^

		double val;

		RATIO operator +(const RATIO &obj) const { return RATIO(Clamp(this->val + obj.val, 0.0, 1.0)); }
		RATIO& operator +=(const RATIO &obj) { this->val = Clamp(this->val + obj.val, 0.0, 1.0); return *this; }
		RATIO operator -(const RATIO &obj) const { return RATIO(Clamp(this->val - obj.val, 0.0, 1.0)); }
		RATIO& operator -=(const RATIO &obj) { this->val = Clamp(this->val - obj.val, 0.0, 1.0); return *this; }
		RATIO operator *(const RATIO &obj) const { return RATIO(Clamp(this->val * obj.val, 0.0, 1.0)); }
		RATIO& operator *=(const RATIO &obj) { this->val = Clamp(this->val * obj.val, 0.0, 1.0); return *this; }
		RATIO operator /(const RATIO &obj) const { return RATIO(Clamp(this->val / obj.val, 0.0, 1.0)); }
		RATIO& operator /=(const RATIO &obj) { this->val = Clamp(this->val / obj.val, 0.0, 1.0); return *this; }

		double operator ()() const { return val; }
	};

	int GetVectorPosFromXY(int Width, int X, int Y); //1�����z���2�����z��ƌ��Ȃ����ꍇ�ɁAX��Y�̍��W����1�����z���̗v�f���𓾂�
	PT<int> GetXYFromVectorPos(int Width, int VectorPos); //1�����z���2�����z��ƌ��Ȃ����ꍇ�ɁA1�����z���̗v�f������X��Y�̍��W�𓾂�

	//sin�֐��e�[�u��
	class SinTable
	{
	public:
		static const float Ratio; // 65536 / (2 * PI)
		static float Table[65536]; //�e�[�u���f�[�^

		static BOOL Init(); //������
		static BOOL Dest(); //�I������
	};

	class PLANE //��ʕ��ʕ�����
	{
	public:
		enum Param { X, Y, Z }; //�W���̍���\���񋓑�

		double a, b, c, d; //ax + by + cz + d = 0 �̌`�ƂȂ�

		PLANE() {} //�R���X�g���N�^
		PLANE(const VECTOR3DOUBLE &vec1, const VECTOR3DOUBLE &vec2, const VECTOR3DOUBLE &vec3); //�R���X�g���N�^�i3�_�̍��W�����ʕ��ʕ����������߂�j
		PLANE(const double &a, const double &b, const double &c, const double &d); //�R���X�g���N�^�i�p�����[�^�𒼐ڎw��j
		PLANE(const D3DXVECTOR3 &vecNormal, const double &d); //�R���X�g���N�^�i�@���x�N�g���ƌ��_����̋����j

		VECTOR3DOUBLE GetNormalLine() const;//���ʕ��������畽�ʂ̖@���x�N�g���𓾂�
		bool GetCrossPointFromLine(VECTOR3DOUBLE &out, const VECTOR3DOUBLE &vecPt1, const VECTOR3DOUBLE &vecPt2) const; //2�̒��_�Ŏ�����钼���Ƃ̌�_�����߂�
		Param GetMaxAbsParam(); //X, Y, Z �̒�����W���̐�Βl���ł��傫�������ǂꂩ�𒲂ׂ�
		double GetOneAxisMoveVector(Param InVector, const double &In, Param OutVector) const; //���ʏ�ŔC�ӂ̎����C�ӂ̗ʓ��������ꍇ�ɁA�ʂ̂���C�ӂ̎��̓����ʂ𓾂�

		static bool GetIntersectionLine(VECTOR3DOUBLE &vecDirection, VECTOR3DOUBLE &vecPoint, const PLANE &pln1, const PLANE &pln2); //2�̕��ʕ������̌�������߂�
	};

	/* ���`�O���t�́A�����̓_�ō\�������2�����̃O���t�ł���B
	Add() �ŗv�f��ǉ����Ă����ƁAGet() �ŔC�ӂ�x�l�ɑΉ�����
	y�̒l����`��Ԃ��Ȃ��瓾�邱�Ƃ��ł���B*/
	template <class T, class U> class LinearGraph //���`�O���t
	{
	public:
		struct POINT //��v�f��\���\����
		{
			T x; //�����̒l
			U y; //�c���̒l
		};
	public:
		LinearGraph(const U &Default) : Default(Default), Zero(Default) {} //�R���X�g���N�^
		LinearGraph(const U &Default, const U &Zero) : Default(Default), Zero(Zero) {} //�R���X�g���N�^
		~LinearGraph() {} //�f�X�g���N�^

		//�v�f��ǉ�
		void Add(const T &x, const U &y)
		{
			for (std::list<POINT>::iterator itr = listPoint.begin(); itr != listPoint.end();) //���X�g�𑖍�
			{
				if (itr->x == x) //x�l���d������ꍇ
					itr = listPoint.erase(itr); //���X�g���̗v�f���폜
				else
					itr++;
			}

			POINT Point;
			Point.x = x;
			Point.y = y;

			listPoint.push_back(Point); //���X�g�ɒǉ�

			//x�l�̏��������Ƀ\�[�g
			listPoint.sort([](const POINT &Point1, const POINT &Point2) { return Point1.x < Point2.x; });

			return;
		}

		//�C�ӂ�x�ɑΉ�����y�̒l���擾
		U Get(const T &x) const
		{
			if (!listPoint.size()) //�v�f�����݂��Ȃ��ꍇ
				return Default; //�f�t�H���g�l���Ԃ�

			for (std::list<POINT>::const_iterator itr = listPoint.begin(); itr != listPoint.end(); itr++) //���X�g�𑖍�
			{
				if (x < itr->x) //�������̗v�f���x�l���������Ȃ����ꍇ
				{
					if (itr == listPoint.begin()) //�ŏ��̗v�f�̏ꍇ
						return itr->y;

					//���݂̈�O�̃C�e���[�^���擾
					std::list<POINT>::const_iterator itrPrev = itr;
					itrPrev--;

					double Ratio = double(x - itrPrev->x) / double(itr->x - itrPrev->x); //2�̗v�f�Ԃ̔䗦

					return itrPrev->y + (itr->y - itrPrev->y) * Ratio; //���`���
				}
			}

			//�S�Ă̗v�f���x���傫���ꍇ
			std::list<POINT>::const_iterator itr = listPoint.end();
			itr--;
			return itr->y; //�Ō�̗v�f��y�l��Ԃ��Ă���
		}
		//�ŏ��̃|�C���g�𓾂�
		ho::Enable<POINT> GetFrontPoint() const
		{
			if (listPoint.size() == 0) //�|�C���g�����݂��Ȃ��ꍇ
				return ho::Enable<POINT>(); //�����l��Ԃ��Ă���

			return ho::Enable<POINT>(listPoint.front());
		}
		//�Ō�̃|�C���g�𓾂�
		ho::Enable<POINT> GetBackPoint() const
		{
			if (listPoint.size() == 0) //�|�C���g�����݂��Ȃ��ꍇ
				return ho::Enable<POINT>(); //�����l��Ԃ��Ă���

			return ho::Enable<POINT>(listPoint.back());
		}
		//��Ԑϕ����s��
		U GetIntervalIntegration(const T &Begin, const T &End) const
		{
			U Area = Zero; //�ʐ�

			try
			{
				if (Begin > End) //�n�_�̕����傫���ꍇ
					throw(TEXT("�ϕ���Ԃ��s���ł�"), __WFILE__, __LINE__, false);
				if (Begin == End) //�n�_�ƏI�_�������ʒu�̏ꍇ
					return Zero;

				if (!listPoint.size()) //�v�f�����݂��Ȃ��ꍇ
					return Default * (End - Begin); //�f�t�H���g�l�Őϕ������l���Ԃ�

				bool Integration = false; //�ϕ������ǂ���
				for (std::list<POINT>::const_iterator itr = listPoint.begin(); itr != listPoint.end(); itr++) //���X�g�𑖍�
				{
					if (Integration) //�ϕ����̏ꍇ
					{
						if (End <= itr->x) //�������̗v�f���I���̒l���������Ȃ����ꍇ
						{
							std::list<POINT>::const_iterator itrPrev = itr;
							itrPrev--;
							Area += GetArea(*itrPrev, *itr, ho::Enable<T>(), ho::Enable<T>(End)); //�ʐς��v�Z
							Integration = false; //�ϕ����I��
							break;
						} else { //�n�܂�̒l���I���̒l���܂܂�Ȃ��ꍇ
							std::list<POINT>::const_iterator itrPrev = itr;
							itrPrev--;
							Area += GetArea(*itrPrev, *itr, ho::Enable<T>(), ho::Enable<T>()); //�ʐς��v�Z
						}
					} else { //�܂��ϕ����n�܂��Ă��Ȃ��ꍇ
						if (Begin <= itr->x) //�������̗v�f���n�܂�̒l���������Ȃ����ꍇ
						{
							if (End <= itr->x) //�������̗v�f���I���̒l���������Ȃ����ꍇ
							{
								if (itr == listPoint.begin()) //�ŏ��̗v�f�̏ꍇ
								{
									Area += GetArea(ho::Enable<POINT>(), *itr, ho::Enable<T>(Begin), ho::Enable<T>(End)); //�ʐς��v�Z
									break;
								} else {
									std::list<POINT>::const_iterator itrPrev = itr;
									itrPrev--;
									Area += GetArea(*itrPrev, *itr, ho::Enable<T>(Begin), ho::Enable<T>(End)); //�ʐς��v�Z
									break;
								}
							} else { //�I���̒l�͊܂܂�Ă��Ȃ��ꍇ
								if (itr == listPoint.begin()) //�ŏ��̗v�f�̏ꍇ
								{
									Area += GetArea(ho::Enable<POINT>(), *itr, ho::Enable<T>(Begin), ho::Enable<T>()); //�ʐς��v�Z
								} else {
									std::list<POINT>::const_iterator itrPrev = itr;
									itrPrev--;
									Area += GetArea(*itrPrev, *itr, ho::Enable<T>(Begin), ho::Enable<T>()); //�ʐς��v�Z
								}
								Integration = true; //�ϕ����Ƃ���
							}
						}
					}
				}

				if (Integration) //�܂��I���C�����܂Őϕ����Ă��Ȃ��̂Ƀ��X�g���I����Ă��܂����ꍇ
				{
					std::list<POINT>::const_iterator itr = listPoint.end();
					itr--; //�Ō�̗v�f�̃C�e���[�^���擾
					Area += GetArea(*itr, ho::Enable<POINT>(), ho::Enable<T>(), ho::Enable<T>(End)); //�ʐς��v�Z
				}
			}
			catch(const ho::Exception &e)
			{
				ER(e.strMsg.c_str(), e.pFile, e.Line, e.Continue);
			}

			return Area;
		}

		//�A�N�Z�b�T
		const std::list<POINT> &GetlistPoint() const { return listPoint; }
	private:
		const U Default; //�v�f�����݂��Ȃ��ꍇ�ɕԂ��f�t�H���gy�l
		const U Zero; //�v�Z�J�n���Ȃǂ�0�������������ꍇ��0�ɊY������y�l
		std::list<POINT> listPoint; //�v�f���X�g

		//BeginPoint��EndPoint�ō\��������`�́ABegin����End�܂ł̖ʐς��v�Z����
		U GetArea(const ho::Enable<POINT> &BeginPoint, const ho::Enable<POINT> &EndPoint, const ho::Enable<T> &BeginLine, const ho::Enable<T> &EndLine) const
		{
			//���C���ʒu�̃|�C���g��Ԃ������_��
			auto fGetPointFromLine = [&](const ho::Enable<T> &Line)->POINT
			{
				POINT Point;
				Point.x = Line();
				Point.y = this->Get(Line());

				return Point;
			};

			POINT Left, Right; //��`���\�����鍶�ƉE�̌v�Z�p�|�C���g

			try
			{
				if (BeginLine.operator!() && EndLine.operator!()) //�n�܂胉�C���ƏI��胉�C�����w�肳��Ă���ꍇ
					if (BeginLine() >= EndLine()) //�n�܂胉�C�����I��胉�C���Ɠ������E���̏ꍇ
						throw(TEXT("�ϕ���Ԃ��s���ł��B"), __WFILE__, __LINE__, false);
	
				//���̌v�Z�p�|�C���g�Ɋւ��鏈��
				if (BeginPoint.operator!()) //���̃|�C���g���L���̏ꍇ
				{
					if (EndLine.operator!()) //�E�̃��C�����L���ȏꍇ
						if (EndLine() < BeginPoint().x) //�E�̃��C�������̃|�C���g�������ɂ���ꍇ
							throw(TEXT("�ϕ���Ԃ��s���ł��B"), __WFILE__, __LINE__, false);
					if (BeginLine.operator!()) //���̃��C�����L���̏ꍇ
					{
						if (BeginLine() < BeginPoint().x) //���C�����|�C���g��荶�ɂ���ꍇ
						{
							Left = BeginPoint(); //�|�C���g���n�_�Ƃ���
						} else {
							Left = fGetPointFromLine(BeginLine); //���C���ʒu���n�_�Ƃ���
						}
					} else { //���̃��C���������̏ꍇ
						Left = BeginPoint(); //�|�C���g���n�_�Ƃ���
					}
				} else { //���̃|�C���g�������̏ꍇ
					if (BeginLine.operator!()) //���̃��C�����L���̏ꍇ
					{
						Left = fGetPointFromLine(BeginLine); //���C���ʒu���n�_�Ƃ���
					} else { //���̃��C���������̏ꍇ
						throw(TEXT("�ϕ���Ԃ��s���ł��B"), __WFILE__, __LINE__, false);
					}
				}
	
				//�E�̌v�Z�p�|�C���g�Ɋւ��鏈��
				if (EndPoint.operator!()) //�E�̃|�C���g���L���̏ꍇ
				{
					if (BeginLine.operator!()) //���̃��C�����L���ȏꍇ
						if (BeginLine() > EndPoint().x) //���̃��C�����E�̃|�C���g�����E�ɂ���ꍇ
							throw(TEXT("�ϕ���Ԃ��s���ł��B"), __WFILE__, __LINE__, false);
					if (EndLine.operator!()) //�E�̃��C�����L���̏ꍇ
					{
						if (EndLine() > EndPoint().x) //���C�����|�C���g���E�ɂ���ꍇ
						{
							Right = EndPoint(); //�|�C���g���n�_�Ƃ���
						} else {
							Right = fGetPointFromLine(EndLine); //���C���ʒu���I�_�Ƃ���
						}
					} else { //�E�̃��C���������̏ꍇ
						Right = EndPoint(); //�|�C���g���I�_�Ƃ���
					}
				} else { //�E�̃|�C���g�������̏ꍇ
					if (EndLine.operator!()) //�E�̃��C�����L���̏ꍇ
					{
						Right = fGetPointFromLine(EndLine); //���C���ʒu���I�_�Ƃ���
					} else { //�E�̃��C���������̏ꍇ
						throw(TEXT("�ϕ���Ԃ��s���ł��B"), __WFILE__, __LINE__, false);
					}
				}
			}
			catch(const ho::Exception &e)
			{
				ER(e.strMsg.c_str(), e.pFile, e.Line, e.Continue);
			}

			return (Left.y + Right.y) * (Right.x - Left.x) * 0.5;
		}
	};

	//���`2�����O���t�𑦐Ȃō쐬���A��������l�𓾂�
	template <class T, class U> U GetLinearGraph(const T &x, DWORD Num, ...)
	{
		/* ���� x �͎擾�������l�̃O���t���x��
		Num �̓O���t��̓_�̐�
		�Ȍ�̉ϒ������́Ax �� y�� ��2�̕ϐ��� Num �������ē��͂���B
		class U �� unsigned �Ȍ^���w�肷��ƌv�Z�ߒ��Ń}�C�i�X�������s���エ�������Ȃ�̂ŕs�B
		*/

		va_list arg;
		va_start(arg, Num);

		T X[2];
		U Y[2];
		for (DWORD i = 0; i < Num; i++)
		{
			X[i % 2] = va_arg(arg, T);
			Y[i % 2] = va_arg(arg, U);
			if (x <= X[i % 2])
			{
				va_end(arg);

				if (i == 0)
				{
					return Y[0];
				} else {
					if (X[i % 2] - X[(i - 1) % 2] <= 0) //X���̕��т��K���ł͂Ȃ��ꍇ
						return U(Y[i % 2]); //�K���ɕԂ�
					return U(Y[(i - 1) % 2] + (Y[i % 2] - Y[(i - 1) % 2]) * ((x - X[(i - 1) % 2]) / double(X[i % 2] - X[(i - 1) % 2])));
				}
			}
		}

		va_end(arg);

		return Y[(Num - 1) % 2];
	}
	template <> void GetLinearGraph<double, DWORD>(const double &x, DWORD Num, ...); //�w�肵�Ă͂����Ȃ��^����ꉻ
	template <> void GetLinearGraph<float, DWORD>(const float &x, DWORD Num, ...); //�w�肵�Ă͂����Ȃ��^����ꉻ
	template <> void GetLinearGraph<int, DWORD>(const int &x, DWORD Num, ...); //�w�肵�Ă͂����Ȃ��^����ꉻ
	template <> void GetLinearGraph<unsigned int, DWORD>(const unsigned int &x, DWORD Num, ...); //�w�肵�Ă͂����Ȃ��^����ꉻ
	template <> void GetLinearGraph<DWORD, DWORD>(const DWORD &x, DWORD Num, ...); //�w�肵�Ă͂����Ȃ��^����ꉻ

	//�����Ȑ����݂̂ō\�������2�����O���t�𑦐Ȃō쐬���A��������l�𓾂�
	namespace eJust //x�̒l���_�ƈ�v�����ꍇ�̋�����\���񋓑�
	{
		enum e
		{
			Truncation,	//�؎̂�
			Conclusion,	//�؏グ
			Average,	//���ϒl
		};
	};
	template <class T, class U> U GetHorizontalLinearGraph(T x, eJust::e Just, DWORD Num, ...)
	{
		/* ���� x �͎擾�������l�̃O���t���x��
		Num �̓O���t��̓_�̐�
		�Ȍ�̉ϒ������́Ay���̍����Ax���̋�؂�Ay���̍����̏��œ����B
		���� Num �̐����� x���̋�؂肪����A�Ō�͕K��y���̍����ŏI���B
		*/

		va_list arg;
		va_start(arg, Num);

		T X;
		U Y;

		switch (Just)
		{
		case eJust::Truncation: //�؎̂�
			for (DWORD i = 0; i < Num; i++)
			{
				Y = va_arg(arg, U);
				X = va_arg(arg, T);
				if (x <= X)
				{
					va_end(arg);
					return Y;
				}
			}
			Y = va_arg(arg, U);
			va_end(arg);
			return Y;
			break;
		case eJust::Conclusion: //�؏グ
			for (DWORD i = 0; i < Num; i++)
			{
				Y = va_arg(arg, U);
				X = va_arg(arg, T);
				if (x < X)
				{
					va_end(arg);
					return Y;
				}
			}
			Y = va_arg(arg, U);
			va_end(arg);
			return Y;
			break;
		case eJust::Average: //����
			for (DWORD i = 0; i < Num; i++)
			{
				Y = va_arg(arg, U);
				X = va_arg(arg, T);
				if (x <= X)
				{
					va_end(arg);
					return Y;
				} else if (x == X)
				{
					U Y2 = va_arg(arg, U);
					va_end(arg);
					return U((Y + Y2) * 0.5);
				}
			}
			Y = va_arg(arg, U);
			va_end(arg);
			return Y;
			break;
		}

		ER(TEXT("�l����v�����ꍇ�̏������w�肳��Ă��܂���ł����B"), __WFILE__, __LINE__, false);

		return 0;
	}
}

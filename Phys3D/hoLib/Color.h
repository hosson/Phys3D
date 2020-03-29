//�F���N���X
#pragma once
#include <Windows.h>

namespace ho
{
	class ARGB
	{
	public:
		struct Byte
		{
			BYTE b, g, r, a;
		};
	public:
		ARGB() {} //�R���X�g���N�^
		ARGB(const DWORD &dw) { this->dw = dw; }
		ARGB(const BYTE a, const BYTE r, const BYTE g, const BYTE b) { this->argb.a = a; this->argb.r = r; this->argb.g = g; this->argb.b = b; }		
		ARGB(const double &a, const double &r, const double &g, const double &b); //�R���X�g���N�^
		ARGB(const double &a, const double &H, const double &S, const double &V, bool HSV); //�R���X�g���N�^�iHSV�l�Ŏw�肵��RGB�l�Ɏ����ϊ��j
		~ARGB() {} //�f�X�g���N�^

		union
		{
			DWORD dw;
			Byte argb;
		};

		void MulAlpha(const double &value); //�A���t�@�l�ɐώZ
		void MulRGB(const double &value); //RGB�l�ɐώZ

		//���Z�q
		ARGB operator *(const ARGB &value) const //�S�����ɐώZ
		{
			//BYTE�^��255*255=65535��255�ɕϊ����邽�߂ɁA255�̋t���ł���0.0039215686274509803921568627451��ώZ���Ă���
			return ARGB(BYTE(this->argb.a * value.argb.a * 0.0039215686274509803921568627451),
				BYTE(this->argb.r * value.argb.r * 0.0039215686274509803921568627451),
				BYTE(this->argb.g * value.argb.g * 0.0039215686274509803921568627451),
				BYTE(this->argb.b * value.argb.b * 0.0039215686274509803921568627451));
		}
		ARGB& operator *=(const double &value) //�S�����ɐώZ
		{
			this->argb.a = BYTE(this->argb.a * value);
			this->argb.r = BYTE(this->argb.r * value);
			this->argb.g = BYTE(this->argb.g * value);
			this->argb.b = BYTE(this->argb.b * value);
			return *this;
		}
		ARGB operator *(const double &value) const //�S�����ɐώZ
		{
			return ARGB(BYTE(this->argb.a * value), BYTE(this->argb.r * value), BYTE(this->argb.g * value), BYTE(this->argb.b * value));
		}

		ARGB &operator =(const DWORD &dw) { this->dw = dw; return *this; }
		operator DWORD() const { return dw; }
	};
}
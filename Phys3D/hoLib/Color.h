//色情報クラス
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
		ARGB() {} //コンストラクタ
		ARGB(const DWORD &dw) { this->dw = dw; }
		ARGB(const BYTE a, const BYTE r, const BYTE g, const BYTE b) { this->argb.a = a; this->argb.r = r; this->argb.g = g; this->argb.b = b; }		
		ARGB(const double &a, const double &r, const double &g, const double &b); //コンストラクタ
		ARGB(const double &a, const double &H, const double &S, const double &V, bool HSV); //コンストラクタ（HSV値で指定してRGB値に自動変換）
		~ARGB() {} //デストラクタ

		union
		{
			DWORD dw;
			Byte argb;
		};

		void MulAlpha(const double &value); //アルファ値に積算
		void MulRGB(const double &value); //RGB値に積算

		//演算子
		ARGB operator *(const ARGB &value) const //全成分に積算
		{
			//BYTE型の255*255=65535を255に変換するために、255の逆数である0.0039215686274509803921568627451を積算している
			return ARGB(BYTE(this->argb.a * value.argb.a * 0.0039215686274509803921568627451),
				BYTE(this->argb.r * value.argb.r * 0.0039215686274509803921568627451),
				BYTE(this->argb.g * value.argb.g * 0.0039215686274509803921568627451),
				BYTE(this->argb.b * value.argb.b * 0.0039215686274509803921568627451));
		}
		ARGB& operator *=(const double &value) //全成分に積算
		{
			this->argb.a = BYTE(this->argb.a * value);
			this->argb.r = BYTE(this->argb.r * value);
			this->argb.g = BYTE(this->argb.g * value);
			this->argb.b = BYTE(this->argb.b * value);
			return *this;
		}
		ARGB operator *(const double &value) const //全成分に積算
		{
			return ARGB(BYTE(this->argb.a * value), BYTE(this->argb.r * value), BYTE(this->argb.g * value), BYTE(this->argb.b * value));
		}

		ARGB &operator =(const DWORD &dw) { this->dw = dw; return *this; }
		operator DWORD() const { return dw; }
	};
}
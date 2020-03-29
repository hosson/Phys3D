#include "Color.h"
#include <math.h>
#include "LibError.h"
#include "Math.h"

namespace ho
{
	//コンストラクタ
	ARGB::ARGB(const double &a, const double &r, const double &g, const double &b)
	{
		this->argb.a = BYTE(a * 255.0);
		this->argb.r = BYTE(r * 255.0);
		this->argb.g = BYTE(g * 255.0);
		this->argb.b = BYTE(b * 255.0);
	}

	//コンストラクタ（HSVは各0.0～1.0で指定：HSV値で指定してRGB値に自動変換）
	ARGB::ARGB(const double &a, const double &H, const double &S, const double &V, bool HSV)
	{
		//0.0～1.0 を 0～255 に変換するラムダ式
		auto fByte = [](const double d)->BYTE { return BYTE(d * 255.0); };

		this->argb.a = fByte(a);

		if (S == 0.0)
		{
			this->argb.r = this->argb.g = this->argb.b = fByte(V);
		} else {
			const int H1 = ho::Clamp(int(fmod(H, 1.0) / (1.0 / 6.0)), 0, 5);

			const double f = H / (1.0 / 6.0) - H1;

			const double p = V * (1.0 - S);
			const double q = V * (1.0 - f * S);
			const double t = V * (1.0 - (1.0 - f) * S);

			switch (H1)
			{
			case 0:
				this->argb.r = fByte(V);
				this->argb.g = fByte(t);
				this->argb.b = fByte(p);
				break;
			case 1:
				this->argb.r = fByte(q);
				this->argb.g = fByte(V);
				this->argb.b = fByte(p);
				break;
			case 2:
				this->argb.r = fByte(p);
				this->argb.g = fByte(V);
				this->argb.b = fByte(t);
				break;
			case 3:
				this->argb.r = fByte(p);
				this->argb.g = fByte(q);
				this->argb.b = fByte(V);
				break;
			case 4:
				this->argb.r = fByte(t);
				this->argb.g = fByte(p);
				this->argb.b = fByte(V);
				break;
			case 5:
				this->argb.r = fByte(V);
				this->argb.g = fByte(p);
				this->argb.b = fByte(q);
				break;
			default:
				{
				}
				break;
			}
		}
	}

	//アルファ値に積算
	void ARGB::MulAlpha(const double &value)
	{
		this->argb.a = BYTE(this->argb.a * value);

		return;
	}

	//RGB値に積算
	void ARGB::MulRGB(const double &value)
	{
		this->argb.r = BYTE(this->argb.r * value);
		this->argb.g = BYTE(this->argb.g * value);
		this->argb.b = BYTE(this->argb.b * value);

		return;
	}
}
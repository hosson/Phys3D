//既存のクラスを継承したクラス

#pragma once
#include <d3dx9.h>
#include <d3d9types.h>
#include <Windows.h>
#include "Math.h"

namespace ho {
	class D3DCOLORVALUE : public ::D3DCOLORVALUE
	{
	public:
		D3DCOLORVALUE() {} //コンストラクタ
		D3DCOLORVALUE(float a, float r, float g, float b) { this->a = a; this->r = r; this->g = g; this->b = b; } //コンストラクタ
		D3DCOLORVALUE(DWORD argb)
		{
			a = ((argb & 0xff000000) >> 24) * 0.0039215686274509803921568627451f; // 255.0fで除算している
			r = ((argb & 0x00ff0000) >> 16) * 0.0039215686274509803921568627451f;
			g = ((argb & 0x0000ff00) >> 8) * 0.0039215686274509803921568627451f;
			b = (argb & 0x000000ff) * 0.0039215686274509803921568627451f;
		}
		D3DCOLORVALUE(const ::D3DCOLORVALUE &obj)
		{
			this->a = obj.a;
			this->r = obj.r;
			this->g = obj.g;
			this->b = obj.b;
		}

		D3DCOLORVALUE operator +(const D3DCOLORVALUE &obj)
		{
			return D3DCOLORVALUE(Clamp(this->a + obj.a, 0.0f, 1.0f),
				Clamp(this->r + obj.r, 0.0f, 1.0f),
				Clamp(this->g + obj.g, 0.0f, 1.0f),
				Clamp(this->b + obj.b, 0.0f, 1.0f));
		}

		D3DCOLORVALUE operator *(float value) { return D3DCOLORVALUE(a * value, r * value, g * value, b * value); }
		bool operator ==(const D3DCOLORVALUE &obj)
		{
			if (this->a != obj.a)
				return false;
			if (this->r != obj.r)
				return false;
			if (this->g != obj.g)
				return false;
			if (this->b != obj.b)
				return false;
			return true;
		}
		bool operator !=(const D3DCOLORVALUE &obj) { return (!(*this == obj)); }
	};
}

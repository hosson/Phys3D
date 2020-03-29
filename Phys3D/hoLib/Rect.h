//Rect系のクラス
#pragma once
#include <Windows.h>
#include "PT.h"
#include "Direction.h"

namespace ho {
	//RECT構造体を継承して関数などを追加したクラス
	class RECT : public ::RECT
	{
	public:
		RECT() {}
		RECT(int left, int top, int right, int bottom) { this->left = left; this->top = top; this->right = right; this->bottom = bottom; }

		e4Direction::e GetMinDirection() const; //値の一番小さい方向を返す
		bool JudgeContain(PT<int> pt) const; //ある座標が矩形の中に含まれるかどうかを判定する
		bool JudgeContain(int x, int y) const;
		int GetWidth() const { return right - left; } //幅を得る
		int GetHeight() const { return bottom - top; } //高さを得る
		void SetWidth(int Width) { this->right = this->left + Width; } //幅だけを設定
		void SetHeight(int Height) { this->bottom = this->top + Height; } //高さだけを設定
		void Set(int left, int top, int right, int bottom) { this->left = left; this->top = top; this->right = right; this->bottom = bottom; } //一括セット
	private:
		bool JudgeContainAlg(int x, int y) const; //ある座標が矩形の中に含まれるかどうかを判定する内部アルゴリズム
	};

	//座標と幅と高さで表すRECT
	class RectWH
	{
	public:
		int left, top, width, height;

		//コンストラクタ
		RectWH() { ZeroMemory(this, sizeof(RectWH)); }
		RectWH(int Left, int Top, int Width, int Height);
		RectWH(::RECT rect); //RECT構造体から変換

		//演算子
		RectWH operator +(const RectWH &obj) const { return RectWH(this->left + obj.left, this->top + obj.top, this->width + obj.width, this->height + obj.height); }
		bool operator ==(const RectWH &obj) const { return this->left == obj.left && this->top == obj.top && this->width == obj.width && this->height == obj.height; }
		bool operator !=(const RectWH &obj) const { return !(*this == obj); }

		::RECT GetWinRECT(int Offset = 0) const { ::RECT rect = {left, top, left + width + Offset, top + height + Offset}; return rect; } //RECT構造体で返す
		RECT GethoRECT(int Offset = 0) const { RECT rect(left, top, left + width + Offset, top + height + Offset); return rect; } //RECT構造体で返す
	};
}

//2次元座標クラス

#pragma once

namespace ho
{

	template <class T> class PT
	{
	public:
		PT() {}
		PT(T x, T y) { this->x = x; this->y = y; }

		T x, y; //座標

		PT GetAbs() { return PT(abs(this->x), abs(this->y)); } //絶対値に変換

		//演算子のオーバーロード
		PT operator +(const PT &obj) const { return PT(this->x + obj.x, this->y + obj.y); }
		PT &operator +=(const PT &obj) { this->x += obj.x; this->y += obj.y; return *this; }
		PT operator -(const PT &obj) const { return PT(this->x - obj.x, this->y - obj.y); }
		PT &operator -=(const PT &obj) { this->x -= obj.x; this->y -= obj.y; return *this; }
		PT operator *(const PT &value) const { return PT(this->x * value, this->y * value); }
		PT &operator *=(const PT &value) { this->x *= value; this->y *= value; return *this; }
		PT operator /(const PT &value) const { return PT(this->x / value, this->y / value); }
		PT &operator /=(const PT &value) { this->x /= value; this->y /= value; return *this; }

		bool operator ==(const PT obj) { return this->x == obj.x && this->y == obj.y; }
		bool operator !=(const PT obj) { return this->x != obj.x || this->y != obj.y; }
	};
}

/*POINT構造体の代わりにテンプレート化したクラス*/
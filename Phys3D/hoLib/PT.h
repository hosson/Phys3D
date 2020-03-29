//2�������W�N���X

#pragma once

namespace ho
{

	template <class T> class PT
	{
	public:
		PT() {}
		PT(T x, T y) { this->x = x; this->y = y; }

		T x, y; //���W

		PT GetAbs() { return PT(abs(this->x), abs(this->y)); } //��Βl�ɕϊ�

		//���Z�q�̃I�[�o�[���[�h
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

/*POINT�\���̂̑���Ƀe���v���[�g�������N���X*/
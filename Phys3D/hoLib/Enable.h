//任意の型に有効か無効かの情報をいっしょに持たせるクラス
#pragma once

namespace ho
{
	template <class T> class Enable
	{
	public:
		//コンストラクタ
		Enable() 
		{
			this->e = false;
		}
		Enable(const T &val)
		{
			this->e = true;
			this->value = val;
		}

		bool e; //Enable=true or Disable=false
		T value;

		const T &get() const { return this->value; } //参照渡しで値のみを得る
		T &getRef() { return this->value; } //参照渡しで得る
		void Disable() { this->e = false; } //無効化

		const bool get_e() const { return this->e; } //有効かどうかを得る
		bool operator !() const { return this->e; } //有効かどうか

		T operator ()() const { return this->value; } //値を得る
		Enable<T>& operator =(T val) { this->value = val; this->e = true; return *this; } //代入
	};
}

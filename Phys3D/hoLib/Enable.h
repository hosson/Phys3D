//�C�ӂ̌^�ɗL�����������̏�����������Ɏ�������N���X
#pragma once

namespace ho
{
	template <class T> class Enable
	{
	public:
		//�R���X�g���N�^
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

		const T &get() const { return this->value; } //�Q�Ɠn���Œl�݂̂𓾂�
		T &getRef() { return this->value; } //�Q�Ɠn���œ���
		void Disable() { this->e = false; } //������

		const bool get_e() const { return this->e; } //�L�����ǂ����𓾂�
		bool operator !() const { return this->e; } //�L�����ǂ���

		T operator ()() const { return this->value; } //�l�𓾂�
		Enable<T>& operator =(T val) { this->value = val; this->e = true; return *this; } //���
	};
}

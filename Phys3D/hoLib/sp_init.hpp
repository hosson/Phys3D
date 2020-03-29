//std::shared_ptr �̏��������x������N���X
#pragma once
#include <memory>

namespace ho
{
	//�X�}�[�g�|�C���^�������p�֐�
	template <typename U> std::shared_ptr<U> sp_init(U *p)
	{
		std::shared_ptr<U> sp = ((sp_base<U> *)p)->_get(); //���N���X�̃X�}�[�g�|�C���^���擾
		((sp_base<U> *)p)->_reset(); //���N���X�̃X�}�[�g�|�C���^�����

		return sp; //�X�}�[�g�|�C���^��Ԃ��ƁA�Q�ƃJ�E���g�� 1 �ɖ߂�
	}

	//���N���X
	template <typename U> class sp_base
	{
		friend std::shared_ptr<U> sp_init<U>(U *p); //_get() �� _reset() �� private �ɂ��邽�߂̃t�����h�w��
	public:
		sp_base() //���N���X�̃R���X�g���N�^����Ɏ��s�����
		{
			this->sp_this = std::shared_ptr<U>((U *)this);
		}
	private:
		std::shared_ptr<U> _get() { return sp_this; }
		void _reset() { sp_this.reset(); }
	protected:
		std::shared_ptr<U> sp_this;
	};
}


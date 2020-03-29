//�����N���X�g�����̃X�}�[�g�|�C���^
#pragma once

namespace ho
{
	class LinkListPtr_Base
	{
	public:
	protected:
		LinkListPtr_Base(LinkListPtr_Base *pPrev, LinkListPtr_Base *pNext) //�R���X�g���N�^
		{
			this->pPrev = pPrev;
			this->pNext = pNext;
		}

		//�����N���X�g����O��鏈��
		bool Remove()
		{
			if (!pPrev && !pNext) //�O�������ڑ�����Ă��Ȃ��ꍇ
				return true; //�I�u�W�F�N�g�͍폜�����

			if (pNext) //�����ڑ�����Ă���ꍇ
				pNext->pPrev = pPrev;
			if (pPrev) //�O���ڑ�����Ă���ꍇ
				pPrev->pNext = pNext;

			return false; //�I�u�W�F�N�g�͍폜����Ȃ�
		}

		//�����̃I�u�W�F�N�g�����X�g�̎������ɒǉ�����
		void Join(LinkListPtr_Base &obj) const
		{
			if (pNext) //���łɎ������ɑ��݂���ꍇ
			{
				obj.pPrev = (LinkListPtr_Base *)this;
				obj.pNext = pNext;
				pNext->pPrev = &obj;
				pNext = &obj;
			} else {
				obj.pPrev = (LinkListPtr_Base *)this;
				pNext = &obj;
			}

			return;
		}

		mutable LinkListPtr_Base *pPrev, *pNext; //�אڂ���I�u�W�F�N�g�ւ̃|�C���^
	};

	template <class T> class LinkListPtr : public LinkListPtr_Base
	{
	public:
		//�R���X�g���N�^
		LinkListPtr(T *p = NULL) : LinkListPtr_Base(NULL, NULL)
		{
			this->p = p;
		}

		//�R�s�[�R���X�g���N�^
		LinkListPtr(const LinkListPtr<T> &obj) : LinkListPtr_Base(NULL, NULL)
		{
			this->p = NULL;
			Delete(); //���X�g����O��A�Q�Ƃ���Ȃ��Ȃ����I�u�W�F�N�g��j������

			obj.Join(*this);
			this->p = obj.p;
		}

		//�f�X�g���N�^
		~LinkListPtr()
		{
			Delete(); //���X�g����O��A�Q�Ƃ���Ȃ��Ȃ����I�u�W�F�N�g��j������
		}

		//���X�g����O��A�Q�Ƃ���Ȃ��Ȃ����I�u�W�F�N�g��j������
		void Delete()
		{
			if (Remove()) //�����N���X�g����O��鏈�������A�I�u�W�F�N�g���폜�����ꍇ
			{
				if (p)
				{
					delete p;
					p = NULL;
				}
			}
		}

		//�I�u�W�F�N�g�ւ̃|�C���^���擾
		T Getp()
		{
			return p;
		}

		//���
		LinkListPtr<T> &operator =(const LinkListPtr<T> &obj) {
			if (&obj != this) //�����|�C���^���Q�Ƃ�����̂ł͂Ȃ��ꍇ
			{
				Delete(); //���X�g����O��A�Q�Ƃ���Ȃ��Ȃ����I�u�W�F�N�g��j������

				obj.Join(*this);
				this->p = obj.p;
			}
			return *this;
		}

		//�l�擾
		const T &operator *() const { return *p; }
		T &operator *() { return *p; }

		//�I�u�W�F�N�g�̃����o�A�N�Z�X�p
		const T *operator ->() const { return p; }
		T *operator ->() { return p; }

	private:
		T *p; //�I�u�W�F�N�g�ւ̃|�C���^
	};
}

/*�Œ���̓���͂��邪�A�܂��������ȋ@�\������B*/
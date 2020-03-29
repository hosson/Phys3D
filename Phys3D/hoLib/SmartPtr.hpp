//�X�}�[�g�|�C���^�N���X
//������

#pragma once
#include "Common.h"

#include "DetectMemoryLeak.h"

namespace ho
{
	template <class T> class SmartPtr
	{
	public:
		//�R���X�g���N�^
		SmartPtr(const T *pSrc = NULL) //�ʏ�
		{
			this->p = pSrc;
			this->pRefCount = NULL;
			if (p) //�|�C���^���n���ꂽ�ꍇ
				AddRef(); //�Q�ƃJ�E���^�𑝉�
		}
		SmartPtr(const SmartPtr<T> &src) //�R�s�[�R���X�g���N�^
		{
			this->p = src.p; //�|�C���^���R�s�[
			this->pRefCount = src.pRefCount; //�Q�ƃJ�E���^�ւ̃|�C���^���R�s�[

			AddRef(); //�Q�ƃJ�E���^�𑝉�
		}

		//�f�X�g���N�^
		~ComPtr()
		{
			SubRef(); //�Q�ƃJ�E���^������
		}

		//�Q�ƃJ�E���^�𑝉�
		void AddRef()
		{
			if (pRefCount) //�Q�ƃJ�E���^�����݂���ꍇ
				(*pRefCount)++;
			else
				pRefCount = new ULONG(1); //�Q�ƃJ�E���^���쐬

			return;
		}
		//�Q�ƃJ�E���^������
		void SubRef()
		{
			if (pRefCount) //�Q�ƃJ�E���^�����݂���ꍇ
			{
				(*pRefCount)--;
				if (*pRefCount == 0) //�Q�ƃJ�E���^��0�ɂȂ����ꍇ
				{
					SDELETE(pRefCount); //�Q�ƃJ�E���^���폜
					SDELETE(*p); //�I�u�W�F�N�g�̎��̂��폜
				}
			}

			return;
		}

		//�A�N�Z�b�T
		T Getp() { return p; } //�|�C���^���擾

		//���Z�q
		SmartPtr &operator =(const SmartPtr<T> &src) //������Z�q�̃I�[�o�[���[�h
		{
			if (this->p == src.p) //�������g�ւ̑���̏ꍇ
				return *this; //�������Ȃ�

			SubRef(); //�������g�̎Q�ƃJ�E���^������

			this->p = src.p; //�|�C���^���R�s�[
			this->pRefCount = src.pRefCount; //�Q�ƃJ�E���^�ւ̃|�C���^���R�s�[

			AddRef(); //������̎Q�ƃJ�E���^�𑝉�

			return *this;
		}

		T operator ->() //�I�u�W�F�N�g�̃����o�ւ̃A�N�Z�X�p
		{
			if (pRefCount) //�Q�ƃJ�E���^�����݂���i1�ȏ�́j�ꍇ
				return *p;
			//���G���[�i�G���[�������L�q�������������j
			return NULL; //COM�|�C���^��������̃C���^�[�t�F�[�X���w���Ă��Ȃ��ꍇ
		} 

		operator T() const //�C���^�[�t�F�[�X�ւ̃A�N�Z�X�p�̃L���X�g
		{
			if (pRefCount) //�Q�ƃJ�E���^�����݂���i1�ȏ�́j�ꍇ
				return *p;
			//���G���[�i�G���[�������L�q�������������j
			return NULL; //COM�|�C���^��������̃C���^�[�t�F�[�X���w���Ă��Ȃ��ꍇ
		}

		operator T*() const //�C���^�[�t�F�[�X�ւ̃_�u���|�C���^�擾�p�L���X�g
		{
			if (pRefCount) //�Q�ƃJ�E���^�����݂���i1�ȏ�́j�ꍇ
				return p;
			//���G���[�i�G���[�������L�q�������������j
			return NULL; //COM�|�C���^��������̃C���^�[�t�F�[�X���w���Ă��Ȃ��ꍇ
		} 
	private:
		T *p; //�I�u�W�F�N�g�ւ̃|�C���^
		ULONG *pRefCount; //�Q�ƃJ�E���^�ւ̃|�C���^
	};
}

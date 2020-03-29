//COM�|�C���^�N���X
#pragma once
#include "Common.h"
#include <Windows.h>

namespace ho
{
	template <class T> class ComPtr
	{
	public:
		//�R���X�g���N�^
		ComPtr(T *ppSrcInterface = NULL) //�ʏ�
		{
			this->ppInterface = ppSrcInterface;
			this->pRefCount = NULL;
			AddRef(); //�Q�ƃJ�E���^�𑝉�
		}
		ComPtr(const ComPtr<T> &src) //�R�s�[�R���X�g���N�^
		{
			this->ppInterface = src.ppInterface; //�C���^�[�t�F�[�X�ւ̃_�u���|�C���^���R�s�[
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
				pRefCount = new unsigned long(1); //�Q�ƃJ�E���^���쐬

			if (ppInterface)
				if (*ppInterface)
					(*ppInterface)->AddRef(); //�C���^�[�t�F�[�X���̎Q�ƃJ�E���^�𑝉�

			return;
		}
		//�Q�ƃJ�E���^������
		void SubRef()
		{
			if (pRefCount) //�Q�ƃJ�E���^�����݂���ꍇ
			{
				(*pRefCount)--;
				if (ppInterface)
					if (*ppInterface)
						(*ppInterface)->Release(); //�C���^�[�t�F�[�X�����

				if (*pRefCount == 0) //�Q�ƃJ�E���^��0�ɂȂ����ꍇ
				{
					SDELETE(pRefCount); //�Q�ƃJ�E���^���폜
					SDELETE(this->ppInterface); //�C���^�[�t�F�[�X�ւ̃_�u���|�C���^���폜
				}
			}


			return;
		}

		//Create�n�֐��p
		T *ToCreator()
		{
			SubRef(); //�Q�ƃJ�E���^������
			this->ppInterface = NULL; //�J�E���^���������̂ŃC���^�[�t�F�[�X�ւ̃_�u���|�C���^������

			this->ppInterface = new T; //�C���^�[�t�F�[�X�ւ̃|�C���^���쐬
			*this->ppInterface = NULL;
			this->pRefCount = NULL; //�Q�ƃJ�E���^��������
			AddRef(); //�Q�ƃJ�E���^�𑝉�

			return this->ppInterface; //�C���^�[�t�F�[�X�ւ̃_�u���|�C���^��n��
		}

		//�|�C���^�̒��g�����ւ���
		void Swap(ComPtr<T> &src)
		{
			//�o���̃|�C���^�̎Q�ƃJ�E���^�̒l���擾
			unsigned long SrcRef = src.GetRefCount();
			unsigned long MyRef = this->GetRefCount();

			//�o���̃|�C���^�̎Q�ƃJ�E���^�̍������v�Z
			BOOL SrcDecriment = FALSE; //�����̕����Q�Ɛ����������ǂ���
			unsigned long SubRef = MyRef - SrcRef;
			if (SrcRef > MyRef) //�����̕����Q�Ɛ��������ꍇ
			{
				SubRef = SrcRef - MyRef;
				SrcDecriment = TRUE;
			}

			//�Q�ƃJ�E���^�̑������ƌ����������߂�
			T pReleaseObj, pAddObj;
			if (SrcDecriment)
			{
				pReleaseObj = src.GetpInterface();
				pAddObj = *this->ppInterface;
			} else {
				pReleaseObj = *this->ppInterface;
				pAddObj = src.GetpInterface();
			}

			//�݂��̎Q�ƃJ�E���g����������悤�ɑ���
			unsigned long i;
			if (pReleaseObj && pAddObj) //�o�����L���ȃC���^�[�t�F�[�X�̏ꍇ
			{
				for (i = 0; i < SubRef; i++)
				{
					pReleaseObj->Release();
					pAddObj->AddRef();
				}
			} else if (pReleaseObj && !pAddObj) //�������������L���̏ꍇ
			{
				for (i = 0; i < SubRef; i++)
					pReleaseObj->Release();
			} else if (!pReleaseObj && pAddObj) //�������������L���̏ꍇ
			{
				for (i = 0; i < SubRef; i++)
					pAddObj->AddRef();
			}

			//COM�|�C���^���̃C���^�[�t�F�[�X�ւ̃|�C���^������
			T pTemp = src.GetpInterface();
			src.SetpInterface(this->GetpInterface());
			this->SetpInterface(pTemp);

			return;
		}


		//���Z�q
		ComPtr &operator =(const ComPtr<T> &src) //������Z�q�̃I�[�o�[���[�h
		{
			if (this->ppInterface == src.ppInterface) //�������g�ւ̑���̏ꍇ
				return *this; //�������Ȃ�

			SubRef(); //�������g�̎Q�ƃJ�E���^������

			this->ppInterface = src.ppInterface; //�C���^�[�t�F�[�X�ւ̃_�u���|�C���^���R�s�[
			this->pRefCount = src.pRefCount; //�Q�ƃJ�E���^�ւ̃|�C���^���R�s�[

			AddRef(); //������̎Q�ƃJ�E���^�𑝉�

			return *this;
		}
		ComPtr &operator =(T* ppSrcInterface) //�C���^�[�t�F�[�X�ւ̃_�u���|�C���^�̑��
		{
			if (this->ppInterface == ppSrcInterface) //�������g�ւ̑���̏ꍇ
				return *this; //�������Ȃ�

			SubRef(); //�������g�̎Q�ƃJ�E���^������

			this->pRefCount = NULL; //�Q�ƃJ�E���^��������
			this->ppInterface = ppSrcInterface; //�C���^�[�t�F�[�X�ւ̃_�u���|�C���^���R�s�[
			AddRef(); //�Q�ƃJ�E���^�𑝉�

			return *this;
		}

		//�C���^�[�t�F�[�X�ւ̃|�C���^���擾
		T GetpInterface() const
		{
			if (ppInterface)
				return *ppInterface;
			return NULL;
		} 

		T *GetppInterface() //�C���^�t�F�[�X�ւ̃_�u���|�C���^���擾
		{
			return ppInterface;
		}

		void SetpInterface(T p)
		{
			if (!ppInterface)
				ppInterface = new T;
			*ppInterface = p;
			return;
		}

		unsigned long *GetpRefCount() { return pRefCount; } //�Q�ƃJ�E���^�ւ̃|�C���^���擾
		unsigned long GetRefCount() //�Q�ƃJ�E���^�̒l���擾
		{
			if (pRefCount) //�Q�ƃJ�E���^�����݂���i1�ȏ�́j�ꍇ
				return *pRefCount;
			return 0; 
		}


		T operator ->() //�C���^�t�F�[�X�ւ̃A�N�Z�X�p
		{
			if (!ppInterface)
				return NULL;
			if (pRefCount) //�Q�ƃJ�E���^�����݂���i1�ȏ�́j�ꍇ
				return *ppInterface;
			return NULL; //COM�|�C���^��������̃C���^�[�t�F�[�X���w���Ă��Ȃ��ꍇ
		} 

		operator T() const //�C���^�[�t�F�[�X�ւ̃A�N�Z�X�p�̃L���X�g
		{
			if (!ppInterface)
				return NULL;
			if (pRefCount) //�Q�ƃJ�E���^�����݂���i1�ȏ�́j�ꍇ
				return *ppInterface;
			return NULL; //COM�|�C���^��������̃C���^�[�t�F�[�X���w���Ă��Ȃ��ꍇ
		}

		operator T*() const //�C���^�[�t�F�[�X�ւ̃_�u���|�C���^�擾�p�L���X�g
		{
			if (pRefCount) //�Q�ƃJ�E���^�����݂���i1�ȏ�́j�ꍇ
				return ppInterface;
			return NULL; //COM�|�C���^��������̃C���^�[�t�F�[�X���w���Ă��Ȃ��ꍇ
		} 
	private:
		T *ppInterface; //�C���^�[�t�F�[�X�ւ̃_�u���|�C���^
		unsigned long *pRefCount; //�Q�ƃJ�E���^�ւ̃|�C���^
	};
}

/*COM�p�̃X�}�[�g�|�C���^

���g������
Create�n�֐��֓n���Ƃ��́AToCreator() �֐����g���B
	ComPtr<LPDIRECT3DTEXTURE9> cp;
	CreateTexture����(�c, cp.ToCreator());
cp ���X�R�[�v�𔲂���ƃf�X�g���N�^���Ă΂�A�����I�Ƀe�N�X�`���{�̂���������B

���_���Ȏg����1��
�R���X�g���N�^�������Z�q���g����
ComPtr<LPDIRECT3DTEXTURE9> cp = new LPDIRECT3DTEXTURE9;
�ȂǂƂ���ƁA�C���^�[�t�F�[�X�̎��̂͂܂��쐬����Ă��Ȃ��ɂ�������炸
COM�|�C���^�����ŃC���^�[�t�F�[�X�� AddRef() ���Ăڂ��Ƃ��ăG���[���o��B

���_���Ȏg����2��
	LPDIRECT3DTEXTURE9 pTex;
	CreateTexture�����i�c, &pTex);
	ComPtr<LPDIRECT3DTEXTURE9> cp(pTex);
�ł� AddRef() �͐���ɌĂׂ邪�ApTex �� new �Ő������Ă��Ȃ����߁A
pTex ���X�R�[�v�𔲂��Ĕj�����ꂽ�u�Ԃ� ppInterface ���^���u�����O�|�C���^�ƂȂ��Ă��܂��B

���ʓ|�Ȏg����1��
	LPDIRECT3DTEXTURE9 *ppTex = new LPDIRECT3DTEXTURE9;
	*ppTex = NULL;
	ComPtr<LPDIRECT3DREXTURE9> cpTex = ppTex;
	CreateTexture����(�c, cpTex);
�R���X�g���N�^���Ă΂ꂽ���_�� *ppTex == NULL �A�܂�܂����̂����݂��Ȃ����Ƃ�
������Ă���Ηǂ��B���̂Ƃ� �Ō�� delete ppTex; �������Ă͂��Ă͂����Ȃ��B


���ʓ|�Ȏg����2��
ToCreator() ���g��Ȃ������ł�
	LPDIRECT3DTEXTURE9 *ppTex = new LPDIRECT3DTEXTURE9;
	CreateTexture����(�c, ppTex); //COM�����̎Q�ƃJ�E���^�� 0��1 �ɂȂ�
	ComPtr<LPDIRECT3DTEXTURE9> cpTex = ppTex; //COM�����̎Q�ƃJ�E���^�� 1��2 �ɂȂ�
	(*ppTex)->Release();  //COM�����̎Q�ƃJ�E���^�� 2��1 �ɂȂ�
	delete ppTex; //new �Ő��������̂Ŗ����I�ɉ��
�ƂȂ�Bdelete �� Release() �Ȃǂ��蓮�ŌĂ΂˂΂Ȃ�Ȃ��Ȃ�A�ʓ|�ł���B

*/
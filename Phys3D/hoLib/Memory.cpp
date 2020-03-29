#include "Memory.h"
#include <malloc.h>
#include "Common.h"
#include "LibError.h"
#include <Windows.h>
#include "tstring.h"

namespace ho
{
	//�R���X�g���N�^
	Allocator::Allocator()
	{
		//�������v�[���R���e�i���̃Z���T�C�Y�̍\�����w��
		std::vector<size_t> vectorCellSize(5);
		vectorCellSize.at(0) = 16;
		vectorCellSize.at(1) = 32;
		vectorCellSize.at(2) = 64;
		vectorCellSize.at(3) = 128;
		vectorCellSize.at(4) = 256;

		//�X���b�h�����̃������v�[���R���e�i���쐬
		pMemoryPoolContainerObj = new MemoryPoolContainer(this, vectorCellSize);

		InitializeCriticalSection(&cs);
#ifdef __DEBUG
		this->Count = 0;
#endif
	}

	//�f�X�g���N�^
	Allocator::~Allocator()
	{
		SDELETE(pMemoryPoolContainerObj);

		DeleteCriticalSection(&cs);
#ifdef __DEBUG
		if (Count)
		{
			ho::tstring str;
			ho::tPrint(str, TEXT("�������A���P�[�^�̊m�ۂƉ���̉񐔂���v���Ă��܂���B(Count = %d)"), Count);
			ER(str.c_str(), __WFILE__, __LINE__, true);
		}
#endif
	}

	//�������m��
	void *Allocator::Alloc(size_t Size)
	{
		EnterCriticalSection(&cs);
#ifdef __DEBUG
		if (vectorSizeCount.size() < Size)
			vectorSizeCount.resize(Size, 0);
		vectorSizeCount.at(Size - 1)++;
		Count++;
#endif

		//return malloc(Size);
		void *p = pMemoryPoolContainerObj->Alloc(Size);

		LeaveCriticalSection(&cs);

		return p; //�w�肳�ꂽ�X���b�h�ԍ��̃������v�[���R���e�i����m��
	}
	
	//���������
	void Allocator::Free(void *const p)
	{
		EnterCriticalSection(&cs);
#ifdef __DEBUG
		Count--;
#endif
		if (pMemoryPoolContainerObj->Free(p)) //��������������݂āA���������ꍇ
		{
			LeaveCriticalSection(&cs);
			return;
		}

		//����������A�h���X���������v�[�����Ɋ܂܂�Ȃ������ꍇ
		LeaveCriticalSection(&cs);
		free(p);

		return;
	}









	//�R���X�g���N�^
	MemoryPoolContainer::MemoryPoolContainer(Allocator *pAllocatorObj, const std::vector<size_t> &vectorMemoryPoolCellSize)
	{
		this->pAllocatorObj = pAllocatorObj;

		const DWORD MemoryCellNum = 1024000; //�������v�[�����̃Z���̗�

		//�������v�[���R���e�i�쐬
		vectorpMemoryPoolObj.resize(vectorMemoryPoolCellSize.size());
		for (DWORD i = 0; i < vectorMemoryPoolCellSize.size(); i++)
			vectorpMemoryPoolObj.at(i) = new MemoryPool(this, vectorMemoryPoolCellSize.at(i), MemoryCellNum);
	}

	//�f�X�g���N�^
	MemoryPoolContainer::~MemoryPoolContainer()
	{
		for (DWORD i = 0; i < vectorpMemoryPoolObj.size(); i++)
			SDELETE(vectorpMemoryPoolObj.at(i));
	}

	//�������m��
	void *MemoryPoolContainer::Alloc(size_t Size)
	{
		void *p;
		for (DWORD i = 0; i <vectorpMemoryPoolObj.size(); i++) //�������v�[���𑖍�
		{
			p = vectorpMemoryPoolObj.at(i)->Alloc(Size); //�������m�ۂ����݂�
			if (p) //�m�ۂł��Ă����ꍇ
				return p;
		}

		//�Y�����郁�����v�[����������Ȃ������ꍇ
		return malloc(Size);
	}

	//���������
	bool MemoryPoolContainer::Free(void *p)
	{
		for (DWORD i = 0; i < vectorpMemoryPoolObj.size(); i++)
			if (vectorpMemoryPoolObj.at(i)->Free(p)) //�������v�[���Ń�������������݂āA���������ꍇ
				return true;

		return false; //�Y�����郁�����v�[�������݂��Ȃ������ꍇ
	}













	//�R���X�g���N�^
	MemoryPool::MemoryPool(MemoryPoolContainer *pMemoryPoolContainerObj, size_t CellSize, unsigned int CellNum)
	{
		this->pMemoryPoolContainerObj = pMemoryPoolContainerObj;
		this->CellSize = CellSize;
		this->CellNum = CellNum;
		pMemory = malloc(CellSize * CellNum); //�������m��
		pMemoryEnd = (unsigned char *)pMemory + CellSize * CellNum; //�������v�[���I�[�̃A�h���X
		pUseMap = (unsigned char *)malloc(sizeof(unsigned char) * ((CellNum + 7) / 8)); //�g�p�}�b�v�m��
		pUseMapEnd = pUseMap + sizeof(unsigned char) * ((CellNum + 7) / 8); //�g�p�}�b�v�̏I�[�A�h���X
		pUseMapNow = pUseMap;
		ZeroMemory(pUseMap, sizeof(unsigned char) * ((CellNum + 7) / 8)); //�d�l�}�b�v��������
		UseMapBit = 0;
		UseNum = 0;
	}
	
	//�f�X�g���N�^
	MemoryPool::~MemoryPool()
	{
		free(pUseMap); 
		free(pMemory);
	}

	//�������m��
	void *MemoryPool::Alloc(size_t Size)
	{
		if (Size > CellSize) //�m�ۃT�C�Y���傫������ꍇ
			return NULL;

		if (UseNum >= (signed)CellNum) //���ɑS�ẴZ�����g�p����Ă���ꍇ
		{
			ho::tstring str;
			ho::tPrint(str, TEXT("%d Byte �̃������v�[�������܂����B"), this->CellSize);
			ER((TCHAR *)str.c_str(), __WFILE__, __LINE__, false);
		}

		//�������}�b�v����󂢂Ă���Z����T��
		while (*(pUseMapNow) & (0x1 << (UseMapBit % 8))) //�g�p�}�b�v���Q�Ƃ��Ďg�p�ς݂̏ꍇ�̓��[�v�p��
		{
			UseMapBit++; //���̃r�b�g�ֈڍs
			if (UseMapBit == 8) //���̃o�C�g�ֈڍs����ꍇ
			{
				UseMapBit = 0;
				pUseMapNow++;
				if (pUseMapNow == pUseMapEnd) //�g�p�}�b�v���I���ɓ��B�����ꍇ
				{
					pUseMapNow = pUseMap; //�擪�A�h���X�֖߂�
					//�������Ŏ��̃������v�[�����쐬�����肷��
				}
			}
		}

		//�������}�b�v���g�p�ς݂ɍX�V����
		*pUseMapNow = *pUseMapNow | (0x01 << UseMapBit);

		UseNum++;

		return (unsigned char *)pMemory + ((pUseMapNow - pUseMap) * 8 + UseMapBit) * CellSize; //�m�ۂ���A�h���X��Ԃ�
	}

	//���������
	bool MemoryPool::Free(void *p) 
	{
		if (p >= pMemory && p < pMemoryEnd) //�������A�h���X���������v�[�����̏ꍇ
		{
			int AddSub = ((unsigned char *)p - (unsigned char *)pMemory) / CellSize;
			//*(pUseMap + AddSub / 8) = *(pUseMap + AddSub / 8) | (0x1 << AddSub % 8); //������ꂽ�}�b�v�̃r�b�g�� or ���g����1�ɂ���
			*(pUseMap + AddSub / 8) = *(pUseMap + AddSub / 8) ^ (0x1 << AddSub % 8); //or ��1�ɂ����r�b�g�� xor �Ŕ��]����0�ɂ���

			UseNum--;
			return true; //��������� true ��Ԃ�
		}

		return false; //�����̃A�h���X���������v�[���Ɋ܂܂�Ȃ��ꍇ�� false ���Ԃ�
	}
}





//�z�u new
void* operator new(size_t size, ho::Allocator &Allocator)
{
	return Allocator.Alloc(size);
}

// �z�u delete
void operator delete(void *p, ho::Allocator &Allocator)
{
	Allocator.Free(p);

	return;
}

//new �����������郁�����v�[���ƃA���P�[�^
#pragma once
#include <vector>
#include <Windows.h>
#include "Version.h"
#include <process.h>

namespace ho
{
	class MemoryPool;
	class MemoryPoolContainer;

	class Allocator
	{
	public:
		Allocator(); //�R���X�g���N�^
		~Allocator(); //�f�X�g���N�^

		void *Alloc(size_t Size); //�������m��
		void Free(void *const p); //���������
		//void Free(void *const p); //���������

		//�A�N�Z�b�T
		MemoryPoolContainer *GetpMemoryPoolContainerObj() { return pMemoryPoolContainerObj; }
	private:
		MemoryPoolContainer *pMemoryPoolContainerObj; //�������v�[���R���e�i�ւ̃|�C���^
		CRITICAL_SECTION cs;
#ifdef __DEBUG
		std::vector<DWORD> vectorSizeCount; //�m�ۃ������T�C�Y���v
		int Count; //����Y��m�F�p�J�E���^
#endif
	};

	class MemoryPoolContainer
	{
	public:
		MemoryPoolContainer(Allocator *pAllocatorObj, const std::vector<size_t> &vectorMemoryPoolCellSize); //�R���X�g���N�^
		~MemoryPoolContainer(); //�f�X�g���N�^

		void *Alloc(size_t Size); //�������m��
		bool Free(void *p); //���������

		//�A�N�Z�b�T
		std::vector<MemoryPool *> &GetvectorpMemoryPoolObj() { return vectorpMemoryPoolObj; }
	private:
		Allocator *pAllocatorObj;
		std::vector<MemoryPool *> vectorpMemoryPoolObj; //�������v�[���ւ̃|�C���^�z��
	};


	class MemoryPool
	{
	public:
		MemoryPool(MemoryPoolContainer *pMemoryPoolContainerObj, size_t CellSize, unsigned int CellNum); //�R���X�g���N�^
		~MemoryPool(); //�f�X�g���N�^

		void *Alloc(size_t Size); //�������m��
		bool Free(void *p); //���������

		//�A�N�Z�b�T
		size_t GetCellSize() { return CellSize; }
		int GetUseNum() { return UseNum; }
	private:
		MemoryPoolContainer *pMemoryPoolContainerObj;
		size_t CellSize; //�Z���i��x��alloc�Ăяo���Ŋm�ۉ\�ȃ������j�̃T�C�Y
		unsigned int CellNum; //�Z���̗�
		unsigned char *pUseMap; //�������g�p��
		unsigned char *pUseMapEnd; //�������g�p�}�b�v�̏I�[�A�h���X
		unsigned char *pUseMapNow; //���ݎw���Ă���g�p�}�b�v�̃A�h���X
		int UseMapBit; //���ݎw���Ă���g�p�}�b�v�̃r�b�g�ʒu�i0�`7�j
		void *pMemory; //�v�[���ƂȂ郁�����̃|�C���^
		void *pMemoryEnd; //�������v�[���I�[�̃A�h���X
		int UseNum; //�g�p�ς݃Z����
	};
}

//�z�u new
void* operator new(size_t size, ho::Allocator &Allocator);

// �z�u delete
void operator delete(void *p, ho::Allocator &Allocator);

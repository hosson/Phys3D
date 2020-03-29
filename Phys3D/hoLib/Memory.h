//new を高速化するメモリプールとアロケータ
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
		Allocator(); //コンストラクタ
		~Allocator(); //デストラクタ

		void *Alloc(size_t Size); //メモリ確保
		void Free(void *const p); //メモリ解放
		//void Free(void *const p); //メモリ解放

		//アクセッサ
		MemoryPoolContainer *GetpMemoryPoolContainerObj() { return pMemoryPoolContainerObj; }
	private:
		MemoryPoolContainer *pMemoryPoolContainerObj; //メモリプールコンテナへのポインタ
		CRITICAL_SECTION cs;
#ifdef __DEBUG
		std::vector<DWORD> vectorSizeCount; //確保メモリサイズ統計
		int Count; //解放忘れ確認用カウンタ
#endif
	};

	class MemoryPoolContainer
	{
	public:
		MemoryPoolContainer(Allocator *pAllocatorObj, const std::vector<size_t> &vectorMemoryPoolCellSize); //コンストラクタ
		~MemoryPoolContainer(); //デストラクタ

		void *Alloc(size_t Size); //メモリ確保
		bool Free(void *p); //メモリ解放

		//アクセッサ
		std::vector<MemoryPool *> &GetvectorpMemoryPoolObj() { return vectorpMemoryPoolObj; }
	private:
		Allocator *pAllocatorObj;
		std::vector<MemoryPool *> vectorpMemoryPoolObj; //メモリプールへのポインタ配列
	};


	class MemoryPool
	{
	public:
		MemoryPool(MemoryPoolContainer *pMemoryPoolContainerObj, size_t CellSize, unsigned int CellNum); //コンストラクタ
		~MemoryPool(); //デストラクタ

		void *Alloc(size_t Size); //メモリ確保
		bool Free(void *p); //メモリ解放

		//アクセッサ
		size_t GetCellSize() { return CellSize; }
		int GetUseNum() { return UseNum; }
	private:
		MemoryPoolContainer *pMemoryPoolContainerObj;
		size_t CellSize; //セル（一度のalloc呼び出しで確保可能なメモリ）のサイズ
		unsigned int CellNum; //セルの量
		unsigned char *pUseMap; //メモリ使用状況
		unsigned char *pUseMapEnd; //メモリ使用マップの終端アドレス
		unsigned char *pUseMapNow; //現在指している使用マップのアドレス
		int UseMapBit; //現在指している使用マップのビット位置（0～7）
		void *pMemory; //プールとなるメモリのポインタ
		void *pMemoryEnd; //メモリプール終端のアドレス
		int UseNum; //使用済みセル量
	};
}

//配置 new
void* operator new(size_t size, ho::Allocator &Allocator);

// 配置 delete
void operator delete(void *p, ho::Allocator &Allocator);

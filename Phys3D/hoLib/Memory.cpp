#include "Memory.h"
#include <malloc.h>
#include "Common.h"
#include "LibError.h"
#include <Windows.h>
#include "tstring.h"

namespace ho
{
	//コンストラクタ
	Allocator::Allocator()
	{
		//メモリプールコンテナ内のセルサイズの構成を指定
		std::vector<size_t> vectorCellSize(5);
		vectorCellSize.at(0) = 16;
		vectorCellSize.at(1) = 32;
		vectorCellSize.at(2) = 64;
		vectorCellSize.at(3) = 128;
		vectorCellSize.at(4) = 256;

		//スレッド数分のメモリプールコンテナを作成
		pMemoryPoolContainerObj = new MemoryPoolContainer(this, vectorCellSize);

		InitializeCriticalSection(&cs);
#ifdef __DEBUG
		this->Count = 0;
#endif
	}

	//デストラクタ
	Allocator::~Allocator()
	{
		SDELETE(pMemoryPoolContainerObj);

		DeleteCriticalSection(&cs);
#ifdef __DEBUG
		if (Count)
		{
			ho::tstring str;
			ho::tPrint(str, TEXT("メモリアロケータの確保と解放の回数が一致していません。(Count = %d)"), Count);
			ER(str.c_str(), __WFILE__, __LINE__, true);
		}
#endif
	}

	//メモリ確保
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

		return p; //指定されたスレッド番号のメモリプールコンテナから確保
	}
	
	//メモリ解放
	void Allocator::Free(void *const p)
	{
		EnterCriticalSection(&cs);
#ifdef __DEBUG
		Count--;
#endif
		if (pMemoryPoolContainerObj->Free(p)) //メモリ解放を試みて、成功した場合
		{
			LeaveCriticalSection(&cs);
			return;
		}

		//解放したいアドレスがメモリプール内に含まれなかった場合
		LeaveCriticalSection(&cs);
		free(p);

		return;
	}









	//コンストラクタ
	MemoryPoolContainer::MemoryPoolContainer(Allocator *pAllocatorObj, const std::vector<size_t> &vectorMemoryPoolCellSize)
	{
		this->pAllocatorObj = pAllocatorObj;

		const DWORD MemoryCellNum = 1024000; //メモリプール内のセルの量

		//メモリプールコンテナ作成
		vectorpMemoryPoolObj.resize(vectorMemoryPoolCellSize.size());
		for (DWORD i = 0; i < vectorMemoryPoolCellSize.size(); i++)
			vectorpMemoryPoolObj.at(i) = new MemoryPool(this, vectorMemoryPoolCellSize.at(i), MemoryCellNum);
	}

	//デストラクタ
	MemoryPoolContainer::~MemoryPoolContainer()
	{
		for (DWORD i = 0; i < vectorpMemoryPoolObj.size(); i++)
			SDELETE(vectorpMemoryPoolObj.at(i));
	}

	//メモリ確保
	void *MemoryPoolContainer::Alloc(size_t Size)
	{
		void *p;
		for (DWORD i = 0; i <vectorpMemoryPoolObj.size(); i++) //メモリプールを走査
		{
			p = vectorpMemoryPoolObj.at(i)->Alloc(Size); //メモリ確保を試みる
			if (p) //確保できていた場合
				return p;
		}

		//該当するメモリプールが見つからなかった場合
		return malloc(Size);
	}

	//メモリ解放
	bool MemoryPoolContainer::Free(void *p)
	{
		for (DWORD i = 0; i < vectorpMemoryPoolObj.size(); i++)
			if (vectorpMemoryPoolObj.at(i)->Free(p)) //メモリプールでメモリ解放を試みて、成功した場合
				return true;

		return false; //該当するメモリプールが存在しなかった場合
	}













	//コンストラクタ
	MemoryPool::MemoryPool(MemoryPoolContainer *pMemoryPoolContainerObj, size_t CellSize, unsigned int CellNum)
	{
		this->pMemoryPoolContainerObj = pMemoryPoolContainerObj;
		this->CellSize = CellSize;
		this->CellNum = CellNum;
		pMemory = malloc(CellSize * CellNum); //メモリ確保
		pMemoryEnd = (unsigned char *)pMemory + CellSize * CellNum; //メモリプール終端のアドレス
		pUseMap = (unsigned char *)malloc(sizeof(unsigned char) * ((CellNum + 7) / 8)); //使用マップ確保
		pUseMapEnd = pUseMap + sizeof(unsigned char) * ((CellNum + 7) / 8); //使用マップの終端アドレス
		pUseMapNow = pUseMap;
		ZeroMemory(pUseMap, sizeof(unsigned char) * ((CellNum + 7) / 8)); //仕様マップを初期化
		UseMapBit = 0;
		UseNum = 0;
	}
	
	//デストラクタ
	MemoryPool::~MemoryPool()
	{
		free(pUseMap); 
		free(pMemory);
	}

	//メモリ確保
	void *MemoryPool::Alloc(size_t Size)
	{
		if (Size > CellSize) //確保サイズが大きすぎる場合
			return NULL;

		if (UseNum >= (signed)CellNum) //既に全てのセルが使用されている場合
		{
			ho::tstring str;
			ho::tPrint(str, TEXT("%d Byte のメモリプールが溢れました。"), this->CellSize);
			ER((TCHAR *)str.c_str(), __WFILE__, __LINE__, false);
		}

		//メモリマップから空いているセルを探す
		while (*(pUseMapNow) & (0x1 << (UseMapBit % 8))) //使用マップを参照して使用済みの場合はループ継続
		{
			UseMapBit++; //次のビットへ移行
			if (UseMapBit == 8) //次のバイトへ移行する場合
			{
				UseMapBit = 0;
				pUseMapNow++;
				if (pUseMapNow == pUseMapEnd) //使用マップが終わりに到達した場合
				{
					pUseMapNow = pUseMap; //先頭アドレスへ戻る
					//★ここで次のメモリプールを作成したりする
				}
			}
		}

		//メモリマップを使用済みに更新する
		*pUseMapNow = *pUseMapNow | (0x01 << UseMapBit);

		UseNum++;

		return (unsigned char *)pMemory + ((pUseMapNow - pUseMap) * 8 + UseMapBit) * CellSize; //確保するアドレスを返す
	}

	//メモリ解放
	bool MemoryPool::Free(void *p) 
	{
		if (p >= pMemory && p < pMemoryEnd) //解放するアドレスがメモリプール内の場合
		{
			int AddSub = ((unsigned char *)p - (unsigned char *)pMemory) / CellSize;
			//*(pUseMap + AddSub / 8) = *(pUseMap + AddSub / 8) | (0x1 << AddSub % 8); //解放されたマップのビットを or を使って1にする
			*(pUseMap + AddSub / 8) = *(pUseMap + AddSub / 8) ^ (0x1 << AddSub % 8); //or で1にしたビットを xor で反転して0にする

			UseNum--;
			return true; //解放されると true を返す
		}

		return false; //引数のアドレスがメモリプールに含まれない場合は false が返る
	}
}





//配置 new
void* operator new(size_t size, ho::Allocator &Allocator)
{
	return Allocator.Alloc(size);
}

// 配置 delete
void operator delete(void *p, ho::Allocator &Allocator)
{
	Allocator.Free(p);

	return;
}

#include "D3DResource.h"
#include "Debug.h"

namespace ho
{
	//コンストラクタ
	D3DResourceManager::D3DResourceManager()
	{
	}

	//デストラクタ
	D3DResourceManager::~D3DResourceManager()
	{
		Optimize();	//最適化（未使用リソースを削除）

		if (listpD3DResourceObj.size() > 0) //まだ解放されていないリソースがある場合
			ER(TEXT("まだ解放されていないリソースがあります。"), __WFILE__, __LINE__, true);
	}

	//リソースを登録する
	void D3DResourceManager::Regist(D3DResource *pD3DResourceObj) 
	{
		listpD3DResourceObj.push_back(pD3DResourceObj);

		return;
	}

	//最適化（未使用リソースを削除）
	void D3DResourceManager::Optimize()
	{
		for (std::list<D3DResource *>::iterator itr = listpD3DResourceObj.begin(); itr != listpD3DResourceObj.end();)
		{
			if ((*itr)->GetRefCount() == 1) //参照カウンタが1の場合（リソースクラスの中にあるCOMポインタだけがリソースを参照している状態の場合）
			{
				SDELETE(*itr); //リソースクラスを削除
				itr = listpD3DResourceObj.erase(itr);
			} else {
				itr++;
			}
		}

		return;
	}

	//D3DPOOL_DEFAULT フラグで作成されたリソースを解放
	void D3DResourceManager::OnLostDevice() 
	{
		for (std::list<D3DResource *>::iterator itr = listpD3DResourceObj.begin(); itr != listpD3DResourceObj.end(); itr++)
			(*itr)->OnLostDevice();

		return;
	}

	//D3DPOOL_DEFAULT フラグで作成されたリソースを再作成する
	void D3DResourceManager::OnResetDevice(LPDIRECT3DDEVICE9 pD3DDevice) 
	{
		for (std::list<D3DResource *>::iterator itr = listpD3DResourceObj.begin(); itr != listpD3DResourceObj.end(); itr++)
			(*itr)->OnResetDevice(pD3DDevice);

		return;
	}
}


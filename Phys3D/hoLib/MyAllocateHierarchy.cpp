#include "MyAllocateHierarchy.h"
#include <stdlib.h>
#include "Direct3D.h"

namespace ho {
	//コンストラクタ
	MyAllocateHierarchy::MyAllocateHierarchy(float AmbientRatio) 
	{
		pRootFrame = NULL;
		this->AmbientRatio = AmbientRatio;
	}

	//デストラクタ
	MyAllocateHierarchy::~MyAllocateHierarchy()
	{
		if (pRootFrame) //ルートフレームが設定されている場合
			DestroyFrame(pRootFrame); //フレームを削除
	}

	//フレームの新規作成
	HRESULT MyAllocateHierarchy::CreateFrame(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame)
	{
		//フレームを新しく生成する
		ho::D3DXFRAME_WORLD *pFrame = new ho::D3DXFRAME_WORLD;
		ZeroMemory(pFrame, sizeof(D3DXFRAME));
		pFrame->Name = new char[strlen(Name) + 1];
		strcpy_s(pFrame->Name, strlen(Name) + 1, Name);

		*ppNewFrame = pFrame;

		return D3D_OK;
	}

	//メッシュコンテナ作成
	HRESULT MyAllocateHierarchy::CreateMeshContainer(THIS_ LPCSTR Name, CONST D3DXMESHDATA *pMeshData, CONST D3DXMATERIAL *pMaterials, CONST D3DXEFFECTINSTANCE *pEffectInstances, DWORD NumMaterials, CONST DWORD *pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER *ppNewMeshContainer)
	{
		//メッシュコンテナオブジェクトを作成
		D3DXMESHCONTAINER *pMC = new D3DXMESHCONTAINER;
		ZeroMemory(pMC, sizeof(D3DXMESHCONTAINER));
	
		//名前のコピー
		pMC->Name = StrCopy(Name);

		//メッシュデータをコピー
		switch (pMeshData->Type) //メッシュのタイプによって分岐
		{
		case D3DXMESHTYPE_MESH: //通常メッシュ
			pMC->MeshData.pMesh = pMeshData->pMesh;
			pMC->MeshData.pMesh->AddRef();
			break;
		case D3DXMESHTYPE_PMESH: //プログレッシブメッシュ
			pMC->MeshData.pPMesh = pMeshData->pPMesh;
			pMC->MeshData.pPMesh->AddRef();
			break;
		case D3DXMESHTYPE_PATCHMESH: //パッチメッシュ
			pMC->MeshData.pPatchMesh = pMeshData->pPatchMesh;
			pMC->MeshData.pPatchMesh->AddRef();
			break;
		}
		pMC->MeshData.Type = pMeshData->Type; //タイプもコピー

		//マテリアルデータのコピー
		pMC->pMaterials = new D3DXMATERIAL[NumMaterials]; //マテリアルの数だけ配列を確保
		for (DWORD i = 0; i < NumMaterials; i++)
		{
			pMC->pMaterials[i].MatD3D = pMaterials[i].MatD3D;

			//アンビエント色成分をディフューズ成分と AmbientRatio から計算する
			pMC->pMaterials[i].MatD3D.Ambient = pMaterials[i].MatD3D.Diffuse;
			pMC->pMaterials[i].MatD3D.Ambient.r *= AmbientRatio;
			pMC->pMaterials[i].MatD3D.Ambient.g *= AmbientRatio;
			pMC->pMaterials[i].MatD3D.Ambient.b *= AmbientRatio;

			//((D3DXMATERIAL_TEXTURE *)pMC->pMaterials)[i].MatD3D.Power *= 4.0f; //スペキュラを拡散させる
			/*Power は Blender 上での Hardness に対応し、Blender 上の 511 が 1000.0f となる。
			値が大きいほど、スペキュラが一点に集中する。
			*/

			pMC->pMaterials[i].pTextureFilename = StrCopy(pMaterials[i].pTextureFilename);
		}
		//マテリアルの数
		pMC->NumMaterials = NumMaterials;

		//エフェクトデータ
		const D3DXEFFECTINSTANCE *pEI = pEffectInstances;
		pMC->pEffects = new D3DXEFFECTINSTANCE;
		ZeroMemory(pMC->pEffects, sizeof(D3DXEFFECTINSTANCE));
		if (pEI)
		{
			pMC->pEffects->pEffectFilename = StrCopy(pEI->pEffectFilename);
			pMC->pEffects->NumDefaults = pEI->NumDefaults;
			pMC->pEffects->pDefaults = new D3DXEFFECTDEFAULT[pEI->NumDefaults];

			D3DXEFFECTDEFAULT *pDIST = pEI->pDefaults; // コピー元
			D3DXEFFECTDEFAULT *pCOPY = pMC->pEffects->pDefaults; // コピー先
			for(DWORD i = 0; i < pEI->NumDefaults; i++)
			{
				pCOPY[i].pParamName = StrCopy(pDIST[i].pParamName);
				DWORD NumBytes = pCOPY[i].NumBytes = pDIST[i].NumBytes;
				pCOPY[i].Type = pDIST[i].Type;
			   if(pDIST[i].Type <= D3DXEDT_DWORD)
			   {
				  pCOPY[i].pValue = new DWORD[NumBytes];
				  memcpy(pCOPY[i].pValue, pDIST[i].pValue, NumBytes);
			   }
			}
		}

		//隣接ポリゴンインデックス
		DWORD NumPolygon = pMeshData->pMesh->GetNumFaces();
		pMC->pAdjacency = new DWORD[NumPolygon * 3];
		memcpy(pMC->pAdjacency, pAdjacency, NumPolygon * 3 * sizeof(DWORD));

		//スキン情報
		if (pSkinInfo)
		{	
			pMC->pSkinInfo = pSkinInfo;
			pMC->pSkinInfo->AddRef();
		}

		*ppNewMeshContainer = pMC; //メッシュコンテナ構造体のポインタを引数のポインタへ渡す

		return D3D_OK;
	}

	//フレーム削除
	HRESULT MyAllocateHierarchy::DestroyFrame(THIS_ LPD3DXFRAME pFrameToFree)
	{
		SDELETES(pFrameToFree->Name);

		if (pFrameToFree->pMeshContainer)
			DestroyMeshContainer(pFrameToFree->pMeshContainer);

		if (pFrameToFree->pFrameSibling)
			DestroyFrame(pFrameToFree->pFrameSibling);

		if (pFrameToFree->pFrameFirstChild)
			DestroyFrame(pFrameToFree->pFrameFirstChild);

		SDELETE(pFrameToFree);

		return D3D_OK;
	}

	//メッシュコンテナ削除
	HRESULT MyAllocateHierarchy::DestroyMeshContainer(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree)
	{
		SDELETE(pMeshContainerToFree->Name); //名前

		//メッシュ
		RELEASE(pMeshContainerToFree->MeshData.pMesh);

		//マテリアル
		for (DWORD i = 0; i < pMeshContainerToFree->NumMaterials; i++)
			SDELETES(pMeshContainerToFree->pMaterials[i].pTextureFilename);
		SDELETES(pMeshContainerToFree->pMaterials);

		//エフェクト
		for (DWORD i = 0; i < pMeshContainerToFree->pEffects->NumDefaults; i++)
		{
			SDELETES(pMeshContainerToFree->pEffects->pDefaults[i].pParamName);
			SDELETES(pMeshContainerToFree->pEffects->pDefaults[i].pValue);
		}
		SDELETES(pMeshContainerToFree->pEffects->pEffectFilename);
		SDELETES(pMeshContainerToFree->pEffects->pDefaults);
		SDELETE(pMeshContainerToFree->pEffects);

		//隣接ポリゴンインデックス
		SDELETE(pMeshContainerToFree->pAdjacency);
		RELEASE(pMeshContainerToFree->pSkinInfo);

		SDELETE(pMeshContainerToFree);

		return D3D_OK;
	}

	//文字列の領域確保とコピー
	LPSTR MyAllocateHierarchy::StrCopy(LPCSTR pSrc) 
	{
		if (!pSrc)
			return NULL;

		LPSTR str = new char[strlen(pSrc) + 1];
		strcpy_s(str, strlen(pSrc) + 1, pSrc);

		return str;
	}

	//初期状態の変換行列を保存（再帰関数）
	void MyAllocateHierarchy::SetInitTransMatrix(D3DXFRAME_WORLD *pFrame) 
	{
		pFrame->InitTransMatrix = pFrame->TransformationMatrix; //初期状態の変換行列を保存

		if (pFrame->pFrameFirstChild) //子供フレームが存在する場合
			SetInitTransMatrix((D3DXFRAME_WORLD *)pFrame->pFrameFirstChild); //再帰実行

		if (pFrame->pFrameSibling) //兄弟フレームが存在する場合
			SetInitTransMatrix((D3DXFRAME_WORLD *)pFrame->pFrameSibling); //再帰実行

		return;
	}

	//面の数を取得する再帰関数
	DWORD D3DXFRAME_WORLD::GetNumFacesReflect() 
	{
		DWORD Faces = 0;

		D3DXMESHCONTAINER *pMeshContainer = this->pMeshContainer;
		while (pMeshContainer)
		{
			Faces += pMeshContainer->MeshData.pMesh->GetNumFaces();
			pMeshContainer = pMeshContainer->pNextMeshContainer;
		}

		if (this->pFrameFirstChild) //子供フレームが存在する場合
			Faces += ((D3DXFRAME_WORLD *)this->pFrameFirstChild)->GetNumFacesReflect(); //再帰実行

		if (this->pFrameSibling) //兄弟フレームが存在する場合
			Faces += ((D3DXFRAME_WORLD *)this->pFrameSibling)->GetNumFacesReflect(); //再帰実行

		return Faces;
	}

	//名前からフレームを検索する再帰関数
	D3DXFRAME_WORLD *D3DXFRAME_WORLD::FindFrame(LPSTR str) 
	{
		if (!strcmp(this->Name, str)) //名前が一致した場合
			return this;

		if (this->pFrameFirstChild) //子供フレームが存在する場合
		{
			D3DXFRAME_WORLD *pFindFrame = ((D3DXFRAME_WORLD *)this->pFrameFirstChild)->FindFrame(str); //子供フレームを探す
			if (pFindFrame) //見つかっていた場合
				return pFindFrame;
		}

		if (this->pFrameSibling) //次の兄弟フレームが存在する場合
		{
			D3DXFRAME_WORLD *pFindFrame = ((D3DXFRAME_WORLD *)this->pFrameSibling)->FindFrame(str); //兄弟フレームを探す
			if (pFindFrame) //見つかっていた場合
				return pFindFrame;
		}

		return NULL;
	}
}
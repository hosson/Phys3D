#pragma once
#include <d3dx9.h>
#include "Common.h"
#include <d3d9.h>

namespace ho {
	//各フレームにワールド変換用の行列を追加したクラス
	class D3DXFRAME_WORLD : public D3DXFRAME
	{
	public:
		D3DXMATRIX WorldTransMatrix; //描画時のワールド変換用行列
		D3DXMATRIX InitTransMatrix; //初期状態の変換行列

		DWORD GetNumFacesReflect(); //面の数の合計を取得する再帰関数
		D3DXFRAME_WORLD *FindFrame(LPSTR str); //名前からフレームを検索する再帰関数
	};

	//マテリアル構造体にテクスチャへのポインタを追加したクラス
	/*class D3DXMATERIAL_TEXTURE : public D3DXMATERIAL
	{
	public:
		LPDIRECT3DTEXTURE9 pD3DTexture;
	};*/

	//ID3DXAllocateHierarchyクラスを継承したクラス
	class MyAllocateHierarchy : public ID3DXAllocateHierarchy
	{
	public:
		MyAllocateHierarchy(float AmbientRatio = 0.4f); //コンストラクタ 0.31f
		~MyAllocateHierarchy(); //デストラクタ
	
		STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame); //フレームの作成
		STDMETHOD(CreateMeshContainer)(THIS_ LPCSTR Name, CONST D3DXMESHDATA *pMeshData, CONST D3DXMATERIAL *pMaterials, CONST D3DXEFFECTINSTANCE *pEffectInstances, DWORD NumMaterials, CONST DWORD *pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER *ppNewMeshContainer); //メッシュコンテナ作成
		STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree); //フレームの削除
		STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree); //メッシュコンテナの削除

		void SetInitTransMatrix(D3DXFRAME_WORLD *pFrame); //初期状態の変換行列を保存（再帰関数）

		//アクセッサ
		D3DXFRAME_WORLD **GetppRootFrame() { return &pRootFrame; }
	protected:
	private:
		D3DXFRAME_WORLD *pRootFrame; //ルートフレームへのポインタ
		float AmbientRatio; //アンビエントカラーを決定するときの係数

		LPSTR StrCopy(LPCSTR pSrc); //文字列の領域確保とコピー
	};
}

/*Direct3Dでのアニメーション用にID3DXAllocateHierarchyクラスを継承したクラス。
アニメーション付きのxファイルを読み込む場合に、D3DXLoadMeshHierarchyFromX関数の
引数にこの MyAllocateHierarchyオブジェクトへのポインタを渡す。*/
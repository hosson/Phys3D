//メッシュを含むクラス

#pragma once
#include <d3dx9mesh.h>
#include <vector>
#include "D3DVertex.h"
#include <list>
#include <map>
#include "MyAllocateHierarchy.h"
#include "ComPtr.hpp"

namespace ho
{

	class Mesh;
	class D3DResourceManager;

	class MeshManager
	{
	public:
		MeshManager(); //コンストラクタ
		~MeshManager(); //デストラクタ

		DWORD AddMesh(Mesh *pMeshObj); //メッシュオブジェクトを登録
		void DeleteMesh(Mesh *pMeshObj); //メッシュオブジェクトを削除
		void DeleteMesh(LPD3DXMESH pMeshObj); //メッシュオブジェクトを削除

		LPD3DXMESH GetpMeshObj(DWORD Index) { vectorpMeshObj.at(Index); } //メッシュを得る
	private:
		std::vector<Mesh *> vectorpMeshObj; //メッシュオブジェクトへのポインタのリスト
		D3DResourceManager *pD3DResourceManagerObj; //リソース管理オブジェクトへのポインタ
	};

	class MeshBuffer;

	class Mesh
	{
	public:
		Mesh(LPD3DXMESH pD3DXMesh); //コンストラクタ（作成済みのメッシュを包む）
		Mesh(ho::Direct3D *pDirect3DObj, TCHAR *pFilename, D3DResourceManager *pD3DResourceManagerObj, double TextureSizeRatio = 1.0); //コンストラクタ（ファイルからロード）
		Mesh(ho::Direct3D *pDirect3DObj, TCHAR *pFilename, D3DResourceManager *pD3DResourceManagerObj, BOOL Animation); //コンストラクタ（ファイルからアニメーション付きでロード）
		Mesh(MeshBuffer *pMeshBufferObj, ho::Direct3D *pDirect3DObj, D3DResourceManager *pD3DResourceManagerObj); //コンストラクタ（メッシュバッファから作成）
		~Mesh(); //デストラクタ

		void Draw(); //描画
		DWORD GetNumFaces(); //面の数の合計を取得
		ho::D3DXFRAME_WORLD *Mesh::FindpFrame(LPSTR str); //フレーム名からフレームへのポインタを検索して取得する

		//アクセッサ
		LPDIRECT3DDEVICE9 GetpD3DDevice() { return pD3DDevice; }
		LPD3DXMESH GetpD3DXMesh() { return pD3DXMesh; }
		std::list<D3DMATERIAL9> *GetplistMaterial() { return &listMaterial; }
		float GetRadius() { return Radius; }
	private:
		LPDIRECT3DDEVICE9 pD3DDevice; //関連付けられたデバイス（ロード失敗などで無効のメッシュにはNULLが設定される）
		ho::Direct3D *pDirect3DObj;
		LPD3DXMESH pD3DXMesh; //メッシュへのポインタ
		std::list<D3DMATERIAL9> listMaterial; //マテリアルリスト
		std::list<ComPtr<LPDIRECT3DTEXTURE9>> listcpTexture; //テクスチャリスト
		DWORD Options; //作成時にGetOptionsで得られた値
		D3DResourceManager *pD3DResourceManagerObj; //Direct3D リソース管理オブジェクトへのポインタ
		MyAllocateHierarchy *pAH; //フレーム階層オブジェクトへのポインタ
		D3DXFRAME_WORLD *pFrame; //フレーム階層の先頭フレームへのポインタ
		ID3DXAnimationController *pAC; //アニメーションコントローラ
		float Radius; //半径

		BOOL CopyToBuffer(MeshBuffer *pMeshBufferObj); //作成したメッシュへメッシュバッファからデータをコピーする
		void DrawNormal(); //標準メッシュの描画
		void DrawAnimation(); //アニメーション付きメッシュの描画
		DWORD GetNumFacesReflect(D3DXFRAME_WORLD *pFrame); //面の数を取得する再帰関数
		void CalcRadius(); //半径（原点から最も遠い頂点までの距離）を計算する
	};

	class MeshBuffer
	{
	public:
		struct SUBSET //1つのマテリアルとテクスチャを持った頂点情報のサブセット
		{
			std::vector<D3DVERTEX> vectorVt; //頂点情報配列
			std::vector<WORD> vectorIndex; //頂点インデックス配列
			D3DMATERIAL9 Material; //マテリアル
			ComPtr<LPDIRECT3DTEXTURE9> cpTexture; //テクスチャ
		};
	public:
		BOOL CreateSubset(int Index, SUBSET *pSubset); //サブセットを追加
		BOOL AddVertex(int SubsetIndex, const D3DVERTEX *pVt, int Vertices, const WORD *pIndex, int Indices); //サブセットに頂点情報と頂点インデックスを追加
		DWORD GetVerticesNum(); //頂点数の合計を得る
		DWORD GetIndicesNum(); //頂点インデックス数の合計を得る

		//アクセッサ
		std::map<int, SUBSET> *GetpmapSubset() { return &mapSubset; }
	private:
		std::map<int, SUBSET> mapSubset;
	};

	//ID3DXMeshの頂点情報から端の位置を取得
	D3DXVECTOR3 GetD3DXMeshCornerPos(LPD3DXMESH pMesh, int Axis, bool Max, bool World);
	bool D3DXVector3SortX1(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2);
	bool D3DXVector3SortX2(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2);
	bool D3DXVector3SortY1(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2);
	bool D3DXVector3SortY2(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2);
	bool D3DXVector3SortZ1(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2);
	bool D3DXVector3SortZ2(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2);
}

/*LPD3DXMESH をラッピングするMeshクラスとそれを管理する
MeshManagerクラスで構成される。

＜使い方＞
・プログラム内に一つだけMeshManagerクラスを作る。
・LPD3DXMESHを作成したらMeshクラスで包みそのままMeshManagerに登録する。
・メッシュの削除時は、MeshManager::DeleteMeshを使うだけで削除処理が完了する。
・動的に頂点情報を追加したい場合は、MeshBufferを作りMeshBuffer::Addで頂点を追加したのち、
　Meshのコンストラクタでそのポインタを渡す。
・コンストラクタに引数にテクスチャマネージャを渡すと、メッシュ削除時に付随するテクスチャも
　自動で削除される。引数をNULLにすると自動では削除されない。

 ＜MeshクラスのRadiusについて＞
 ・視錐台カリング等の判定に使えるが、モデルロード時のみ計算されるので、アニメーションするときは
 正確な半径はアニメーション中の頂点座標からの再計算が必要になる。
*/

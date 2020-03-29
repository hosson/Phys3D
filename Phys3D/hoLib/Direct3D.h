//DirectX Graphicsのクラス

#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9tex.h>
#include <dxerr9.h>

#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "dxerr9.lib")

#include "Common.h"
#include "PT.h"
#include "ComPtr.hpp"
#include "tstring.h"

namespace ho {
	//前方宣言
	class D3DVERTEX;
	class D3DVERTEX_2D;
	class GetCharTexture;
	class DrawString;
	class D3DResourceManager;
	class MeshManager;
	class DrawDebug;

	class Direct3D
	{
	public:
		struct VIEWPORTCULLING //視錐台カリング計算用構造体
		{
			D3DXVECTOR3 vecNear[4], vecFar[4]; //視錐台の8点の座標
			D3DXVECTOR3 vecNFar, vecNNear, vecNLeft, vecNRight, vecNTop, vecNBottom; //法線ベクトル
		};
	public:
		Direct3D(HWND hWnd, LOGFONT &lf); //コンストラクタ
		virtual ~Direct3D(); //デストラクタ
		bool Init(HWND hWnd, LOGFONT &lf); //初期化
		bool CreateObj1(); //オブジェクト作成1（デバイス作成前）
		bool CreateObj2(LOGFONT lf); //オブジェクト作成2（デバイス作成後）
		bool InitD3D(HWND hWnd); //Direct3Dの初期化
		bool InitRender(); //レンダーの初期化

		bool BeginDraw(); //描画開始（呼び出し必須）
		void Draw(); //フレーム毎の描画
		bool EndDraw(); //描画終了（呼び出し必須）
		void OnLostDevice(); //ロスト状態のデバイス上で行う処理（リソース解放）
		bool DeviceReset(); //ロスト状態のデバイスをリセット

		void SetRenderTargetBackBuffer(); //レンダーターゲットをバックバッファに設定する

		virtual D3DXMATRIX SetWorldMatrix(D3DXMATRIX *pmtrxWorld); //ワールド行列を設定する
		virtual D3DMATERIAL9 SetMaterial(D3DMATERIAL9 *pMaterial); //マテリアルを設定する


		DWORD GetUsableLightIndexNum(); //SetLight()で使える（無効になっている）最小の番号を取得する
		void GetBillBoardMatrix(D3DXMATRIX &out, D3DXMATRIX &mtrxTransScale, const D3DXMATRIX *pmtrxView = NULL); //ビュー行列からビルボード描画用のワールド行列を取得
		::D3DXVECTOR3 GetScreenToWorld(::D3DXVECTOR3 &vecScreen, ::D3DXMATRIX *pmtrxView = NULL); //任意のビュー行列のスクリーン座標からワールド座標を得る
		::D3DXVECTOR3 GetWorldToScreen(::D3DXVECTOR3 &vecWorld, ::D3DXMATRIX *pmtrxView = NULL); //ワールド座標からスクリーン座標を得る

		virtual PT<int> GetptClientSize() { return PT<int>(GetClientWidth(), GetClientHeight()); }
		virtual int GetClientWidth() const { return 640; } //クライアント領域の幅を得る
		virtual int GetClientHeight() const { return 480; } //クライアント領域の高さを得る

		static void GetErrorStr(tstring &str, HRESULT hr); //HRESULT値からエラー文字列を取得する

		//アクセッサ
		LPDIRECT3DDEVICE9 GetpD3DDevice();
		DrawString *GetpDrawStringObj() { return pDrawStringObj; }
		D3DResourceManager *GetpD3DResourceManagerObj() { return pD3DResourceManagerObj; }
		MeshManager *GetpMeshManagerObj() { return pMeshManagerObj; }
		bool GetDeviceLost() { return DeviceLost; }
		DrawDebug *GetpDrawDebugObj() { return pDrawDebugObj; }
	protected:
		LPDIRECT3D9 pD3D;
		LPDIRECT3DDEVICE9 pD3DDevice;
		DrawString *pDrawStringObj; //文字描画オブジェクトへのポインタ
		D3DResourceManager *pD3DResourceManagerObj; //Direct3D リソース管理オブジェクトへのポインタ
		MeshManager *pMeshManagerObj; //メッシュ管理オブジェクトへのポインタ
		bool DeviceLost; //デバイスロスト状態かどうか
		D3DPRESENT_PARAMETERS d3dpp; //デバイス作成時のパラメータ
		DrawDebug *pDrawDebugObj; //デバッグ描画オブジェクトへのポインタ

		virtual bool GetWindowed() { return true; } //ウィンドウモードかどうか
		virtual bool GetVSYNC() const { return true; } //VSYNC同期をするかどうか
	};

}

/*
毎フレームの描画の最初に DrawBegin() を、最後に DrawEnd() を必ず呼び出す。
InitRender() や Draw() は本体側で適切なものを作って使う。
*/

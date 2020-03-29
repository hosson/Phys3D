#include "Direct3D.h"
#include "Debug.h"
#include <tchar.h>
#include "CharTexture.h"
#include "DrawString.h"
#include "Mesh.h"
#include "Math.h"
#include "D3DResource.h"
#include "DrawDebug.h"
#include "StringTextureCache.h"
#include "LibError.h"

namespace ho
{
	Direct3D::Direct3D(HWND hWnd, LOGFONT &lf) //コンストラクタ
	{
		this->pD3D = NULL;
		this->pD3DDevice = NULL;
		this->pD3DResourceManagerObj = NULL;
		this->pMeshManagerObj = NULL;
		this->pDrawStringObj = NULL;
		this->pDrawDebugObj = NULL;

		this->DeviceLost = false;

		if (hWnd) //継承する場合は hWnd に NULL を渡すようにし、継承先のコンストラクタからこの初期化を行う
			Init(hWnd, lf);
	}

	Direct3D::~Direct3D() //デストラクタ
	{
		SDELETE(pDrawDebugObj);
		SDELETE(pDrawStringObj);
		SDELETE(pMeshManagerObj);
		SDELETE(pD3DResourceManagerObj);
		RELEASE(pD3DDevice);
		RELEASE(pD3D);
	}

	//初期化
	bool Direct3D::Init(HWND hWnd, LOGFONT &lf)
	{
		CreateObj1(); //オブジェクト作成1
		if (InitD3D(hWnd)) //Direct3Dの初期化
		{
			InitRender(); //レンダーを初期化
			CreateObj2(lf); //オブジェクト作成2
		} else {
			return false;
		}

		return true;
	}

	//オブジェクト作成1（デバイス作成前）
	bool Direct3D::CreateObj1() 
	{
		pD3DResourceManagerObj = new D3DResourceManager(); //Direct3D リソース管理オブジェクトを作成
		pMeshManagerObj = new MeshManager(); //メッシュ管理オブジェクトを作成

		return TRUE;
	}

	//オブジェクト作成2（デバイス作成後）
	bool Direct3D::CreateObj2(LOGFONT lf) 
	{
		pDrawStringObj = new DrawString(this, pD3DDevice, &lf, false); //文字列描画オブジェクトを作成
		pDrawDebugObj = new ho::DrawDebug(this); //デバッグ描画オブジェクトを作成

		return TRUE;
	}

	bool Direct3D::InitD3D(HWND hWnd) //Direct3Dの初期化
	{
		HRESULT hr;

		try
		{
			//描画対象のウィンドウハンドル取得
			if (!hWnd)
				throw Exception(TEXT("NULL のウィンドウハンドルが渡されました。"), __WFILE__, __LINE__, false);

			//Direct3Dオブジェクトの作成
			if (!(pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
				throw Exception(TEXT("Direct3D オブジェクトの作成に失敗しました。"), __WFILE__, __LINE__, false);

			ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

			//現在のディスプレイモードの取得
			D3DDISPLAYMODE mode;
			if (FAILED(hr = pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode)))
			{
				tstring str;
				GetErrorStr(str, hr);
				str = TEXT("ディスプレイモードの取得に失敗しました。") + str;
				throw Exception(str, __WFILE__, __LINE__, false);
			}
			d3dpp.BackBufferFormat = mode.Format;

			//デバイス作成のためのパラメータを設定
			d3dpp.BackBufferWidth = GetClientWidth();							//画面幅
			d3dpp.BackBufferHeight = GetClientHeight();							//画面高さ
			//d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;							//バックバッファの形式
			d3dpp.BackBufferCount = 1;											//バックバッファの数
			d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;							//表示方法
			d3dpp.EnableAutoDepthStencil = TRUE;								//Zバッファ有無
			d3dpp.AutoDepthStencilFormat = D3DFMT_D16;							//Zバッファのフォーマット
			d3dpp.hDeviceWindow = hWnd;											//描画対象のウィンドウハンドル
			d3dpp.Windowed = GetWindowed();										//ウィンドウモード
			d3dpp.Flags = NULL;														
			if (GetVSYNC())
				d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;			//VSYNC同期する
			else
				d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;			//VSYNC同期しない

			//デバイスの作成
			if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &pD3DDevice)))
				if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pD3DDevice)))
					if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &pD3DDevice)))
						if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pD3DDevice)))
							throw Exception(TEXT("デバイスの作成に失敗しました。"), __WFILE__, __LINE__, false);
		}
		catch(Exception &e)
		{
			ER(e.strMsg.c_str(), e.pFile, e.Line, e.Continue);
			return false;
		}

		return true;
	}

	//レンダーの各種設定を行う。ここはソフトによって内容が入れ替わるのでこれは一例。
	bool Direct3D::InitRender()
	{
		//アルファブレンディング
		pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
		pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW); //カリング
		pD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE); //Zバッファリング有効

		//テクスチャフィルタの設定
		pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		pD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		return true;
		/*
		pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1); //頂点フォーマットを指定


		//透視変換の設定
		D3DXMATRIX matProj;
		D3DXMatrixPerspectiveFovLH(&matProj, 0.8f , float(GetClientWidth()) / float(GetClientHeight()), 2.0f, 800.0f);
		pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

		//View 座標の設定
		D3DXMATRIX matView;
		D3DXMatrixLookAtLH(&matView, &D3DXVECTOR3(2.8f, 79.3f, -54.0f), &D3DXVECTOR3(-1.0f, -4.0f, 0.0f), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
		pD3DDevice->SetTransform(D3DTS_VIEW, &matView);




		//テクスチャフィルタの設定
		pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		pD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	/*	pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		pD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		

		//マテリアル設定
		D3DMATERIAL9 mat;
		ZeroMemory(&mat, sizeof(D3DMATERIAL9));
		mat.Diffuse.r = mat.Diffuse.g = mat.Diffuse.b = mat.Diffuse.a = 1.0f; //Diffuse
		mat.Ambient.r = mat.Ambient.g =  mat.Ambient.b =  mat.Ambient.a = 0.1f; //Ambient
		mat.Specular.r =  mat.Specular.g = mat.Specular.b = mat.Specular.a = 1.0f; //Specular
		mat.Power = 10.0f; //Specular power
		mat.Emissive.r = mat.Emissive.g = mat.Emissive.b = 	mat.Emissive.a = 0.0f; //Emissive
		SetMaterial(&mat);
	
		//ライト設定
		D3DLIGHT9 light;
		ZeroMemory(&light, sizeof(D3DLIGHT9));

		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Diffuse.r = light.Diffuse.g = light.Diffuse.b = light.Diffuse.a = 1.0f;
		light.Specular.r = light.Specular.g = light.Specular.b = light.Specular.a = 0.7f;
		light.Ambient.r = light.Ambient.g = light.Ambient.b = light.Ambient.a = 0.0f;
		light.Position = D3DXVECTOR3(0.0f, 50.0f, -10.0f); //光の位置
		light.Direction = D3DXVECTOR3(0, -1.0f, 0); //光の方向
		light.Range = 200.0f; //光の届く距離
		light.Attenuation0 = 0.3f; 
		light.Attenuation1 = 0.0002f; //距離に比例した減衰
		light.Attenuation2 = 0.1f; //距離2乗に比例した減衰

		pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
		pD3DDevice->SetLight(0, &light);
		pD3DDevice->LightEnable(0, TRUE);

		pD3DDevice->SetRenderState(D3DRS_AMBIENT, 0xffffffff); //全体の環境光
		*/
		return true;
	}

	//描画開始（呼び出し必須）
	bool Direct3D::BeginDraw()
	{
		if (DeviceLost) //デバイスロスト状態の場合
			return DeviceReset(); //デバイスリセットを試みる

		return true;
	}

	//フレーム毎の描画（一例）
	void Direct3D::Draw() 
	{
		if (!pD3DDevice)
			return;

		pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0xff, 0xff, 0xff), 1.0f, 0); //バックバッファのクリア

		pD3DDevice->BeginScene(); //描画開始

		/*ここにDrawPrimitiveUpなどの描画メソッドを記述*/

		pD3DDevice->EndScene(); //描画終了

		return;
	}

	//描画終了（呼び出し必須）
	bool Direct3D::EndDraw() 
	{
		if (!pD3DDevice)
			return true;

		if (pD3DDevice->Present(NULL, NULL, NULL, NULL) == D3DERR_DEVICELOST) //デバイスロストの場合
		{
			OnLostDevice(); //リソース解放など
			return false;
		}

		pDrawStringObj->FrameProcess(); //文字列キャッシュの最適化など
		pD3DResourceManagerObj->Optimize(); //リソース管理クラスの最適化

		return true;
	}

	//ワールド行列を設定する
	D3DXMATRIX Direct3D::SetWorldMatrix(D3DXMATRIX *pmtrxWorld)
	{
		D3DXMATRIX mtrxReturn;
		pD3DDevice->GetTransform(D3DTS_WORLD, &mtrxReturn);

		pD3DDevice->SetTransform(D3DTS_WORLD, pmtrxWorld);

		return mtrxReturn; //デバイスに設定されていた古い行列が返る
	}

	//マテリアルを設定する
	D3DMATERIAL9 Direct3D::SetMaterial(D3DMATERIAL9 *pMaterial)
	{
		D3DMATERIAL9 OldMaterial;
		pD3DDevice->GetMaterial(&OldMaterial);
		pD3DDevice->SetMaterial(pMaterial);

		return OldMaterial;
	}

	//LPDIRECT3DDEVICE9を得る
	LPDIRECT3DDEVICE9 Direct3D::GetpD3DDevice() 
	{
		if (!pD3DDevice)
		{
			ER(TEXT("Direct3Dデバイスの取得が出来ていません。"), __WFILE__, __LINE__, false);
			return NULL;
		}

		return pD3DDevice;
	}

	//SetLight()で使える（無効になっている）最小の番号を取得する
	DWORD Direct3D::GetUsableLightIndexNum() 
	{
		const DWORD Max = 255; //検索する最大値
		BOOL Enable;

		for (DWORD i = 0; i < Max; i++)
		{
			pD3DDevice->GetLightEnable(i, &Enable); //取得
			if (!Enable) //無効だった場合
				return i;
		}

		return 0; //見つからなかった場合はとりあえず0を返す
	}

	//現在のビュー行列からビルボード描画用のワールド行列を取得
	void Direct3D::GetBillBoardMatrix(D3DXMATRIX &out, D3DXMATRIX &mtrxTransScale, const D3DXMATRIX *pmtrxView) 
	{
		D3DXMATRIX mtrxView;

		if (!pmtrxView) //ビュー行列が渡されていない場合
			pD3DDevice->GetTransform(D3DTS_VIEW, &mtrxView); //デバイスから現在のビュー行列を取得
		else
			mtrxView = *pmtrxView;

		D3DXMatrixInverse(&mtrxView, NULL, &mtrxView); //ビュー行列を逆行列に変換

		//ビューマトリックスの逆行列は、オブジェクトの向き(回転)だけを考慮するので平行移動は無効にする。
		mtrxView._41 = 0.0f;
		mtrxView._42 = 0.0f;
		mtrxView._43 = 0.0f;
   
		D3DXMATRIX mtrxScale, mtrxTrans; //拡大縮小行列、平行移動行列

		//スケーリングマトリックスを抽出する
		D3DXMatrixIdentity(&mtrxScale);
		mtrxScale._11 = mtrxTransScale._11;
		mtrxScale._22 = mtrxTransScale._22;
		mtrxScale._33 = mtrxTransScale._33;

		//平行移動マトリックスを抽出する
		D3DXMatrixIdentity(&mtrxTrans);
		mtrxTrans._41 = mtrxTransScale._41;
		mtrxTrans._42 = mtrxTransScale._42;
		mtrxTrans._43 = mtrxTransScale._43;

		//スケーリングして、回転して、平行移動する。
		out = mtrxScale * mtrxView * mtrxTrans;

		return;
	}

	//任意のビュー行列のスクリーン座標からワールド座標を得る
	::D3DXVECTOR3 Direct3D::GetScreenToWorld(::D3DXVECTOR3 &vecScreen, ::D3DXMATRIX *pmtrxView) 
	{
		D3DXMATRIX mtrxView, mtrxProj, mtrxViewPort; //投射行列、ビューポート行列

		if (!pmtrxView) //ビュー行列が渡されていない場合
			pD3DDevice->GetTransform(D3DTS_VIEW, &mtrxView); //デバイスから現在のビュー行列を取得
		else
			mtrxView = *pmtrxView;

		//各行列を取得
		pD3DDevice->GetTransform(D3DTS_PROJECTION, &mtrxProj); //投射行列取得
		D3DVIEWPORT9 vp;
		pD3DDevice->GetViewport(&vp); //ビューポート取得
		D3DXMatrixIdentity(&mtrxViewPort);
		mtrxViewPort._11 = vp.Width * 0.5f;
		mtrxViewPort._22 = vp.Height * -0.5f;
		mtrxViewPort._41 = vp.Width * 0.5f;
		mtrxViewPort._42 = vp.Height * 0.5f;
		mtrxViewPort._33 = vp.MaxZ - vp.MinZ;
		mtrxViewPort._43 = vp.MinZ;

		//各行列を逆行列に変換
		D3DXMATRIX mtrxViewInv; //ビュー行列の逆行列
		D3DXMatrixInverse(&mtrxViewInv, NULL, &mtrxView);
		D3DXMatrixInverse(&mtrxProj, NULL, &mtrxProj);
		D3DXMatrixInverse(&mtrxViewPort, NULL, &mtrxViewPort);

		//変換
		D3DXVECTOR3 vecReturn;
		D3DXMATRIX mtrx = mtrxViewPort * mtrxProj * mtrxViewInv;
		D3DXVec3TransformCoord(&vecReturn, &vecScreen, &mtrx);

		return vecReturn;
	}

	//ワールド座標からスクリーン座標を得る
	::D3DXVECTOR3 Direct3D::GetWorldToScreen(::D3DXVECTOR3 &vecWorld, ::D3DXMATRIX *pmtrxView) 
	{
		PT<int> ptScreen;

		D3DXMATRIX mtrxView, mtrxProj, mtrxViewPort; //ビュー行列、投射行列、ビューポート行列

		//各行列を取得

		if (!pmtrxView) //ビュー行列が渡されていない場合
			pD3DDevice->GetTransform(D3DTS_VIEW, &mtrxView); //デバイスから現在のビュー行列を取得
		else
			mtrxView = *pmtrxView;

		pD3DDevice->GetTransform(D3DTS_PROJECTION, &mtrxProj); //投射行列取得
		D3DVIEWPORT9 vp;
		pD3DDevice->GetViewport(&vp); //ビューポート取得
		D3DXMatrixIdentity(&mtrxViewPort);
		mtrxViewPort._11 = vp.Width * 0.5f;
		mtrxViewPort._22 = vp.Height * -0.5f;
		mtrxViewPort._41 = vp.Width * 0.5f;
		mtrxViewPort._42 = vp.Height * 0.5f;
		mtrxViewPort._33 = vp.MaxZ - vp.MinZ;
		mtrxViewPort._43 = vp.MinZ;

		//変換
		D3DXVECTOR3 vecReturn;
		D3DXMATRIX mtrx = mtrxView * mtrxProj * mtrxViewPort;
		D3DXVec3TransformCoord(&vecReturn, &vecWorld, &mtrx);

		return vecReturn;
	}

	//HRESULT値からエラー文字列を取得する
	void Direct3D::GetErrorStr(tstring &str, HRESULT hr) 
	{
		ho::tPrint(str, TEXT("HRESULT=%08X (%s) ErrStr=%s Desc=%s\n"), hr, (FAILED(hr)) ? TEXT("FAILED") : TEXT("SUCCEEDED"), DXGetErrorString9(hr), DXGetErrorDescription9(hr));

		return;
	}

	//ロスト状態のデバイス上で行う処理（リソース解放）
	void Direct3D::OnLostDevice() 
	{
		DeviceLost = true;

		pD3DResourceManagerObj->OnLostDevice(); //ロストデバイス時に D3DPOOL_DEFAULT フラグを伴って作成されたリソースなどをすべて解放する
		pDrawStringObj->OnLostDevice(); //ロストデバイス時のリソース解放

		DeviceReset(); //リソース解放後の復帰処理

		return;
	}

	//ロスト状態のデバイスをリセット
	bool Direct3D::DeviceReset()
	{
		if (D3DERR_DEVICENOTRESET == pD3DDevice->TestCooperativeLevel()) //デバイスが復帰できる場合
		{
			//d3dpp.Windowed = TRUE;
			HRESULT hr;
			if (FAILED(hr = pD3DDevice->Reset(&d3dpp))) //デバイスリセット
			{
				ho::tstring str;
				GetErrorStr(str, hr); //エラー文字列取得

				return false; //リセット失敗
			}

			pD3DResourceManagerObj->OnResetDevice(pD3DDevice); //リセットされたデバイス上で、 D3DPOOL_DEFAULT フラグで作成されたリソースを再作成する
			DeviceLost = false;

			InitRender(); //レンダー設定の初期化

			return true; //リセット成功
		}

		return false; //リセット失敗
	}

	//レンダーターゲットをバックバッファに設定する
	void Direct3D::SetRenderTargetBackBuffer() 
	{
		ho::ComPtr<LPDIRECT3DSURFACE9> pSurface;
		pD3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, pSurface.ToCreator()); //バックバッファのサーフェイスを取得
		pD3DDevice->SetRenderTarget(0, pSurface);
	
		return;
	}
}

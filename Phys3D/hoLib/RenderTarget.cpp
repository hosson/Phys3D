#include "RenderTarget.h"
#include "Debug.h"
#include <d3dx9.h>
#include "Direct3D.h"
#include "D3DResource.h"
#include "../ShaderVertex.h"

namespace ho
{
	//コンストラクタ
	RenderTarget::RenderTarget(Direct3D *pDirect3DObj, const PT<int> &ptSize, D3DFORMAT fmt, DWORD VTColor)
	{
		this->pDirect3DObj = pDirect3DObj;
		this->ptSize = ptSize;
		this->VTColor = VTColor;

		//通常のレンダリング用のテクスチャ作成
		HRESULT hr;
		if (FAILED(hr = D3DXCreateTexture(pDirect3DObj->GetpD3DDevice(), ptSize.x, ptSize.y,
			1, D3DUSAGE_RENDERTARGET, fmt, D3DPOOL_DEFAULT, cpTex.ToCreator())))
		{
			tstring str;
			ho::Direct3D::GetErrorStr(str, hr);
			str = TEXT("レンダリング用のテクスチャ作成に失敗しました。") + str;
			ERR(str.c_str(), __WFILE__, __LINE__, false);
		} else {
			pDirect3DObj->GetpD3DResourceManagerObj()->Regist(new ho::Texture(cpTex)); //リソース管理オブジェクトに登録

			//テクスチャの内容を初期化
			const LPDIRECT3DDEVICE9 pD3DDevice = pDirect3DObj->GetpD3DDevice();
			ho::ComPtr<LPDIRECT3DSURFACE9> cpOldSurface;
			pD3DDevice->GetRenderTarget(0, cpOldSurface.ToCreator());
			this->SetRenderTarget(0);
			pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
			pD3DDevice->SetRenderTarget(0, cpOldSurface);
		}
	}

	//デストラクタ
	RenderTarget::~RenderTarget()
	{
	}

	//デバイスのレンダーターゲットに指定
	void RenderTarget::SetRenderTarget(int Index) 
	{
		if (!cpTex.GetpInterface())
			return;

		ComPtr<LPDIRECT3DSURFACE9> cpSurface;
		cpTex->GetSurfaceLevel(0, cpSurface.ToCreator()); //テクスチャからサーフェイスを取得

		pDirect3DObj->GetpD3DDevice()->SetRenderTarget(Index, cpSurface); //指定されたインデックスにレンダーターゲットを設定

		return;
	}

	//テクスチャを現在のレンダーターゲットに描画
	void RenderTarget::Render()
	{
		const LPDIRECT3DDEVICE9 pD3DDevice = pDirect3DObj->GetpD3DDevice();

		//レンダーターゲットのサイズとレンダリング元のカラーから頂点情報を作成
		D3DVERTEX_2D vt[4];
		vt[0].SetVertex(0, 0, 0, VTColor, 0, 0);
		vt[1].SetVertex((float)ptSize.x, 0, 0, VTColor, 1.0f, 0);
		vt[2].SetVertex(0, (float)ptSize.y, 0, VTColor, 0, 1.0f);
		vt[3].SetVertex((float)ptSize.x, (float)ptSize.y, 0, VTColor, 1.0f, 1.0f);
		D3DVERTEX_2D::Offset(vt, 4);


		ho::ComPtr<IDirect3DBaseTexture9 *> cpOldTexture;
		pD3DDevice->GetTexture(0, cpOldTexture.ToCreator()); //古いテクスチャを保存
		DWORD OldFVF;
		pD3DDevice->GetFVF(&OldFVF); //古い頂点フォーマットを保存

		pD3DDevice->SetTexture(0, cpTex); //テクスチャを設定
		pD3DDevice->SetFVF(ho::D3DVERTEX_2D::FVF);
		pD3DDevice->BeginScene();
		pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, ho::D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(ho::D3DVERTEX_2D));
		pD3DDevice->EndScene();

		pD3DDevice->SetFVF(OldFVF); //古い頂点フォーマットを復元
		pD3DDevice->SetTexture(0, cpOldTexture); //古いテクスチャを復元

		return;
	}



	//テクスチャを現在のレンダーターゲットに描画
	void RenderTarget::Render(ho::PT<int> &ptSize, DWORD *pVtColor)
	{
		const LPDIRECT3DDEVICE9 pD3DDevice = pDirect3DObj->GetpD3DDevice();

		//レンダーターゲットのサイズとレンダリング元のカラーから頂点情報を作成
		D3DVERTEX_2D vt[4];
		const DWORD NowVtColor = [&]()->DWORD //頂点カラーを選択
		{
			if (pVtColor) //引数で指定されている場合
				return *pVtColor;
			return VTColor;
		}();
		vt[0].SetVertex(0, 0, 0, NowVtColor, 0, 0);
		vt[1].SetVertex((float)ptSize.x, 0, 0, NowVtColor, 1.0f, 0);
		vt[2].SetVertex(0, (float)ptSize.y, 0, NowVtColor, 0, 1.0f);
		vt[3].SetVertex((float)ptSize.x, (float)ptSize.y, 0, NowVtColor, 1.0f, 1.0f);
		D3DVERTEX_2D::Offset(vt, 4);


		ho::ComPtr<IDirect3DBaseTexture9 *> cpOldTexture;
		pD3DDevice->GetTexture(0, cpOldTexture.ToCreator()); //古いテクスチャを保存
		DWORD OldFVF;
		pD3DDevice->GetFVF(&OldFVF); //古い頂点フォーマットを保存

		pD3DDevice->SetTexture(0, cpTex); //テクスチャを設定
		pD3DDevice->SetFVF(ho::D3DVERTEX_2D::FVF);
		pD3DDevice->BeginScene();
		pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, ho::D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(ho::D3DVERTEX_2D));
		pD3DDevice->EndScene();

		pD3DDevice->SetFVF(OldFVF); //古い頂点フォーマットを復元
		pD3DDevice->SetTexture(0, cpOldTexture); //古いテクスチャを復元

		return;
	}

	//任意のレンダーターゲットへテクスチャを描画
	void RenderTarget::Render(RenderTarget *pRenderTargetObj, int Index, DWORD *pVtColor, LPDIRECT3DVERTEXDECLARATION9 pvd)
	{
		const LPDIRECT3DDEVICE9 pD3DDevice = pDirect3DObj->GetpD3DDevice();

		//レンダーターゲットのサイズとレンダリング元のカラーから頂点情報を作成
		DWORD NowVtColor;
		if (pVtColor) //引数で頂点カラーが指定されている場合
			NowVtColor = *pVtColor;
		else
			NowVtColor = this->VTColor;
		D3DVERTEX_2D vt[4];
		vt[0].SetVertex(0, 0, 0, NowVtColor, 0, 0);
		vt[1].SetVertex((float)pRenderTargetObj->GetptSize().x, 0, 0, NowVtColor, 1.0f, 0);
		vt[2].SetVertex(0, (float)pRenderTargetObj->GetptSize().y, 0, NowVtColor, 0, 1.0f);
		vt[3].SetVertex((float)pRenderTargetObj->GetptSize().x, (float)pRenderTargetObj->GetptSize().y, 0, NowVtColor, 1.0f, 1.0f);
		D3DVERTEX_2D::Offset(vt, 4);


		pRenderTargetObj->SetRenderTarget(Index); //引数のオブジェクトをレンダーターゲットに設定

		ho::ComPtr<IDirect3DBaseTexture9 *> cpOldTexture;
		pD3DDevice->GetTexture(0, cpOldTexture.ToCreator()); //古いテクスチャを保存
		DWORD OldFVF;
		pD3DDevice->GetFVF(&OldFVF); //古い頂点フォーマットを保存

		pD3DDevice->SetTexture(0, cpTex); //テクスチャを設定

		if (pvd) //頂点宣言が指定されている場合
			pD3DDevice->SetVertexDeclaration(pvd);
		else
			pD3DDevice->SetFVF(ho::D3DVERTEX_2D::FVF);
	
		pD3DDevice->BeginScene();
		pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, ho::D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(ho::D3DVERTEX_2D));
		pD3DDevice->EndScene();

		pD3DDevice->SetFVF(OldFVF); //古い頂点フォーマットを復元
		pD3DDevice->SetTexture(0, cpOldTexture); //古いテクスチャを復元

		return;
	}

	//テクスチャを任意の色で埋める
	void RenderTarget::Fill(const DWORD &Color)
	{
		ho::ComPtr<LPDIRECT3DSURFACE9> cpSurface;
		cpTex->GetSurfaceLevel(0, cpSurface.ToCreator()); //テクスチャのサーフェイスを取得

		pDirect3DObj->GetpD3DDevice()->ColorFill(cpSurface, NULL, Color);

		return;
	}
}

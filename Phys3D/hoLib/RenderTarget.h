//レンダリング対象を示すクラス
#pragma once
#include "ComPtr.hpp"
#include "PT.h"
#include "D3DVertex.h"

namespace ho
{
	class Direct3D;

	class RenderTarget
	{
	public:
		RenderTarget(Direct3D *pDirect3DObj, const PT<int> &ptSize, D3DFORMAT fmt = D3DFMT_A8R8G8B8, DWORD VTColor = 0xffffffff); //コンストラクタ
		~RenderTarget(); //デストラクタ

		void SetRenderTarget(int Index); //デバイスのレンダーターゲットに指定
		void Render(); //テクスチャを現在のレンダーターゲットに描画
		void Render(PT<int> &ptSize, DWORD *pVtColor = NULL); //テクスチャを現在のレンダーターゲットに描画
		void Render(RenderTarget *pRenderTargetObj, int Index, DWORD *pVtColor = NULL, LPDIRECT3DVERTEXDECLARATION9 pvd = NULL); //任意のレンダーターゲットへテクスチャを描画
		void Fill(const DWORD &Color); //テクスチャを任意の色で埋める

		//アクセッサ
		ho::ComPtr<LPDIRECT3DTEXTURE9> GetcpTex() { return cpTex; }
		PT<int> &GetptSize() { return ptSize; }
	private:
		Direct3D *pDirect3DObj;
		PT<int> ptSize; //サイズ
		DWORD VTColor; //頂点カラー
		ho::ComPtr<LPDIRECT3DTEXTURE9> cpTex;
	};
}

/*
テクスチャや頂点情報を保持し、レンダリングターゲットに設定したり、
テクスチャに設定したりする機能を持つ。

バックバッファへの描画などは Render() を使い、
この RenderTarget オブジェクトどうしで
レンダリングするときは Render(RenderTarget *pRenderTargetObj, int Index)
を使う。
*/
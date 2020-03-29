//文字列描画クラス
#include "DrawString.h"
#include "CharTexture.h"
#include "StringTextureCache.h"

namespace ho {

//コンストラクタ
DrawString::DrawString(Direct3D *pDirect3DObj, LPDIRECT3DDEVICE9 pD3DDevice, LOGFONT *pLF, BOOL Cache)
{
	this->pDirect3DObj = pDirect3DObj;
	this->pD3DDevice = pD3DDevice;
	this->pGetCharTextureObj = new GetCharTexture(*pLF, pD3DDevice); //文字テクスチャ取得オブジェクトを作成
	this->pStringTextureCacheManagerObj = NULL;
	if (Cache)
		pStringTextureCacheManagerObj = new StringTextureCacheManager(pD3DDevice, this); //文字列テクスチャキャッシュオブジェクトを作成

	//6つの頂点で表された長方形の頂点インデックスを設定
	Index[0] = 0;
	Index[1] = 1;
	Index[2] = 2;
	Index[3] = 2;
	Index[4] = 1;
	Index[5] = 3;

	//カレント設定
	dwCurrentColor = 0xffffffff;
	CurrentHeight = pGetCharTextureObj->GetLOGFONT().lfHeight;
}

//デストラクタ
DrawString::~DrawString() 
{
	SDELETE(pStringTextureCacheManagerObj);
	SDELETE(pGetCharTextureObj);
}

//描画（カレント設定で描画）
void DrawString::Draw(int x, int y, TCHAR *lpStr)
{
	DrawAlg(x, y, lpStr, dwCurrentColor, CurrentHeight, 0);

	return;
}

//文字列描画（文字サイズの高さを指定）
void DrawString::Draw(int x, int y, TCHAR *lpStr, int Height)
{
	DrawAlg(x, y, lpStr, dwCurrentColor, Height, 0);

	return;
}

//文字列描画（文字の色を指定）
void DrawString::Draw(int x, int y, TCHAR *lpStr, DWORD dwColor)
{
	DrawAlg(x, y, lpStr, dwColor, CurrentHeight, 0);

	return;
}

//文字列描画（文字サイズの高さと色を指定）
void DrawString::Draw(int x, int y, TCHAR *lpStr, int Height, DWORD dwColor)
{
	DrawAlg(x, y, lpStr, dwColor, Height, 0);

	return;
}

//幅を指定して文字列描画
int DrawString::DrawWidth(int x, int y, TCHAR *pStr, int Width, int Height) 
{
	return DrawAlg(x, y, pStr, dwCurrentColor, Height, Width);
}

//1フレーム毎の処理
void DrawString::FrameProcess() 
{
	if (pStringTextureCacheManagerObj)
		pStringTextureCacheManagerObj->Optimize(); //キャッシュの最適化

	return;
}

//デバイスロスト時のリソース解放処理
void DrawString::OnLostDevice()
{
	if (pStringTextureCacheManagerObj)
		pStringTextureCacheManagerObj->Clear(); //キャッシュ削除

	return;
}

//描画の内部処理
int DrawString::DrawAlg(int x, int y, TCHAR *lpStr, DWORD dwColor, int Height, int LimitWidth)
{	
	//頂点フォーマットの設定
	DWORD OldFVF; //変更前の頂点フォーマット
	pD3DDevice->GetFVF(&OldFVF); //頂点フォーマットを保存
	ho::ComPtr<IDirect3DBaseTexture9 *> cpOldTexture; //変更前のテクスチャ
	pD3DDevice->GetTexture(0, cpOldTexture.ToCreator()); //テクスチャを保存
	pD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1); //頂点フォーマットを2Dに指定

	int Drawed = 0; //描画した文字数
	
	if (pStringTextureCacheManagerObj) //文字列キャッシュ使用時
	{
		StringTextureCache *pStringTextureCacheObj = pStringTextureCacheManagerObj->GetpStringTextureCache(lpStr, Height, LimitWidth); //キャッシュを取得

		ho::PT<int> ptTextureSize = pStringTextureCacheObj->GetptTextureSize(); //文字列テクスチャのサイズを取得

		//頂点情報を設定
		vt[0].SetVertex((float)x, (float)y, 0, dwColor, 0.0f, 0.0f);
		vt[1].SetVertex(float(x + ptTextureSize.x), (float)y, 0, dwColor, 1.0f, 0.0f);
		vt[2].SetVertex((float)x, float(y + ptTextureSize.y), 0, dwColor, 0.0f, 1.0f);
		vt[3].SetVertex(float(x + ptTextureSize.x), float(y + ptTextureSize.y), 0, dwColor, 1.0f, 1.0f);
		D3DVERTEX_2D::Offset(vt, 4);

		pD3DDevice->SetTexture(0, pStringTextureCacheObj->GetcpTexture()); //キャッシュから文字列のテクスチャを取得して設定
		pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, Index, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //描画

		Drawed = pStringTextureCacheObj->GetCharNum(); //文字列の文字数を取得
	} else {
		POINT ptTexture; //文字テクスチャの幅と高さ
		double Ratio; //テクスチャと実際に描画するサイズの比率

		//1文字ずつ取り出して描画処理を行う
		int BeginX = x; //描画開始位置のX座標
		while (*lpStr) //lpStrの中身がNULL文字になるまで繰り返す
		{
			ptTexture = pGetCharTextureObj->GetWidthHeight(*lpStr); //文字のテクスチャの幅と高さを取得
			Ratio = (double)Height / (double)pGetCharTextureObj->GetLOGFONT().lfHeight; ////テクスチャと実際に描画するサイズの高さの比率を計算
			ptTexture.x = LONG(ptTexture.x * Ratio);
			ptTexture.y = LONG(ptTexture.y * Ratio);

			if (LimitWidth) //文字列の幅制限が設定されている場合
				if (x + ptTexture.x - BeginX > LimitWidth) //次の文字を描画すると制限幅を超えてしまう場合
					break; //whileループを抜ける

			//頂点情報を設定
			vt[0].SetVertex((float)x - 0.5f, (float)y - 0.5f, 0, dwColor, 0.0f, 0.0f);
			vt[1].SetVertex(float(x + ptTexture.x - 0.5f), (float)y - 0.5f, 0, dwColor, 1.0f, 0.0f);
			vt[2].SetVertex((float)x - 0.5f, float(y + ptTexture.y - 0.5f), 0, dwColor, 0.0f, 1.0f);
			vt[3].SetVertex(float(x + ptTexture.x - 0.5f), float(y + ptTexture.y - 0.5f), 0, dwColor, 1.0f, 1.0f);

			pD3DDevice->SetTexture(0, pGetCharTextureObj->GetTexture(*lpStr)); //文字のテクスチャをデバイスにセット
			pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, Index, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //描画
			Drawed++;

			x += ptTexture.x; //今描いた文字の幅を足し、次の文字の描画位置にする

			lpStr++; //次の文字へポインタを移動
		}
	}

	pD3DDevice->SetFVF(OldFVF); //頂点フォーマットを元に戻しておく
	pD3DDevice->SetTexture(0, cpOldTexture); //テクスチャを戻しておく

	return Drawed;
}

}
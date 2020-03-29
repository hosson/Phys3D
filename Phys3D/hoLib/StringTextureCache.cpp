#include "StringTextureCache.h"
#include "DrawString.h"
#include "CharTexture.h"
#include "Debug.h"
#include "hoLib.h"
#include "Direct3D.h"
#include "D3DResource.h"

//#include "DetectMemoryLeak.h"


namespace ho
{
	//コンストラクタ
	StringTextureCacheManager::StringTextureCacheManager(LPDIRECT3DDEVICE9 pD3DDevice, DrawString *pDrawStringObj)
	{
		this->pD3DDevice = pD3DDevice;
		this->pDrawStringObj = pDrawStringObj;
		this->Time = 0;
	}

	//デストラクタ
	StringTextureCacheManager::~StringTextureCacheManager()
	{
		Clear(); //全てのキャッシュを削除
	}

	//任意の文字列のテクスチャのキャッシュを取得
	StringTextureCache *StringTextureCacheManager::GetpStringTextureCache(TCHAR *pStr, int Height, int LimitWidth)
	{
		for (std::list<StringTextureCache *>::iterator itr = listpStringTextureCacheObj.begin(); itr != listpStringTextureCacheObj.end(); itr++) //文字列テクスチャキャッシュのリストを走査
		{
			if ((*itr)->Compare(pStr)) //キャッシュが見つかった場合
			{
				(*itr)->Use(Time); //キャッシュが使用されることを伝える
				return *itr; //キャッシュを返す
			}
		}

		if (listpStringTextureCacheObj.size() == 500) //キャッシュサイズが500を超えた場合
			ER(TEXT("文字列テクスチャキャッシュの最適化の呼び忘れを確認してください。"), __WFILE__, __LINE__, true); //エラーメッセージ送信

		//キャッシュが見つからなかったので、キャッシュを作成してそれを返す
		StringTextureCache *pStringTextureCacheObj = new StringTextureCache(pStr, Height, LimitWidth, pD3DDevice, this); //文字列テクスチャキャッシュを作成
		listpStringTextureCacheObj.push_back(pStringTextureCacheObj);
		return pStringTextureCacheObj;
	}

	//最適化
	void StringTextureCacheManager::Optimize() 
	{
		//一定期間などのあいだ未使用なキャッシュを削除
		for (std::list<StringTextureCache *>::iterator itr = listpStringTextureCacheObj.begin(); itr != listpStringTextureCacheObj.end();) //文字列テクスチャキャッシュのリストを走査
		{
			if ((*itr)->GetLastUseTime() < Time) //前回の最適化以降使われていない場合
			{
				SDELETE(*itr);
				itr = listpStringTextureCacheObj.erase(itr);
			} else {
				itr++;
			}
		}

		Time++;

		return;
	}

	//全てのキャッシュを削除
	void StringTextureCacheManager::Clear()
	{
		for (std::list<StringTextureCache *>::iterator itr = listpStringTextureCacheObj.begin(); itr != listpStringTextureCacheObj.end();) //文字列テクスチャキャッシュのリストを走査
		{
			SDELETE(*itr);
			itr = listpStringTextureCacheObj.erase(itr);
		}

		return;
	}







	//コンストラクタ
	StringTextureCache::StringTextureCache(TCHAR *pStr, int Height, int LimitWidth, LPDIRECT3DDEVICE9 pD3DDevice, StringTextureCacheManager *pStringTextureCacheManagerObj)
	{

		this->pStringTextureCacheManagerObj = pStringTextureCacheManagerObj;
		this->str = pStr;
		this->CharNum = 0;
		this->LastUseTime = pStringTextureCacheManagerObj->GetTime();

		LOGFONT lf = pStringTextureCacheManagerObj->GetpDrawStringObj()->GetpGetCharTextureObj()->GetLOGFONT(); //フォント情報を取得

		CalcPtStringSize(&this->ptTextureSize, &this->CharNum, pStr, Height, LimitWidth, &lf); //文字列テクスチャのサイズを計算する

		CreateCacheTexture(Height, LimitWidth, pD3DDevice, pStringTextureCacheManagerObj, &lf); //キャッシュとなるテクスチャを作成する
	}

	//キャッシュとなるテクスチャを作成する
	void StringTextureCache::CreateCacheTexture(int Height, int LimitWidth, LPDIRECT3DDEVICE9 pD3DDevice, StringTextureCacheManager *pStringTextureCacheManagerObj, LOGFONT *pLF) 
	{
		HRESULT hr = pD3DDevice->CreateTexture(ptTextureSize.x, ptTextureSize.y, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, cpTexture.ToCreator(), NULL); //文字列のテクスチャを作成

		if (FAILED(hr))
		{
			tstring  str;
			ho::tPrint(str, TEXT("[%s] テクスチャの作成に失敗しました。\nWIDTH %d  HEIGHT %d"), (TCHAR *)str.c_str(), ptTextureSize.x, ptTextureSize.y);
			ER(str.c_str(), __WFILE__, __LINE__, true);
			Direct3D::GetErrorStr(str, hr);
			ER(str.c_str(), __WFILE__, __LINE__, true);
		} else {
			pStringTextureCacheManagerObj->GetpDrawStringObj()->GetpDirect3DObj()->GetpD3DResourceManagerObj()->Regist(new ho::Texture(cpTexture)); //テクスチャをリソース管理クラスに登録

			D3DVERTEX_2D vt[4]; //描画用頂点情報
			POINT ptTexture; //文字テクスチャサイズ取得用
			int x = 0; //描画位置
			DWORD dwColor = 0xffffffff;

			ho::ComPtr<IDirect3DBaseTexture9 *> cpOldTexture;
			pD3DDevice->GetTexture(0, cpOldTexture.ToCreator()); //現在のテクスチャを保存

			DWORD AlphaEnable;
			pD3DDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &AlphaEnable); //アルファブレンドの状態を保存
			pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE); //アルファブレンドをoff

			ho::ComPtr<LPDIRECT3DSURFACE9> cpOldSurface;
			hr = pD3DDevice->GetRenderTarget(0, cpOldSurface.ToCreator()); //現在のレンダーターゲットを保存

			ho::ComPtr<LPDIRECT3DSURFACE9> cpTexSurface; //文字列テクスチャのサーフェイスへのポインタ
			hr = cpTexture->GetSurfaceLevel(0, cpTexSurface.ToCreator()); //文字列テクスチャのサーフェイスを取得
			hr = pD3DDevice->SetRenderTarget(0, cpTexSurface); //レンダリング対象を文字列テクスチャのサーフェイスへ設定

			//文字をテクスチャ上へ描画
			int i = 0;
			for (TCHAR *p = (TCHAR *)str.c_str(); *p != NULL; p++) //文字列を先頭から1文字ずつ走査
			{
				if (i >= this->CharNum)
					break;

				ptTexture = pStringTextureCacheManagerObj->GetpDrawStringObj()->GetpGetCharTextureObj()->GetWidthHeight(*p); //テクスチャのサイズを取得
				double Ratio = (double)Height / (double)pLF->lfHeight; //テクスチャと実際に描画するサイズの高さの比率を計算
				ptTexture.x = int(ptTexture.x * Ratio);
				ptTexture.y = int(ptTexture.y * Ratio);
	
				pD3DDevice->SetTexture(0, pStringTextureCacheManagerObj->GetpDrawStringObj()->GetpGetCharTextureObj()->GetTexture(*p)); //テクスチャをセット
	
				//頂点情報を設定
				vt[0].SetVertex((float)x, (float)0, 0, dwColor, 0.0f, 0.0f);
				vt[1].SetVertex(float(x + ptTexture.x), (float)0, 0, dwColor, 1.0f, 0.0f);
				vt[2].SetVertex((float)x, float(ptTexture.y), 0, dwColor, 0.0f, 1.0f);
				vt[3].SetVertex(float(x + ptTexture.x), float(ptTexture.y), 0, dwColor, 1.0f, 1.0f);
				D3DVERTEX_2D::Offset(vt, 4);

				pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //描画

				x += ptTexture.x; //描画位置を右へ移動
				i++;
			}

			pD3DDevice->SetTexture(0, cpOldTexture); //テクスチャを元に戻しておく
			pD3DDevice->SetRenderTarget(0, cpOldSurface); //レンダリング対象をもとのサーフェイスへ戻しておく
			pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, AlphaEnable); //アルファブレンドを元に戻しておく
		}

		return;
	}

	//デストラクタ
	StringTextureCache::~StringTextureCache()
	{
	}

	//引数の文字列とテクスチャの文字列が同じかどうかを比較する
	BOOL StringTextureCache::Compare(TCHAR *pStr) 
	{
		if (str.compare(pStr) == 0) //文字列が一致した場合
			return TRUE;

		return FALSE;
	}

	//文字列のサイズを計算する
	void StringTextureCache::CalcPtStringSize(ho::PT<int> *pptSize, int *pCharNum, TCHAR *pStr, int Height, int LimitWidth, LOGFONT *pLF) 
	{
		*pCharNum = 0;
		POINT Point;
		ho::PT<int> ptSize(0, 0);
		for (TCHAR *p = pStr; *p != NULL; p++) //文字列を先頭から1文字ずつ走査
		{
			Point = this->pStringTextureCacheManagerObj->GetpDrawStringObj()->GetpGetCharTextureObj()->GetWidthHeight(*p);
			double Ratio = (double)Height / (double)pLF->lfHeight; //テクスチャと実際に描画するサイズの高さの比率を計算
			Point.x = LONG(Point.x * Ratio);
			Point.y = LONG(Point.y * Ratio);

			if (LimitWidth) //幅制限が設定されている場合
				if (ptSize.x + Point.x > LimitWidth) //これ以上の文字数は横幅制限を超えてしまう場合
					break;

			ptSize.x += Point.x;
			if (ptSize.y < Point.y)
				ptSize.y = Point.y;
			(*pCharNum)++;
		}

		*pptSize = ptSize;

		return;
	}
	
}


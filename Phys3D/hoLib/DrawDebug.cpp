#include "DrawDebug.h"
#include "Direct3D.h"
#include "Debug.h"
#include "D3DResource.h"

namespace ho
{
	//コンストラクタ
	DrawDebug::DrawDebug(Direct3D *pDirect3DObj)
	{
		this->pDirect3DObj = pDirect3DObj;
		LPDIRECT3DDEVICE9 pD3DDevice = pDirect3DObj->GetpD3DDevice();
		HRESULT hr;
		Enable = TRUE;

		//デバッグ表示用フォントの設定
		D3DXFONT_DESC FontDesc;
		ZeroMemory(&FontDesc, sizeof(D3DXFONT_DESC));
		FontDesc.Height = 16;
	
		//フォント作成
		hr = D3DXCreateFontIndirect(pD3DDevice, &FontDesc, cpD3DDbgFont.ToCreator());
		if (FAILED(hr))
		{
			tstring str;
			Direct3D::GetErrorStr(str, hr);
			str = TEXT("フォント作成に失敗しました。") + str;
			ER(str.c_str(), __WFILE__, __LINE__, true);
			Enable =  FALSE; //初期化に失敗した
		} else {
			pDirect3DObj->GetpD3DResourceManagerObj()->Regist(new ho::Font(cpD3DDbgFont)); //リソースマネージャに登録
		}

		//フォント表示用スプライト作成
		hr = D3DXCreateSprite(pD3DDevice, cpD3DDbgSprite.ToCreator());
		if (FAILED(hr))
		{
			tstring str;
			Direct3D::GetErrorStr(str, hr);
			str = TEXT("スプライト作成に失敗しました。") + str;
			ER(str.c_str(), __WFILE__, __LINE__, true);
			Enable =  FALSE; //初期化に失敗した
		} else {
			pDirect3DObj->GetpD3DResourceManagerObj()->Regist(new ho::Sprite(cpD3DDbgSprite)); //リソースマネージャに登録
		}
	}

	//デストラクタ
	DrawDebug::~DrawDebug()
	{
	}

	//文字列を追加
	void DrawDebug::Print(TCHAR *pStr)
	{
		listpStr.push_back(tstring(pStr)); //文字列をリストに追加

		return;
	}

	//描画
	void DrawDebug::Draw(int Y)
	{
		if (!Enable)
			return;

		cpD3DDbgSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE); //スプライト描画開始

		RECT rect;
		int LineHeight = 20; //1行の高さ
		for (std::list<tstring>::iterator itr = listpStr.begin(); itr != listpStr.end(); itr++) //文字列リストを走査
		{
			SetRect(&rect, 0, Y, 200, LineHeight);
			cpD3DDbgFont->DrawText(cpD3DDbgSprite, itr->c_str(), -1, &rect, DT_NOCLIP, 0xff00ff00);
			Y += LineHeight;
		}

		cpD3DDbgSprite->End(); //スプライト描画終了

		listpStr.clear(); //文字列リストを消去

		return;
	}
}

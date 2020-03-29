//文字テクスチャを扱うクラス
#include "CharTexture.h"
#include <Windows.h>
#include "hoLib.h"
#include "LibError.h"

namespace ho {
	//コンストラクタ
	GetCharTexture::GetCharTexture(LOGFONT lf, LPDIRECT3DDEVICE9 pD3DDevice)
	{
		this->pD3DDevice = pD3DDevice;

		Objects = 0;
		ZeroMemory(&pCharTextureObj, sizeof(CharTexture *) * 65536); //ポインタ配列を初期化

		this->lf = lf; //生成する文字のフォント情報を保存
	}

	//デストラクタ
	GetCharTexture::~GetCharTexture()
	{
		DeleteAllObj(); //全てのオブジェクトを削除
	}

	//全てのオブジェクトを削除
	void GetCharTexture::DeleteAllObj()
	{
		for (int i = 0; i < 65536; i++)
			SDELETE(pCharTextureObj[i]);

		Objects = 0;

		return;
	}

	//任意の文字のテクスチャ取得
	LPDIRECT3DTEXTURE9 GetCharTexture::GetTexture(TCHAR Char)
	{
		if (!pCharTextureObj[Char]) //テクスチャがまだ存在しない場合
		{
			pCharTextureObj[Char] = new CharTexture(pD3DDevice, Char, this); //テクスチャオブジェクトを作成
			Objects++;
		}

		return pCharTextureObj[Char]->GetTexture();
	}

	//任意の文字のテクスチャの幅と高さを取得
	POINT GetCharTexture::GetWidthHeight(TCHAR Char)
	{
		if (!pCharTextureObj[Char]) //テクスチャがまだ存在しない場合
		{
			pCharTextureObj[Char] = new CharTexture(pD3DDevice, Char, this); //テクスチャオブジェクトを作成
			Objects++;
		}

		return pCharTextureObj[Char]->GetWidthHeight();
	}













	//コンストラクタ
	CharTexture::CharTexture(LPDIRECT3DDEVICE9 pD3DDevice, TCHAR MakeChar, GetCharTexture *pGetCharTexture) 
	{
		this->pGetCharTexture = pGetCharTexture;

		MakeTexture(pD3DDevice, MakeChar, pGetCharTexture->GetLOGFONT()); //文字のテクスチャ作成
	}

	//デストラクタ
	CharTexture::~CharTexture() 
	{
		RELEASE(pD3DTexture);
	}


	BOOL CharTexture::MakeTexture(LPDIRECT3DDEVICE9 pD3DDevice, TCHAR MakeChar, LOGFONT lf)
	{
		HRESULT hr;
		BOOL Space = FALSE; //作成する文字がスペース等でテクスチャサイズが0の場合TRUEになる

		HDC hDC = CreateCompatibleDC(NULL); //現在のスクリーンと互換性のあるデバイスコンテキストハンドルを取得

		//デバイスコンテキストハンドルにフォントを設定
		HFONT hFont = CreateFontIndirect(&lf);
		SelectObject(hDC, hFont);

		// 文字コード取得
		TCHAR *c = &MakeChar;
		UINT code = 0;
	#if _UNICODE //unicodeの場合
		code = (UINT)*c; //文字コードは単純にワイド文字のUINT変換
	#else
		// 2バイト文字のコードは[文字コード]*256 + [先導コード]
		if (IsDBCSLeadByte((BYTE)MakeChar)) //指定された文字が先導コードだった場合
			code = (BYTE)c[0] << 8 | (BYTE)c[1];
		else
			code = c[0];
	#endif

		//文字のビットマップデータ取得
		GLYPHMETRICS gm;
		CONST MAT2 Mat = {{0,1}, {0,0}, {0,0}, {0,1}};
		DWORD dwSize = GetGlyphOutline(hDC, code, GGO_GRAY8_BITMAP, &gm, 0, NULL, &Mat); //必要のバッファサイズ取得
		if (!dwSize) //スペースなどでサイズが0だった場合
		{
			dwSize = lf.lfHeight * (lf.lfHeight / 2); //LOGFONTの縦幅と縦幅/2の横幅のサイズを指定
			Space = TRUE;
		}
		LPBYTE lpBuf = (LPBYTE)GlobalAlloc(GPTR, dwSize); //メモリ取得
		GetGlyphOutline(hDC, code, GGO_GRAY8_BITMAP, &gm, dwSize, lpBuf, &Mat);

		//TEXTMETRIC取得
		TEXTMETRIC tm;
		GetTextMetrics(hDC, &tm);

		//空のテクスチャ作成
		if (Space) //文字がスペースの場合
		{
			Width = lf.lfHeight / 2;
			Height = lf.lfHeight;
		} else {
			Width = gm.gmCellIncX;
			Height = tm.tmHeight;
		}
		hr = pD3DDevice->CreateTexture(Width, Height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pD3DTexture, NULL); 
		if (FAILED(hr))
		{
			tstring  str;
			ho::tPrint(str, TEXT("[%s] テクスチャの作成に失敗しました。\nWIDTH %d  HEIGHT %d"), &MakeChar, Width, Height);
			ER(str.c_str(), __WFILE__, __LINE__, true);
			Direct3D::GetErrorStr(str, hr);
			ER(str.c_str(),  __WFILE__, __LINE__, true);
			return FALSE;
		}
		//テクスチャをロック
		D3DLOCKED_RECT lockRect;
		hr = pD3DTexture->LockRect(0, &lockRect, NULL, 0);
		if (FAILED(hr))
		{
			tstring  str;
			ho::tPrint(str, TEXT("[%s] テクスチャのロックに失敗しました。\nWIDTH %d  HEIGHT %d"), &MakeChar, Width, Height);
			ER(str.c_str(),  __WFILE__, __LINE__, true);
			Direct3D::GetErrorStr(str, hr);
			ER(str.c_str(),  __WFILE__, __LINE__, true);
			return FALSE;
		}

		//テクスチャ初期化
		ZeroMemory(lockRect.pBits, lockRect.Pitch * Height);

		//フォントビットマップのピッチ計算
		int pitch = (gm.gmBlackBoxX + 3) & 0xfffc; //ビットマップのピッチは必ず4の倍数になるため

		//フォントのビットマップをテクスチャへ書き込み
		if (!Space) //文字がスペースではない場合
		{
			BYTE src;
			int OffsetX = gm.gmptGlyphOrigin.x; //オフセット位置のX座標
			int OffsetY = tm.tmAscent - gm.gmptGlyphOrigin.y; //オフセット位置のY座標
			int xMax = gm.gmBlackBoxX; //書き出す文字の右端
			if (xMax > Width) //テクスチャの幅Widthを超えないようにする（GetGlyphOutlineによる描き出しは幅が4Pixel単位となっているため）
				xMax = Width;

			for (int y = 0; y < int(gm.gmBlackBoxY); y++)
			{
				for (int x = 0; x < xMax; x++)
				{
					if ((y + OffsetY) * Width + x + OffsetX >= Width * Height)
					{
						goto LABEL1;
					}
					src = (*(lpBuf + y * pitch + x) * 255) / 64; //α値の取得
					*((LPDWORD)lockRect.pBits + (y + OffsetY) * Width + x + OffsetX) = 0x00ffffff | (src << 24);
				}
			}
		}
	LABEL1:

		//テクスチャをアンロック
		pD3DTexture->UnlockRect(0);

		//メモリ開放
		GlobalFree(lpBuf);
		DeleteObject(hFont);
		DeleteDC(hDC);

		return TRUE;
	}

}

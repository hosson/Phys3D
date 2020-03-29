//文字列描画クラス

#pragma once
#include "Direct3D.h"
#include "D3DVertex.h"

namespace ho {
	class GetCharTexture;
	class StringTextureCacheManager;

	class DrawString
	{
	public:
		DrawString(Direct3D *pDirect3DObj, LPDIRECT3DDEVICE9 pD3DDevice, LOGFONT *pLF, BOOL Cache); //コンストラクタ
		~DrawString(); //デストラクタ

		void FrameProcess(); //1フレーム毎の処理
		void OnLostDevice(); //デバイスロスト時のリソース解放処理

		void Draw(int x, int y, TCHAR *lpStr); //文字列描画（カレント設定で描画）
		void Draw(int x, int y, TCHAR *lpStr, int Height); //文字列描画（文字サイズの高さを指定）
		void Draw(int x, int y, TCHAR *lpStr, DWORD dwColor); //文字列描画（文字の色を指定）
		void Draw(int x, int y, TCHAR *lpStr, int Height, DWORD dwColor); //文字列描画（文字サイズの高さと色を指定）

		int DrawWidth(int x, int y, TCHAR *pStr, int Width, int Height); //幅を指定して文字列描画

		//アクセッサ
		Direct3D *GetpDirect3DObj() { return pDirect3DObj; }
		GetCharTexture *GetpGetCharTextureObj() { return pGetCharTextureObj; }
		StringTextureCacheManager *GetpStringTextureCacheManagerObj() { return pStringTextureCacheManagerObj; }
	private:
		Direct3D *pDirect3DObj;
		LPDIRECT3DDEVICE9 pD3DDevice;
		GetCharTexture *pGetCharTextureObj; //文字テクスチャ取得クラスのポインタ
		StringTextureCacheManager *pStringTextureCacheManagerObj; //文字列テクスチャキャッシュオブジェクトへのポインタ
		D3DVERTEX_2D vt[4]; //描画用頂点情報
		WORD Index[6]; //描画用頂点インデックス
		DWORD dwCurrentColor; //カレント描画色
		int CurrentHeight; //カレント高さ

		//描画の内部処理
		int DrawAlg(int x, int y, TCHAR *lpStr, DWORD dwColor, int Height, int LimitWidth);
	};
}

/*使用方法
プログラム中に1つだけ DrawStringオブジェクトを作っておき、
文字列を描画したい場所で DrawString::Draw() メソッドを呼び出す。

文字のテクスチャ等の管理は自動で行われる。
コンストラクタの第三引数の Cache を TRUE にすると、文字列に対するキャッシュが働き、
VRAM を消費するが DrawPrimitive 系命令の発行回数を抑えて高速化できる。
*/

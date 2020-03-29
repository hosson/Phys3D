//文字テクスチャクラス群
#pragma once
#include "Direct3D.h"
#include <windows.h>

namespace ho {

	class CharTexture; //前方宣言

	//CharTextureクラスから適切な文字を得るクラス
	class GetCharTexture
	{
	public:
		GetCharTexture(LOGFONT lf, LPDIRECT3DDEVICE9 pD3DDevice); //コンストラクタ
		~GetCharTexture(); //デストラクタ
		LPDIRECT3DTEXTURE9 GetTexture(TCHAR Char); //任意の文字のテクスチャ取得
		POINT GetWidthHeight(TCHAR Char); //任意の文字のテクスチャの幅と高さを取得

		LOGFONT GetLOGFONT() { return lf; }
		int GetObjects() { return Objects; }

	private:
		LOGFONT lf;
		int Objects; //作成されたテクスチャの数
		CharTexture *pCharTextureObj[65536]; //2Byteの文字コード分のポインタ配列

		LPDIRECT3DDEVICE9 pD3DDevice;
		void DeleteAllObj(); //全てのオブジェクトを削除

	};

	//文字テクスチャを持つクラス
	class CharTexture
	{
	public:
		CharTexture(LPDIRECT3DDEVICE9 pD3DDevice, TCHAR MakeChar,  GetCharTexture *pGetCharTexture); //コンストラクタ
		~CharTexture(); //デストラクタ
		LPDIRECT3DTEXTURE9 GetTexture() { return pD3DTexture; }
		POINT GetWidthHeight() { POINT pt = {Width, Height}; return pt; }
	private:
		int Width, Height; //テクスチャのサイズ
		LPDIRECT3DTEXTURE9 pD3DTexture; //文字の書き込まれたテクスチャ
		TCHAR Character; //保持している文字
		GetCharTexture *pGetCharTexture; //親クラスのオブジェクトへのポインタ

		BOOL MakeTexture(LPDIRECT3DDEVICE9 pD3DDevice, TCHAR MakeChar, LOGFONT); //テクスチャ作成
	};

}

/*使用方法など

プログラム開始時に文字表示に使用するフォント情報をLOGFONT構造体に入れてGetCharTextureオブジェクトを1つ
作成する。LOGFONT構造体のフォントサイズのテクスチャが作られることになる。
テクスチャを得るには、GetCharTextureクラスのGetTexture()関数へ、得たい文字の文字コードを渡す。
テクスチャの生成に関しては、GetTexture()が呼ばれた時点でまだ作成されていない文字は作成され、
すでに作成された文字に関しては過去に作ったテクスチャが使われる。つまり、一度作ったテクスチャは、
GetCharTextureオブジェクトが破棄されるまで保存される。これにより、フォント生成にかかるCPUの負荷を
抑えることが出来る。しかし、作成されたテクスチャはどんどんビデオメモリ上に溜まっていき、
ビデオカード上のビデオメモリが不足するとメインメモリを使用し始め、パフォーマンスの低下に繋がる恐れがある。
例えば、32Pixel*32Pixel、32Bitカラー、5000種類の文字を使用した場合にはビデオメモリを20MB使用する。
改良点としては、一定期間使用していないテクスチャは自動的に破棄するなどがある。*/

//画像から独自フォントを使うクラス
#pragma once
#include <vector>
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "CSVReader.h"
#include "Archive.h"
#include "PT.h"

namespace ho
{
	class OriginalFont;
	class FontChar;

	class OriginalFontManager
	{
	public:
		OriginalFontManager(); //コンストラクタ
		~OriginalFontManager(); //デストラクタ

		void Add(OriginalFont *pOriginalFontObj, DWORD Index); //独自フォントオブジェクトを追加
		OriginalFont *GetpOriginalFontObj(DWORD Index); //独自フォントオブジェクトを取得
	private:
		std::vector<OriginalFont *>vectorpOriginalFontObj;
	};

	class OriginalFont
	{
	public:
		OriginalFont(LPDIRECT3DDEVICE9 pD3DDevice, const TCHAR *pArchiveName, const TCHAR *pTextureFilename, const TCHAR *pFontInfoFilename, bool Debug); //コンストラクタ
		virtual ~OriginalFont(); //デストラクタ

		void CreateDrawVt(TCHAR *pStr, float z, std::vector<D3DXVECTOR3> &vectorvecVt, std::vector<D3DXVECTOR2> &vectorvecUV, std::vector<WORD> &vectorIndex, int Height = 0); //任意の文字列描画に必要な頂点情報の配列を生成する
		float GetWidth(TCHAR *pStr, int Height); //文字列と高さから描画時の幅を計算する
	protected:
		LPDIRECT3DTEXTURE9 pTexture;
	private:
		FontChar *pFontCharObj[65536]; //文字マップ
	};

	//1文字を表すクラス
	class FontChar
	{
	public:
		FontChar(int Width, int Height, float tu1, float tu2, float tv1, float tv2); //コンストラクタ
		~FontChar(); //デストラクタ

		//アクセッサ
		ho::PT<int> &GetptWH() { return ptWH; }
		D3DXVECTOR2 *Getpuv() { return uv; }
	private:
		ho::PT<int> ptWH; //幅と高さ（ピクセル単位）
		D3DXVECTOR2 uv[4]; //テクスチャ上の座標
	};
}
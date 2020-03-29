//Direct3D 内部用のデバッグ描画クラス

#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <list>
#include "ComPtr.hpp"
#include "tstring.h"

namespace ho
{
	class Direct3D;

	class DrawDebug
	{
	public:
		DrawDebug(Direct3D *pDirect3DObj); //コンストラクタ
		~DrawDebug(); //デストラクタ

		void Print(TCHAR *pStr); //文字列を追加
		void Draw(int Y = 0); //描画
	private:
		Direct3D *pDirect3DObj;
		std::list<tstring> listpStr; //描画文字列リスト
		ho::ComPtr<LPD3DXFONT> cpD3DDbgFont; //デバッグ表示用フォント
		ho::ComPtr<LPD3DXSPRITE> cpD3DDbgSprite; //デバッグ表示用スプライト
		BOOL Enable; //初期化が無事終了したかどうか
	};
}

/*Direct3Dクラス生成時に自動的に作られる。

プログラム中にデバッグ文字列を出力したい場所で Print() メソッドを呼び出す。
IDirect3DDevice9::Presetn() の手前あたりで Draw() メソッドを呼び出すと、
Print() メソッドで登録された文字列が描画される。このとき内部の文字列バッファも
クリアされる。

*/
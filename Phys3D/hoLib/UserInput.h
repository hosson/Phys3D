//キーボードとマウスの入力クラス
#pragma once
#include <Windows.h>
#include "PT.h"
#include "DirectInput.h"

namespace ho
{
	class UserInput
	{
	public:
		struct MOUSESTATE //マウス状態構造体
		{
			ho::PT<int> ptPos; //カーソル位置
			BOOL LeftButton; //左ボタン
			BOOL RightButton; //右ボタン
			int MouseWheel; //マウスホイールの位置
		};
	public:
		UserInput(HWND hWnd, HINSTANCE hInstance); //コンストラクタ
		~UserInput(); //デストラクタ

		void FrameProcess(); //1フレーム毎の処理
		void WM_MouseWheel(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); //WM_MOUSEWHEELが呼び出されたとき

		//アクセッサ
		BYTE *GetpKeyState() { return KeyState[0]; }
		MOUSESTATE *GetpMouseState() { return MouseState; }
		int GetFrameFlag() { return FrameFlag; }
		void SetFullScreen(bool b) { this->FullScreen = b; }
		DirectInput *GetpDirectInputObj() { return pDirectInputObj; }
	private:
		HWND hWnd; //入力対象とするウィンドウハンドル
		int FrameFlag; //0と1を交互に繰り返す
		BYTE KeyState[2][256]; //キーボードの状態
		MOUSESTATE MouseState[2]; //マウスの状態
		int MouseWheel; //マウスホイールの移動量
		bool FullScreen; //フルスクリーンモードかどうか
		DirectInput *pDirectInputObj; //DirectInput オブジェクトへのポインタ
	};
}

/*キーボードとマウスの状態を取得するクラス。
1フレーム毎にFrameProcess()を呼び出し、入力状態を得たいときは
GetpKeyState()を使う。GetFrameFlag()でKeyState[0]かKeyState[1]の
どちらに最新の入力が入っているかを得られる。*/

#include "UserInput.h"
#include "Debug.h"
#include "hoLib.h"
#include "DirectInput.h"

namespace ho
{
	//コンストラクタ
	UserInput::UserInput(HWND hWnd, HINSTANCE hInstance) //コンストラクタ
	{
		this->hWnd = hWnd;
		this->pDirectInputObj = new DirectInput(hInstance, hWnd); //DirectInput オブジェクトを作成
		FrameFlag = 0;
		this->FullScreen = false;

		FrameProcess(); //1フレーム前を入力しておく
	}

	//デストラクタ
	UserInput::~UserInput() 
	{
		SDELETE(pDirectInputObj); //DirectInput オブジェクトを削除
	}

	//1フレーム毎の処理
	void UserInput::FrameProcess() 
	{
		FrameFlag = 1 - FrameFlag;

		GetKeyboardState(KeyState[FrameFlag]); //キーボードの状態を得る

		//マウス位置を取得
		POINT pt;
		GetCursorPos(&pt);
		if (!FullScreen) //フルスクリーンモードではない場合
			ScreenToClient(hWnd, &pt); //ウィンドウのクライアント領域内の座標に変換
		MouseState[FrameFlag].ptPos.x = pt.x;
		MouseState[FrameFlag].ptPos.y = pt.y;
		MouseState[FrameFlag].LeftButton = KeyState[FrameFlag][VK_LBUTTON] & 0x80;
		MouseState[FrameFlag].RightButton = KeyState[FrameFlag][VK_RBUTTON] & 0x80;
		MouseState[FrameFlag].MouseWheel = MouseWheel;
		MouseWheel = 0;


		return;
	}

	//WM_MOUSEWHEELが呼び出されたとき
	void UserInput::WM_MouseWheel(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
	{
		MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA; //マウスホイール移動量を加算

		return;
	}
}

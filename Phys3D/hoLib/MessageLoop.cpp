#include <windows.h>
#include "MessageLoop.h"

namespace ho
{
	//コンストラクタ
	MessageLoop::MessageLoop(const std::function<bool()> &fFrameControlObj)
		: fFrameControlObj(fFrameControlObj)
	{
	}

	//メッセージループ実行
	int MessageLoop::Run()
	{
		MSG msg;

		if (this->fFrameControlObj._Empty()) //フレーム制御関数が設定されていない場合
		{
			//通常アプリケーションのループ
			while (GetMessage(&msg, NULL, 0, 0)) //WM_QUITEが送られるまで繰り返す
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} else {
			//フレーム制御を挟むアプリケーションのループ
			while (true)
			{
				if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) //メッセージがある場合
				{
					if (!GetMessage(&msg, NULL, 0, 0)) //メッセージがWM_QUITの場合
						break;
					TranslateMessage(&msg); //キーボードを利用可能にする
					DispatchMessage(&msg); //制御をWindowsに戻す
				} else {
					if (!this->fFrameControlObj._Empty()) //フレーム制御関数が設定されている場合
						if (this->fFrameControlObj()) //フレーム制御関数を実行し、終了する場合
							break;
				}
			}
		}

		return msg.wParam;
	}
}

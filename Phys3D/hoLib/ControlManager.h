//ウィンドウ上のコントロールを管理するクラス

#pragma once
#include <Windows.h>
#include <vector>

namespace ho {
	class ControlManager
	{
	public:
		struct CONTROLINFO //各コントロールに関する情報
		{
			HWND hWnd; //コントロールのウィンドウハンドル
			FARPROC OldProc; //サブクラス化時に初期状態のプロシージャを保存する変数
		};

		ControlManager() {}
		BOOL RegisthWnd(int Number, HWND hControl); //任意の管理番号にコントロールのウィンドウハンドルを登録
		BOOL RegistOldProc(int Number, FARPROC OldProc); //任意の管理番号にコントロールの初期状態のウィンドウプロシージャを登録
		HWND GethWnd(int Number); //任意の管理番号のコントロールのウィンドウハンドルを取得
		FARPROC GetOldProc(int Number); //任意の管理番号の初期状態のウィンドウプロシージャを取得
		void SetFontSize(int Height); //全てのコントロールのフォントサイズを一括設定
	private:
		std::vector<CONTROLINFO> vectorCI; //各コントロールのウィンドウハンドル配列
	};
}

/*ウィンドウ上のコントロールを管理するクラス。
管理番号で各コントロールを識別し、コントロールのウィンドウハンドルや
デフォルトのウィンドウプロシージャなどを登録・参照することができる。*/

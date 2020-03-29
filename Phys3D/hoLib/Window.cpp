//ウィンドウクラス

#pragma once
#include "Window.h"
#include "Common.h"

namespace ho
{
	//コンストラクタ
	WindowManager::WindowManager()
	{
	}

	//デストラクタ
	WindowManager::~WindowManager()
	{
		//全てのWindowオブジェクトを削除
		for (int i = 0; i < (signed)vectorpWindowObj.size(); i++)
			SDELETE(vectorpWindowObj.at(i));
	}

	//ウィンドウ追加
	bool WindowManager::AddWnd(int Number, Window *pWindowObj)
	{
		if (Number < (signed)vectorpWindowObj.size()) //追加するウィンドウの管理番号が配列のサイズ内の場合
		{
			if (!vectorpWindowObj.at(Number)) //追加するウィンドウの管理番号には何も登録されていない場合
				vectorpWindowObj.at(Number) = pWindowObj;
			else //既に別のウィンドウが登録されている場合
				return FALSE; //失敗
		} else { //追加するウィンドウの管理番号が配列のサイズより大きい場合
			vectorpWindowObj.resize(Number + 1, NULL); //配列のサイズを拡大
			vectorpWindowObj.at(Number) = pWindowObj;
		}

		return TRUE;
	}

	//任意の番号のウィンドウオブジェクトへのポインタを取得
	Window *WindowManager::GetpWindowObj(int Number) 
	{
		if (vectorpWindowObj.size() <= (unsigned)Number) //配列のサイズが範囲外だった場合
			return NULL;
	
		return vectorpWindowObj.at(Number);
	}







	//ウィンドウ生成
	BOOL Window::CreateWnd(WNDCLASSEX *lpWC, CREATEWINDOWEX *lpCW, int nWinMode) 
	{
		//ウィンドウクラスを登録する
		if (RegisterClassEx(lpWC))
		{
			//ウィンドウクラスの登録ができたので、ウィンドウを生成する
			hWnd = CreateWindowExIndirect(lpCW);
			if (!hWnd)
				return FALSE;

			SetProp(hWnd, TEXT("__WINPROC__"), this); //ウィンドウへプロパティを追加してオブジェクトを関連付ける

			ShowWindow(hWnd, nWinMode); //ウィンドウを表示する
			UpdateWindow(hWnd);
		} else {
			return FALSE;
		}

		return TRUE;
	}

	//コンストラクタ（簡易作成）
	Window::Window(HINSTANCE hInstance, TCHAR *pCaption, TCHAR *pClassName, int Width, int Height)
	{
		WNDCLASSEX wc;

		//ウィンドウクラスを定義する
		ZeroMemory(&wc, sizeof(WNDCLASSEX));
		wc.hInstance = hInstance;								//このインスタンスへのハンドル
		wc.lpszClassName = pClassName;							//ウィンドウクラス名
		wc.lpfnWndProc = Window::WndProc;						//ウィンドウプロシージャ関数
		wc.style = 0;											//ウィンドウスタイル（デフォルト）
		wc.cbSize = sizeof(WNDCLASSEX);							//WNDCLASSEX構造体のサイズを設定
		wc.hIcon = LoadIcon(hInstance, IDI_WINLOGO);			//ラージアイコン
		wc.hIconSm = LoadIcon(hInstance, IDI_WINLOGO);			//スモールアイコン
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);				//カーソルスタイル
		wc.lpszMenuName = NULL;									//メニュー
		wc.cbClsExtra = 0;										//エキストラ
		wc.cbWndExtra = 0;										//必要な情報
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);	//ウィンドウの背景

		RECT rect, rcDesktop;

		GetWindowRect(GetDesktopWindow(), &rcDesktop); //デスクトップの大きさを取得
		SetRect(&rect, 0, 0, Width, Height);
		AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0);

		CREATEWINDOWEX cw;

		ZeroMemory(&cw, sizeof(CREATEWINDOWEX));
		cw.dwExStyle = 0;								//拡張ウィンドウスタイル
		cw.lpClassName = wc.lpszClassName;				//ウィンドウクラスの名前
		cw.lpWindowName	= pCaption;						//ウィンドウタイトル
		cw.dwStyle = WS_OVERLAPPEDWINDOW;				//ウィンドウスタイル
		cw.x = (rcDesktop.right - rect.right) / 2;		// ウィンドウの左角Ｘ座標
		cw.y = (rcDesktop.bottom - rect.bottom) / 2;	// ウィンドウの左角Ｙ座標
		cw.nWidth = rect.right - rect.left;				// ウィンドウの幅
		cw.nHeight = rect.bottom - rect.top;			// ウィンドウの高さ
		cw.hWndParent =	NULL;							//親ウィンドウ
		cw.hMenu = 	NULL;								//メニュー
		cw.hInstance = hInstance;						//このプログラムのインスタンスのハンドル
		cw.lpParam = NULL;								//追加引数	
			
		if (!CreateWnd(&wc, &cw, SW_SHOWNORMAL)) //ウィンドウ生成
		{
			//ウィンドウ生成に失敗した場合の処理を書く
		}
	}

	//コンストラクタ（詳細作成）
	Window::Window(WNDCLASSEX *pWCE, CREATEWINDOWEX *pCWE) 
	{
		CreateWnd(pWCE, pCWE, SW_SHOWNORMAL); //ウィンドウ生成
	}

	//CreateWindowExのラッパー関数
	HWND Window::CreateWindowExIndirect(CREATEWINDOWEX *lpCW) 
	{
		return CreateWindowEx(
			lpCW->dwExStyle,	//拡張ウィンドウスタイル
			lpCW->lpClassName,	//ウィンドウクラスの名前
			lpCW->lpWindowName,	//ウィンドウタイトル
			lpCW->dwStyle,		//ウィンドウスタイル
			lpCW->x,			//ウィンドウの左角Ｘ座標
			lpCW->y,			//ウィンドウの左角Ｙ座標
			lpCW->nWidth,		//ウィンドウの幅
			lpCW->nHeight,		//ウィンドウの高さ
			lpCW->hWndParent,	//親ウィンドウ
			lpCW->hMenu,		//メニュー
			lpCW->hInstance,	//このプログラムのインスタンスのハンドル
			lpCW->lpParam		//追加引数
		);
	}

	//デストラクタ
	Window::~Window()
	{
		if (hWnd)
			SendMessage(hWnd, WM_DESTROY, 0, 0); //ウィンドウを閉じる
	}

	//ウィンドウプロシージャ関数
	LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
	{
		Window *lpObj = (Window *)GetProp(hWnd, TEXT("__WINPROC__")); //ウィンドウハンドルから関連付けられたオブジェクトを取得

		if (lpObj) //オブジェクトが取得できた場合
			return lpObj->LocalWndProc(hWnd, msg, wParam, lParam);

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	//オーバーライド用ウィンドウプロシージャ
	LRESULT Window::LocalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
	{
		switch (msg)
		{
		case WM_DESTROY: //閉じるボタンをクリックした時
			RemoveProp(hWnd, TEXT("__WINPROC__")); //関連付けらていたプロパティを削除
			PostQuitMessage(0); //WM_QUITメッセージを発行
			break;
		default: //上記以外のメッセージはWindowsへ処理を任せる
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	//無効領域の作成（描画更新）
	void Window::MakeInvalidateRect() 
	{
		InvalidateRect(hWnd, NULL, TRUE);

		return;
	}
}

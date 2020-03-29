//ウィンドウクラス
#pragma once
#include <windows.h>
#include <vector>

namespace ho
{
	//前方宣言
	class Window;

	//ウィンドウクラスを管理するクラス
	class WindowManager
	{
	public:
		WindowManager(); //コンストラクタ
		~WindowManager(); //デストラクタ
		bool AddWnd(int Number, Window *pWindowObj); //ウィンドウ追加
		Window *GetpWindowObj(int Number); //任意の番号のウィンドウオブジェクトへのポインタを取得
	private:
		std::vector<Window *> vectorpWindowObj; //ウィンドウオブジェクトへのポインタの配列
	};

	//ウィンドウクラス
	class Window
	{
	public:
		struct CREATEWINDOWEX
		{
			unsigned int dwExStyle;		// 拡張ウィンドウスタイル
			LPCTSTR lpClassName;	// 登録されているクラス名
			LPCTSTR lpWindowName;	// ウィンドウ名
			unsigned int dwStyle;			// ウィンドウスタイル
			int x;					// ウィンドウの横方向の位置
			int y;					// ウィンドウの縦方向の位置
			int nWidth;				// ウィンドウの幅
			int nHeight;			// ウィンドウの高さ
			HWND hWndParent;		// 親ウィンドウまたはオーナーウィンドウのハンドル
			HMENU hMenu;			// メニューハンドルまたは子識別子
			HINSTANCE hInstance;	// アプリケーションインスタンスのハンドル
			LPVOID lpParam;			// ウィンドウ作成データ
		};

		Window(HINSTANCE hInstance, TCHAR *pCaption, TCHAR *pClassName, int Width = 640, int Height = 480); //コンストラクタ（簡易作成）
		Window(WNDCLASSEX *pWCE, CREATEWINDOWEX *pCWE); //コンストラクタ（詳細作成）
		Window() {} //コンストラクタ（何もしない）
		virtual ~Window(); //デストラクタ

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); //ウィンドウプロシージャ
		void MakeInvalidateRect(); //無効領域の作成（描画更新）
		HWND GethWnd() { return hWnd; } //ウィンドウハンドルを取得
	protected:
		HWND hWnd;

		BOOL CreateWnd(WNDCLASSEX *lpWC, CREATEWINDOWEX *lpCW, int nWinMode); //ウィンドウ生成
		HWND CreateWindowExIndirect(CREATEWINDOWEX *); //CreateWindowExのラッパー関数
		virtual LRESULT LocalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); //オーバーライド用ウィンドウプロシージャ
		virtual void Init() {} //ウィンドウが作成された時の初期化処理（オーバーライド用）
	};
}

/*プログラム全体でグローバルなウィンドウ管理クラス（WindowManager）を一つ作る。

ウィンドウを作成するにはWindowクラスのオブジェクトを作り、そのポインタをWindowManager::AddWnd()で登録する。
AddWnd()の引数のNumberで個々のウィンドウは識別されるので、同じ番号に複数のウィンドウを作ることは出来ない。
*/

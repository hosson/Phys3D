#include "ControlManager.h"
#include <WindowsX.h>

namespace ho {
	//任意の管理番号にコントロールのウィンドウハンドルを登録
	BOOL ControlManager::RegisthWnd(int Number, HWND hControl)
	{
		if (Number < (signed)vectorCI.size()) //追加するコントロールの管理番号が配列のサイズ内の場合
		{
			if (!vectorCI.at(Number).hWnd) //追加するコントロールの管理番号には何も登録されていない場合
			{
				vectorCI.at(Number).hWnd = hControl;
			} else { //既に別のコントロールのウィンドウハンドルが登録されている場合
				return FALSE; //失敗
			}
		} else { //追加するコントロールの管理番号が配列のサイズより大きい場合
			CONTROLINFO CI;
			ZeroMemory(&CI, sizeof(CONTROLINFO));
			vectorCI.resize(Number + 1, CI); //配列のサイズを拡大
			vectorCI.at(Number).hWnd = hControl;
		}

		return TRUE;
	}

	//任意の管理番号にコントロールの初期状態のウィンドウプロシージャを登録
	BOOL ControlManager::RegistOldProc(int Number, FARPROC OldProc)
	{
		if (Number < (signed)vectorCI.size()) //追加するコントロールの管理番号が配列のサイズ内の場合
		{
			if (!vectorCI.at(Number).OldProc) //追加するコントロールの管理番号には何も登録されていない場合
			{
				vectorCI.at(Number).OldProc = OldProc;
			} else { //既に別のコントロールのウィンドウプロシージャが登録されている場合
				return FALSE; //失敗
			}
		} else { //追加するコントロールの管理番号が配列のサイズより大きい場合
			CONTROLINFO CI;
			ZeroMemory(&CI, sizeof(CONTROLINFO));
			vectorCI.resize(Number + 1, CI); //配列のサイズを拡大
			vectorCI.at(Number).OldProc = OldProc;
		}

		return TRUE;
	}


	//任意の管理番号のコントロールのウィンドウハンドルを取得
	HWND ControlManager::GethWnd(int Number)
	{
		if (Number < (signed)vectorCI.size()) //追加するコントロールの管理番号が配列のサイズ内の場合
			return vectorCI.at(Number).hWnd;

		return NULL;
	}

	//任意の管理番号の初期状態のウィンドウプロシージャを取得
	FARPROC ControlManager::GetOldProc(int Number)
	{
		if (Number < (signed)vectorCI.size()) //追加するコントロールの管理番号が配列のサイズ内の場合
			return vectorCI.at(Number).OldProc;

		return NULL;
	}


	//全てのコントロールのフォントサイズを一括設定
	void ControlManager::SetFontSize(int Height) 
	{
		//フォント情報作成
		LOGFONT logFont;
		HFONT hFont;
		ZeroMemory(&logFont, sizeof(LOGFONT));
		logFont.lfHeight = Height;
		hFont = CreateFontIndirect(&logFont);

		for (unsigned int i = 0; i < vectorCI.size(); i++)
			SetWindowFont(vectorCI.at(i).hWnd, hFont, TRUE);

		return;
	}
}

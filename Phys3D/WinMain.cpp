#include <Windows.h>
#include <boost\shared_ptr.hpp>
#include "App.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszCmdLine, int nWinMode)
{
	std::shared_ptr<App> spAppObj(new App()); //アプリケーション頂点オブジェクト作成

	int Return = spAppObj->Run(spAppObj, hInstance); //メッセージループ実行

	return Return;
}

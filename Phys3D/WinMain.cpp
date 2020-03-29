#include <Windows.h>
#include <boost\shared_ptr.hpp>
#include "App.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszCmdLine, int nWinMode)
{
	std::shared_ptr<App> spAppObj(new App()); //�A�v���P�[�V�������_�I�u�W�F�N�g�쐬

	int Return = spAppObj->Run(spAppObj, hInstance); //���b�Z�[�W���[�v���s

	return Return;
}

#include "App.h"
#include "hoLib\Window.h"
#include "hoLib\D3D11.h"
#include "hoLib\MessageLoop.h"
#include "hoLib\FrameControl.h"
#include "AppCore.h"
#include "hoLib\Error.h"

App::App()
{
}

App::~App()
{
}

//実行
int App::Run(const std::shared_ptr<App> &spAppObj, HINSTANCE hInstance)
{
	ho::ErrorManager::spErrorManagerObj.reset(new ho::ErrorManager()); //エラー管理オブジェクト作成

	this->spAppCoreObj.reset(new AppCore(spAppObj)); //アプリケーションコアオブジェクト作成

	this->spWindowManagerObj.reset(new ho::WindowManager()); //ウィンドウ管理オブジェクト作成
	this->spWindowManagerObj->AddWnd(0, new ho::Window(hInstance, TEXT("Caption"), TEXT("CLASS_NAME"), 640, 480)); //ウィンドウオブジェクト作成

	//Direct3D11オブジェクト作成
	this->spD3D11Obj.reset(new ho::D3D11::D3D11(640, 480));
	this->spD3D11Obj->Init(this->spD3D11Obj, this->spWindowManagerObj->GetpWindowObj(0)->GethWnd()); //初期化

	this->spFrameControlObj.reset(new ho::FrameControl(std::bind(std::mem_fn(&AppCore::Update), this->spAppCoreObj), std::bind(std::mem_fn(&AppCore::Draw), this->spAppCoreObj))); //フレームレート制御オブジェクト作成
	this->spMessageLoopObj.reset(new ho::MessageLoop(std::bind(std::mem_fn(&ho::FrameControl::Control), this->spFrameControlObj))); //メッセージループオブジェクト作成

	this->spAppCoreObj->Init(this->spAppCoreObj); //アプリケーションコアオブジェクトの初期化

	return this->spMessageLoopObj->Run(); //メッセージループ実行
}

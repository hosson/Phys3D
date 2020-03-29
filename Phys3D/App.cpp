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

//���s
int App::Run(const std::shared_ptr<App> &spAppObj, HINSTANCE hInstance)
{
	ho::ErrorManager::spErrorManagerObj.reset(new ho::ErrorManager()); //�G���[�Ǘ��I�u�W�F�N�g�쐬

	this->spAppCoreObj.reset(new AppCore(spAppObj)); //�A�v���P�[�V�����R�A�I�u�W�F�N�g�쐬

	this->spWindowManagerObj.reset(new ho::WindowManager()); //�E�B���h�E�Ǘ��I�u�W�F�N�g�쐬
	this->spWindowManagerObj->AddWnd(0, new ho::Window(hInstance, TEXT("Caption"), TEXT("CLASS_NAME"), 640, 480)); //�E�B���h�E�I�u�W�F�N�g�쐬

	//Direct3D11�I�u�W�F�N�g�쐬
	this->spD3D11Obj.reset(new ho::D3D11::D3D11(640, 480));
	this->spD3D11Obj->Init(this->spD3D11Obj, this->spWindowManagerObj->GetpWindowObj(0)->GethWnd()); //������

	this->spFrameControlObj.reset(new ho::FrameControl(std::bind(std::mem_fn(&AppCore::Update), this->spAppCoreObj), std::bind(std::mem_fn(&AppCore::Draw), this->spAppCoreObj))); //�t���[�����[�g����I�u�W�F�N�g�쐬
	this->spMessageLoopObj.reset(new ho::MessageLoop(std::bind(std::mem_fn(&ho::FrameControl::Control), this->spFrameControlObj))); //���b�Z�[�W���[�v�I�u�W�F�N�g�쐬

	this->spAppCoreObj->Init(this->spAppCoreObj); //�A�v���P�[�V�����R�A�I�u�W�F�N�g�̏�����

	return this->spMessageLoopObj->Run(); //���b�Z�[�W���[�v���s
}

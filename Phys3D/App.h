//�A�v���P�[�V�������_�N���X
#pragma once
#include <Windows.h>
#include <boost\shared_ptr.hpp>

//�O���錾
namespace ho
{
	class ErrorManager;
	class WindowManager;
	class MessageLoop;
	class FrameControl;

	namespace D3D11
	{
		class D3D11;
	}
}
class AppCore;

class App
{
public:
	App(); //�R���X�g���N�^
	~App(); //�f�X�g���N�^

	int Run(const std::shared_ptr<App> &spAppObj, HINSTANCE hInstance); //���s

	//�A�N�Z�b�T
	const std::shared_ptr<ho::D3D11::D3D11> &getspD3D11Obj() const { return spD3D11Obj; }
private:
	std::shared_ptr<ho::WindowManager> spWindowManagerObj; //�E�B���h�E�Ǘ��I�u�W�F�N�g�ւ̃|�C���^
	std::shared_ptr<ho::D3D11::D3D11> spD3D11Obj; //Direct3D11�I�u�W�F�N�g�ւ̃|�C���^
	std::shared_ptr<ho::FrameControl> spFrameControlObj; //�t���[�����[�g����I�u�W�F�N�g�ւ̃|�C���^
	std::shared_ptr<ho::MessageLoop> spMessageLoopObj; //���b�Z�[�W���[�v�I�u�W�F�N�g�ւ̃|�C���^
	std::shared_ptr<AppCore> spAppCoreObj; //�A�v���P�[�V�����R�A�I�u�W�F�N�g�ւ̃|�C���^
};

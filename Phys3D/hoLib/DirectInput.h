//DirectInput �N���X
#pragma once
#define DIRECTINPUT_VERSION 0x0800 //dinput.h�̃C���N���[�h�����DirectInput�̃o�[�W�������`���Ă���
#include <dinput.h>
#include <Windows.h>
#include <list>

#pragma comment (lib, "dinput8.lib")

namespace ho
{
	//�O���錾
	class DirectInputDevice;

	class DirectInput
	{
	public:
		DirectInput(HINSTANCE hInstance, HWND hWnd); //�R���X�g���N�^
		~DirectInput(); //�f�X�g���N�^

		static BOOL CALLBACK DIEnumDeviceCallback(LPCDIDEVICEINSTANCE pddi, LPVOID pRef); //�f�o�C�X�񋓗p�R�[���o�b�N�֐�
		BOOL DIEnumDeviceLocal(LPCDIDEVICEINSTANCE pddi); //�f�o�C�X�񋓗p�̃��[�J���֐�

		//�A�N�Z�b�T
		LPDIRECTINPUT8 GetpDI8() { return pDI8; }
		HWND GethWnd() { return hWnd; }
		std::list<DirectInputDevice *> &GetlistpDirectInputDeviceObj() { return listpDirectInputDeviceObj; }
	private:
		LPDIRECTINPUT8 pDI8; //DirectInput8 �C���^�[�t�F�[�X�ւ̃|�C���^
		std::list<DirectInputDevice *> listpDirectInputDeviceObj; //�f�o�C�X�I�u�W�F�N�g�ւ̃|�C���^�̃��X�g
		HWND hWnd;
	};

	class DirectInputDevice
	{
	public:
		DirectInputDevice(LPCDIDEVICEINSTANCE pddi, DirectInput *pDirectInputObj); //�R���X�g���N�^
		~DirectInputDevice(); //�f�X�g���N�^

		DIDEVICEINSTANCE *Getpdidi() { return &didi; }
		DIJOYSTATE GetDIJOYSTATE(); //DIJOYSTATE�\���̂𓾂�
	private:
		DIDEVICEINSTANCE didi;
		DirectInput *pDirectInputObj;
		LPDIRECTINPUTDEVICE8 pDID8; //DirectInput8�f�o�C�X�̃C���^�[�t�F�[�X�ւ̃|�C���^
		bool Enable;
	};
}

/*
�v���O�������̏��������� DirectInput �I�u�W�F�N�g������A�I�����ɍ폜����B
���� DirectInput �I�u�W�F�N�g�͐������ɁA�����I�Ɋe�R���g���[���ɑΉ�����
DirectInputDevice �I�u�W�F�N�g�𐶐�����B
�����̓��X�g�Ɋi�[����Ă���̂ŁADirectInput::GetlistpDirectInputDeviceObj() ��
���X�g�����ł��擾�ł���B�����āADirectInputDevice::GetDIJOYSTATE() �ŃR���g���[����
��Ԃ��擾����B
*/
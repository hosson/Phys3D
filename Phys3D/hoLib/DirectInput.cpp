#include "DirectInput.h"
#include <WinError.h>
#include "LibError.h"
#include "Direct3D.h"

namespace ho
{
	//�R���X�g���N�^
	DirectInput::DirectInput(HINSTANCE hInstance, HWND hWnd)
	{
		this->hWnd = hWnd;
		HRESULT hr;
		pDI8 = NULL;

		try
		{
			//DirectInput8�C���^�[�t�F�[�X�̎擾
			if (FAILED(hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID *)&pDI8, NULL)))
			{
				tstring str;
				Direct3D::GetErrorStr(str, hr);
				str = TEXT("DirectInput8 �I�u�W�F�N�g�̍쐬�Ɏ��s���܂����B") + str;
				throw Exception(str, __WFILE__, __LINE__, true);
			}

			//�f�o�C�X��1
			if (FAILED(hr = pDI8->EnumDevices(DI8DEVCLASS_GAMECTRL, DIEnumDeviceCallback, this, DIEDFL_ATTACHEDONLY)))
			//if (FAILED(hr = pDI8->EnumDevices(DI8DEVTYPE_JOYSTICK, DIEnumDeviceCallback, this, DIEDFL_ATTACHEDONLY)))
			{
				tstring str;
				Direct3D::GetErrorStr(str, hr);
				str = TEXT("DirectInput �f�o�C�X�̗񋓂Ɏ��s���܂����B") + str;
				throw Exception(str, __WFILE__, __LINE__, true);
			}
		}
		catch (Exception &e)
		{
			ER(e.strMsg.c_str(), e.pFile, e.Line, e.Continue);
		}

	}

	//�f�X�g���N�^
	DirectInput::~DirectInput()
	{
		for (std::list<DirectInputDevice *>::iterator itr = listpDirectInputDeviceObj.begin(); itr != listpDirectInputDeviceObj.end(); itr++)
			SDELETE(*itr);

		RELEASE(pDI8);
	}

	//�f�o�C�X�񋓗p�R�[���o�b�N�֐�
	BOOL CALLBACK DirectInput::DIEnumDeviceCallback(LPCDIDEVICEINSTANCE pddi, LPVOID pRef)
	{
		return ((DirectInput *)pRef)->DIEnumDeviceLocal(pddi); //�I�u�W�F�N�g���̃����o���Ăяo��
	}

	//�f�o�C�X�񋓗p�̃��[�J���֐�
	BOOL DirectInput::DIEnumDeviceLocal(LPCDIDEVICEINSTANCE pddi) 
	{
		if (pddi->guidInstance.Data1) //�f�o�C�X������ꂽ�ꍇ
			listpDirectInputDeviceObj.push_back(new DirectInputDevice(pddi, this)); //�f�o�C�X�I�u�W�F�N�g���쐬

		return DIENUM_CONTINUE; //�񋓂𑱂���
	}

	




	//�R���X�g���N�^
	DirectInputDevice::DirectInputDevice(LPCDIDEVICEINSTANCE pddi, DirectInput *pDirectInputObj)
	{
		this->didi = *pddi;
		this->pDID8 = NULL;
		this->pDirectInputObj = pDirectInputObj;
		this->Enable = false;

		HRESULT hr;

		try
		{
			//DirectInput8�f�o�C�X���쐬
			if (FAILED(hr = pDirectInputObj->GetpDI8()->CreateDevice(didi.guidInstance, &pDID8, NULL)))
			{
				tstring str;
				Direct3D::GetErrorStr(str, hr);
				str = TEXT("DirectInput8 �I�u�W�F�N�g�̍쐬�Ɏ��s���܂����B") + str;
				throw Exception(str, __WFILE__, __LINE__, true);
			}

			//�f�o�C�X�̃f�[�^�t�H�[�}�b�g�̐ݒ�
			if (FAILED(hr = pDID8->SetDataFormat(&c_dfDIJoystick)))
			{
				tstring str;
				Direct3D::GetErrorStr(str, hr);
				str = TEXT("DirectInput8�f�o�C�X�̃f�[�^�t�H�[�}�b�g�̐ݒ�Ɏ��s���܂����B") + str;
				throw Exception(str, __WFILE__, __LINE__, true);
			}

			//�������[�h�̐ݒ�
			if (FAILED(hr = pDID8->SetCooperativeLevel(pDirectInputObj->GethWnd(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))
			{
				tstring str;
				Direct3D::GetErrorStr(str, hr);
				str = TEXT("DirectInput8�f�o�C�X�̋������[�h�̐ݒ�Ɏ��s���܂����B") + str;
				throw Exception(str, __WFILE__, __LINE__, true);
			}

			//�����[�h���Βl���[�h��
			DIPROPDWORD dipdw;
			ZeroMemory(&dipdw, sizeof(DIPROPDWORD));
			dipdw.diph.dwSize = sizeof(DIPROPDWORD);
			dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			dipdw.diph.dwObj = 0;
			dipdw.diph.dwHow = DIPH_DEVICE;
			dipdw.dwData = DIPROPAXISMODE_ABS;
			if (FAILED(hr = pDID8->SetProperty(DIPROP_AXISMODE, &dipdw.diph)))
			{
				tstring str;
				Direct3D::GetErrorStr(str, hr);
				str = TEXT("DirectInput8�f�o�C�X�̃v���p�e�B�̐ݒ�Ɏ��s���܂����B") + str;
				throw Exception(str, __WFILE__, __LINE__, true);
			}
		
			//���̒l�͈̔͐ݒ�
			//�\���L�[�������Ă��Ȃ��Ƃ���0�ɂȂ�悤��
			DIPROPRANGE dipr;
			ZeroMemory(&dipr, sizeof(DIPROPRANGE));
			dipr.diph.dwSize = sizeof(DIPROPRANGE);
			dipr.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			dipr.diph.dwObj	= DIJOFS_X; //X��
			dipr.diph.dwHow	= DIPH_BYOFFSET;
			dipr.lMin = -1000;
			dipr.lMax = 1000;
			if (FAILED(hr = pDID8->SetProperty(DIPROP_RANGE, &dipr.diph)))
			{
				tstring str;
				Direct3D::GetErrorStr(str, hr);
				str = TEXT("DirectInput8�f�o�C�X�̃v���p�e�B�̐ݒ�Ɏ��s���܂����B") + str;
				throw Exception(str, __WFILE__, __LINE__, true);
			}
			dipr.diph.dwObj = DIJOFS_Y; //Y��
			if (FAILED(hr = pDID8->SetProperty(DIPROP_RANGE, &dipr.diph)))
			{
				tstring str;
				Direct3D::GetErrorStr(str, hr);
				str = TEXT("DirectInput8�f�o�C�X�̃v���p�e�B�̐ݒ�Ɏ��s���܂����B") + str;
				throw Exception(str, __WFILE__, __LINE__, true);
			}

			//�L�[���͎擾���ɍs���̂ł����ł͕s�v�H
			/*
			//���͂�������
			if (FAILED(hr = pDID8->Acquire()))
			{
				tstring str;
				Direct3D::GetErrorStr(str, hr);
				str = TEXT("DirectInput8�f�o�C�X�̃A�N�Z�X���擾�Ɏ��s���܂����B") + str;
				throw Exception(str, __WFILE__, __LINE__, true);
			}
			*/

			Enable = true;

		}
		catch (Exception &e)
		{
			ER(e.strMsg.c_str(), e.pFile, e.Line, e.Continue);
		}
	}

	//�f�X�g���N�^
	DirectInputDevice::~DirectInputDevice()
	{
		if (pDID8) //�f�o�C�X���쐬�ł��Ă����ꍇ
		{
			pDID8->Unacquire(); //�A�N�Z�X�������
			RELEASE(pDID8);
		}
	}

	//DIJOYSTATE�\���̂𓾂�
	DIJOYSTATE DirectInputDevice::GetDIJOYSTATE()
	{
		DIJOYSTATE dijs;
		ZeroMemory(&dijs, sizeof(DIJOYSTATE));

		if (!Enable)
			return dijs;

		HRESULT hr;
		if (SUCCEEDED(hr = pDID8->Acquire())) //���͂�������
		{
			pDID8->Poll();
			pDID8->GetDeviceState(sizeof(DIJOYSTATE), &dijs);
		}

		return dijs;
	}
}

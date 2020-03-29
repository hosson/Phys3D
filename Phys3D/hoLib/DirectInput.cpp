#include "DirectInput.h"
#include <WinError.h>
#include "LibError.h"
#include "Direct3D.h"

namespace ho
{
	//コンストラクタ
	DirectInput::DirectInput(HINSTANCE hInstance, HWND hWnd)
	{
		this->hWnd = hWnd;
		HRESULT hr;
		pDI8 = NULL;

		try
		{
			//DirectInput8インターフェースの取得
			if (FAILED(hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID *)&pDI8, NULL)))
			{
				tstring str;
				Direct3D::GetErrorStr(str, hr);
				str = TEXT("DirectInput8 オブジェクトの作成に失敗しました。") + str;
				throw Exception(str, __WFILE__, __LINE__, true);
			}

			//デバイス列挙1
			if (FAILED(hr = pDI8->EnumDevices(DI8DEVCLASS_GAMECTRL, DIEnumDeviceCallback, this, DIEDFL_ATTACHEDONLY)))
			//if (FAILED(hr = pDI8->EnumDevices(DI8DEVTYPE_JOYSTICK, DIEnumDeviceCallback, this, DIEDFL_ATTACHEDONLY)))
			{
				tstring str;
				Direct3D::GetErrorStr(str, hr);
				str = TEXT("DirectInput デバイスの列挙に失敗しました。") + str;
				throw Exception(str, __WFILE__, __LINE__, true);
			}
		}
		catch (Exception &e)
		{
			ER(e.strMsg.c_str(), e.pFile, e.Line, e.Continue);
		}

	}

	//デストラクタ
	DirectInput::~DirectInput()
	{
		for (std::list<DirectInputDevice *>::iterator itr = listpDirectInputDeviceObj.begin(); itr != listpDirectInputDeviceObj.end(); itr++)
			SDELETE(*itr);

		RELEASE(pDI8);
	}

	//デバイス列挙用コールバック関数
	BOOL CALLBACK DirectInput::DIEnumDeviceCallback(LPCDIDEVICEINSTANCE pddi, LPVOID pRef)
	{
		return ((DirectInput *)pRef)->DIEnumDeviceLocal(pddi); //オブジェクト内のメンバを呼び出す
	}

	//デバイス列挙用のローカル関数
	BOOL DirectInput::DIEnumDeviceLocal(LPCDIDEVICEINSTANCE pddi) 
	{
		if (pddi->guidInstance.Data1) //デバイスが得られた場合
			listpDirectInputDeviceObj.push_back(new DirectInputDevice(pddi, this)); //デバイスオブジェクトを作成

		return DIENUM_CONTINUE; //列挙を続ける
	}

	




	//コンストラクタ
	DirectInputDevice::DirectInputDevice(LPCDIDEVICEINSTANCE pddi, DirectInput *pDirectInputObj)
	{
		this->didi = *pddi;
		this->pDID8 = NULL;
		this->pDirectInputObj = pDirectInputObj;
		this->Enable = false;

		HRESULT hr;

		try
		{
			//DirectInput8デバイスを作成
			if (FAILED(hr = pDirectInputObj->GetpDI8()->CreateDevice(didi.guidInstance, &pDID8, NULL)))
			{
				tstring str;
				Direct3D::GetErrorStr(str, hr);
				str = TEXT("DirectInput8 オブジェクトの作成に失敗しました。") + str;
				throw Exception(str, __WFILE__, __LINE__, true);
			}

			//デバイスのデータフォーマットの設定
			if (FAILED(hr = pDID8->SetDataFormat(&c_dfDIJoystick)))
			{
				tstring str;
				Direct3D::GetErrorStr(str, hr);
				str = TEXT("DirectInput8デバイスのデータフォーマットの設定に失敗しました。") + str;
				throw Exception(str, __WFILE__, __LINE__, true);
			}

			//協調モードの設定
			if (FAILED(hr = pDID8->SetCooperativeLevel(pDirectInputObj->GethWnd(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))
			{
				tstring str;
				Direct3D::GetErrorStr(str, hr);
				str = TEXT("DirectInput8デバイスの協調モードの設定に失敗しました。") + str;
				throw Exception(str, __WFILE__, __LINE__, true);
			}

			//軸モードを絶対値モードに
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
				str = TEXT("DirectInput8デバイスのプロパティの設定に失敗しました。") + str;
				throw Exception(str, __WFILE__, __LINE__, true);
			}
		
			//軸の値の範囲設定
			//十字キーを押していないときが0になるように
			DIPROPRANGE dipr;
			ZeroMemory(&dipr, sizeof(DIPROPRANGE));
			dipr.diph.dwSize = sizeof(DIPROPRANGE);
			dipr.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			dipr.diph.dwObj	= DIJOFS_X; //X軸
			dipr.diph.dwHow	= DIPH_BYOFFSET;
			dipr.lMin = -1000;
			dipr.lMax = 1000;
			if (FAILED(hr = pDID8->SetProperty(DIPROP_RANGE, &dipr.diph)))
			{
				tstring str;
				Direct3D::GetErrorStr(str, hr);
				str = TEXT("DirectInput8デバイスのプロパティの設定に失敗しました。") + str;
				throw Exception(str, __WFILE__, __LINE__, true);
			}
			dipr.diph.dwObj = DIJOFS_Y; //Y軸
			if (FAILED(hr = pDID8->SetProperty(DIPROP_RANGE, &dipr.diph)))
			{
				tstring str;
				Direct3D::GetErrorStr(str, hr);
				str = TEXT("DirectInput8デバイスのプロパティの設定に失敗しました。") + str;
				throw Exception(str, __WFILE__, __LINE__, true);
			}

			//キー入力取得時に行うのでここでは不要？
			/*
			//入力を許可する
			if (FAILED(hr = pDID8->Acquire()))
			{
				tstring str;
				Direct3D::GetErrorStr(str, hr);
				str = TEXT("DirectInput8デバイスのアクセス権取得に失敗しました。") + str;
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

	//デストラクタ
	DirectInputDevice::~DirectInputDevice()
	{
		if (pDID8) //デバイスが作成できていた場合
		{
			pDID8->Unacquire(); //アクセス権を解放
			RELEASE(pDID8);
		}
	}

	//DIJOYSTATE構造体を得る
	DIJOYSTATE DirectInputDevice::GetDIJOYSTATE()
	{
		DIJOYSTATE dijs;
		ZeroMemory(&dijs, sizeof(DIJOYSTATE));

		if (!Enable)
			return dijs;

		HRESULT hr;
		if (SUCCEEDED(hr = pDID8->Acquire())) //入力を許可する
		{
			pDID8->Poll();
			pDID8->GetDeviceState(sizeof(DIJOYSTATE), &dijs);
		}

		return dijs;
	}
}

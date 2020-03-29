//DirectInput クラス
#pragma once
#define DIRECTINPUT_VERSION 0x0800 //dinput.hのインクルードより先にDirectInputのバージョンを定義しておく
#include <dinput.h>
#include <Windows.h>
#include <list>

#pragma comment (lib, "dinput8.lib")

namespace ho
{
	//前方宣言
	class DirectInputDevice;

	class DirectInput
	{
	public:
		DirectInput(HINSTANCE hInstance, HWND hWnd); //コンストラクタ
		~DirectInput(); //デストラクタ

		static BOOL CALLBACK DIEnumDeviceCallback(LPCDIDEVICEINSTANCE pddi, LPVOID pRef); //デバイス列挙用コールバック関数
		BOOL DIEnumDeviceLocal(LPCDIDEVICEINSTANCE pddi); //デバイス列挙用のローカル関数

		//アクセッサ
		LPDIRECTINPUT8 GetpDI8() { return pDI8; }
		HWND GethWnd() { return hWnd; }
		std::list<DirectInputDevice *> &GetlistpDirectInputDeviceObj() { return listpDirectInputDeviceObj; }
	private:
		LPDIRECTINPUT8 pDI8; //DirectInput8 インターフェースへのポインタ
		std::list<DirectInputDevice *> listpDirectInputDeviceObj; //デバイスオブジェクトへのポインタのリスト
		HWND hWnd;
	};

	class DirectInputDevice
	{
	public:
		DirectInputDevice(LPCDIDEVICEINSTANCE pddi, DirectInput *pDirectInputObj); //コンストラクタ
		~DirectInputDevice(); //デストラクタ

		DIDEVICEINSTANCE *Getpdidi() { return &didi; }
		DIJOYSTATE GetDIJOYSTATE(); //DIJOYSTATE構造体を得る
	private:
		DIDEVICEINSTANCE didi;
		DirectInput *pDirectInputObj;
		LPDIRECTINPUTDEVICE8 pDID8; //DirectInput8デバイスのインターフェースへのポインタ
		bool Enable;
	};
}

/*
プログラム内の初期化時に DirectInput オブジェクトを一つ作り、終了時に削除する。
この DirectInput オブジェクトは生成時に、自動的に各コントローラに対応する
DirectInputDevice オブジェクトを生成する。
これらはリストに格納されているので、DirectInput::GetlistpDirectInputDeviceObj() で
リストをいつでも取得できる。そして、DirectInputDevice::GetDIJOYSTATE() でコントローラの
状態を取得する。
*/
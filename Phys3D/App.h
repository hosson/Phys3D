//アプリケーション頂点クラス
#pragma once
#include <Windows.h>
#include <boost\shared_ptr.hpp>

//前方宣言
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
	App(); //コンストラクタ
	~App(); //デストラクタ

	int Run(const std::shared_ptr<App> &spAppObj, HINSTANCE hInstance); //実行

	//アクセッサ
	const std::shared_ptr<ho::D3D11::D3D11> &getspD3D11Obj() const { return spD3D11Obj; }
private:
	std::shared_ptr<ho::WindowManager> spWindowManagerObj; //ウィンドウ管理オブジェクトへのポインタ
	std::shared_ptr<ho::D3D11::D3D11> spD3D11Obj; //Direct3D11オブジェクトへのポインタ
	std::shared_ptr<ho::FrameControl> spFrameControlObj; //フレームレート制御オブジェクトへのポインタ
	std::shared_ptr<ho::MessageLoop> spMessageLoopObj; //メッセージループオブジェクトへのポインタ
	std::shared_ptr<AppCore> spAppCoreObj; //アプリケーションコアオブジェクトへのポインタ
};

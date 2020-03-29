#pragma once

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include <DirectXMath.h>

#include "ComPtr.hpp"
#include "tstring.h"
#include <boost/shared_ptr.hpp>

namespace ho
{
	class OrderChecker;
	class ErrorManager;

	namespace D3D11
	{
		class D3D11
		{
		public:
			D3D11(const int &Width, const int &Height); //コンストラクタ
			~D3D11(); //デストラクタ

			void Init(std::shared_ptr<D3D11> spD3D11Obj, HWND hWnd); //初期化

			void DrawFrameBegin(); //1フレームの描画開始
			void DrawFrameEnd(); //1フレームの描画終了

			static tstring GetErrorStr(const HRESULT &hr); //エラー文字列取得
			static void GetErrorStr(tstring &str, const HRESULT &hr); //エラー文字列取得

			//アクセッサ
			ho::ComPtr<ID3D11Device *> GetcpDevice() { return cpDevice; }
			ho::ComPtr<ID3D11DeviceContext *> GetcpDeviceContext() { return cpDeviceContext; }
			const int &getWidth() const { return Width; }
			const int &getHeight() const { return Height; }
		private:
			ho::ComPtr<ID3D11Device *> cpDevice; //デバイス
			ho::ComPtr<ID3D11DeviceContext *> cpDeviceContext; //デバイスコンテキスト
			ho::ComPtr<IDXGISwapChain *> cpSwapChain; //スワップチェイン
			ho::ComPtr<ID3D11Texture2D *> cpRTT; //レンダーターゲットテクスチャ（バックバッファ）
			ho::ComPtr<ID3D11RenderTargetView *> cpRTV; //レンダーターゲットビュー
			ho::ComPtr<ID3D11ShaderResourceView *> cpRTSRV; //レンダーターゲットシェーダリソースビュー
			DXGI_FORMAT DepthStencilFormat; //深度ステンシルバッファフォーマット
			ho::ComPtr<ID3D11Texture2D *> cpDST; //深度ステンシルテクスチャ
			ho::ComPtr<ID3D11DepthStencilView *> cpDSV; //深度ステンシルビュー 
			ho::ComPtr<ID3D11ShaderResourceView *> cpDSSRV; //深度ステンシルシェーダリソースビュー


			std::shared_ptr<OrderChecker> spDrawOrderCheckerObj; //描画関数呼び出し順序チェックオブジェクト

			const int Width; //横方向のピクセル数
			const int Height; //縦方向のピクセル数
		};
	}
}

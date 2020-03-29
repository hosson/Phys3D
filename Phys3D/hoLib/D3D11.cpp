#include "D3D11.h"
#include "tstring.h"
#include "OrderChecker.h"
#include "Error.h"


namespace ho
{
	namespace D3D11
	{
		//コンストラクタ
		D3D11::D3D11(const int &Width, const int &Height)
			: Width(Width), Height(Height)
		{}

		//デストラクタ
		D3D11::~D3D11()
		{
			//各種インターフェース解放
			this->cpDSSRV = NULL; //深度ステンシルシェーダリソースビュー解放
			this->cpDSV = NULL; //深度ステンシルビュー解放
			this->cpDST = NULL; //深度ステンシルテクスチャ解放
			this->cpRTSRV = NULL; //レンダーターゲットシェーダリソースビュー解放
			this->cpRTV = NULL; //レンダーターゲットビュー解放
			this->cpRTT = NULL; //レンダーターゲットテクスチャ（バックバッファ）解放
			this->cpSwapChain = NULL; //スワップチェイン解放
			this->cpDeviceContext = NULL; //デバイスコンテキスト解放
			this->cpDevice = NULL; //デバイス解放
		}

		//初期化
		void D3D11::Init(std::shared_ptr<D3D11> spD3D11Obj, HWND hWnd)
		{
			this->spDrawOrderCheckerObj.reset(new OrderChecker(2, ho::tstring(TEXT("Direct3D の1フレーム毎の開始と終了関数"))));

			HRESULT hr;

			D3D_FEATURE_LEVEL d3dfl;

			//スワップチェーン構造体の作成
			DXGI_SWAP_CHAIN_DESC sd;
			ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));
			sd.BufferCount = 2;															//2 = ダブルバッファリング、3 = トリプルバッファリング
			sd.BufferDesc.Width = this->Width;											//バッファの横ピクセル数
			sd.BufferDesc.Height = this->Height;										//バッファの縦ピクセル数
			sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;							//バッファのフォーマット
			sd.BufferDesc.RefreshRate.Numerator = 60;									//リフレッシュレートの分子
			sd.BufferDesc.RefreshRate.Denominator = 1;									//リフレッシュレートの分母
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;	//バッファの使い方
			sd.OutputWindow = hWnd;														//ウィンドウハンドル
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
			sd.Windowed = true;

			//D3D11 デバイス作成
			hr = D3D11CreateDeviceAndSwapChain(
				NULL,
				D3D_DRIVER_TYPE_HARDWARE,
				NULL,
				NULL,
				NULL,
				NULL,
				D3D11_SDK_VERSION,
				&sd,
				this->cpSwapChain.ToCreator(),
				this->cpDevice.ToCreator(),
				&d3dfl,
				this->cpDeviceContext.ToCreator()
			);
			if (FAILED(hr))
				ERR(true, D3D11::GetErrorStr(hr), __WFILE__, __LINE__);

			//バックバッファ取得
			hr = this->cpSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)this->cpRTT.ToCreator());
			if (FAILED(hr))
				ERR(true, D3D11::GetErrorStr(hr), __WFILE__, __LINE__);

			//レンダーターゲット生成
			hr = this->cpDevice->CreateRenderTargetView(this->cpRTT, NULL, this->cpRTV.ToCreator());
			if (FAILED(hr))
				ERR(true, D3D11::GetErrorStr(hr), __WFILE__, __LINE__);

			//シェーダリソースビュー生成
			hr = this->cpDevice->CreateShaderResourceView(this->cpRTT, NULL, this->cpRTSRV.ToCreator());
			if (FAILED(hr))
				ERR(true, D3D11::GetErrorStr(hr), __WFILE__, __LINE__);

			//深度ステンシルバッファ生成
			{
				this->DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
				DXGI_FORMAT TextureFormat = DXGI_FORMAT_R24G8_TYPELESS;
				DXGI_FORMAT ResourceFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

				//深度ステンシルテクスチャ生成時の構造体
				D3D11_TEXTURE2D_DESC td;
				ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
				td.Width = this->Width;
				td.Height = this->Height;
				td.MipLevels = 1;
				td.ArraySize = 1;
				td.Format = TextureFormat;
				td.SampleDesc.Count = 1;
				td.SampleDesc.Quality = 0;
				td.Usage = D3D11_USAGE_DEFAULT;
				td.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
				td.CPUAccessFlags = 0;
				td.MiscFlags = 0;

				//深度ステンシルテクスチャ生成
				hr = this->cpDevice->CreateTexture2D(&td, NULL, this->cpDST.ToCreator());
				if (FAILED(hr))
					ERR(true, D3D11::GetErrorStr(hr), __WFILE__, __LINE__);

				//深度ステンシルビュー生成時の構造体
				D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
				ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
				dsvd.Format = this->DepthStencilFormat;
				dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; //★マルチサンプル設定により変更が必要
				dsvd.Texture2D.MipSlice = 0;

				//深度ステンシルビュー生成
				hr = this->cpDevice->CreateDepthStencilView(this->cpDST, &dsvd, this->cpDSV.ToCreator());
				if (FAILED(hr))
					ERR(true, D3D11::GetErrorStr(hr), __WFILE__, __LINE__);

				//シェーダリソースビュー生成時の構造体
				D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
				ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
				srvd.Format = ResourceFormat;
				srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D; //★マルチサンプル設定により変更が必要
				srvd.Texture2D.MostDetailedMip = 0;
				srvd.Texture2D.MipLevels = 1;

				//深度ステンシルシェーダリソースビュー生成
				hr = this->cpDevice->CreateShaderResourceView(this->cpDST, &srvd, this->cpDSSRV.ToCreator());
				if (FAILED(hr))
					ERR(true, D3D11::GetErrorStr(hr), __WFILE__, __LINE__);
			}

			//デバイスコンテキストにレンダーターゲットを設定
			this->cpDeviceContext->OMSetRenderTargets(1, this->cpRTV, this->cpDSV);

			//ビューポート構造体の生成
			D3D11_VIEWPORT vp;
			ZeroMemory(&vp, sizeof(D3D11_VIEWPORT));
			vp.Width = (FLOAT)this->Width;
			vp.Height = (FLOAT)this->Height;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;

			//デバイスコンテキストにビューポートを設定
			this->cpDeviceContext->RSSetViewports(1, &vp);
		}

		//1フレームの描画開始
		void D3D11::DrawFrameBegin()
		{
			this->spDrawOrderCheckerObj->Check(0); //順番チェック

			FLOAT Color[] = { 0.0f, 0.0f, 0.5f, 0.0f }; //初期化色

			//初期化
			this->cpDeviceContext->ClearRenderTargetView(this->cpRTV, Color); //バックバッファ初期化
			this->cpDeviceContext->ClearDepthStencilView(this->cpDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0); //深度バッファ初期化

			return;
		}

		//1フレームの描画終了
		void D3D11::DrawFrameEnd()
		{
			this->spDrawOrderCheckerObj->Check(1); //順番チェック

			this->cpSwapChain->Present(0, 0); //バックバッファをフリップ

			return;
		}

		//エラー文字列取得
		tstring D3D11::GetErrorStr(const HRESULT &hr)
		{
			LPVOID string;

			//エラー表示文字列作成
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				hr,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)&string,
				0,
				NULL);

			tstring  str;

			if (string != NULL)
				str = (TCHAR *)string; //返却用バッファへ代入

			LocalFree(string);

			return str;
		}

		//エラー文字列取得
		void D3D11::GetErrorStr(tstring &str, const HRESULT &hr)
		{
			LPVOID string;

			//エラー表示文字列作成
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				hr,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)&string,
				0,
				NULL);

			if (string != NULL)
				str = (TCHAR *)string; //返却用バッファへ代入

			LocalFree(string);

			return;
		}



	}
}

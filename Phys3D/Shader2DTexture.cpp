#include "Shader2DTexture.h"
#include "hoLib\Error.h"

//コンストラクタ
Shader2DTexture::Shader2DTexture(const std::shared_ptr<ho::D3D11::D3D11> &spD3D11Obj)
: Shader_Base(spD3D11Obj)
{
	ho::tstring strFilename(TEXT("2DTexture.fx")); //シェーダのファイル名
	HRESULT hr;

	//頂点シェーダ生成
	{
		ho::ComPtr<ID3DBlob *> cpBlob;

		CompileShaderFromFile(strFilename, "VSFunc", "vs_4_0", cpBlob); //頂点シェーダコンパイル

		hr = this->spD3D11Obj->GetcpDevice()->CreateVertexShader(cpBlob->GetBufferPointer(), cpBlob->GetBufferSize(), NULL, this->cpVS.ToCreator()); //頂点シェーダ生成
		if (FAILED(hr))
			ERR(true, ho::D3D11::D3D11::GetErrorStr(hr), __WFILE__, __LINE__);

		//入力レイアウト設定
		{
			//入力レイアウト定義
			D3D11_INPUT_ELEMENT_DESC layout[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			UINT numElements = ARRAYSIZE(layout);

			//入力レイアウト生成
			hr = this->spD3D11Obj->GetcpDevice()->CreateInputLayout(
				layout,
				numElements,
				cpBlob->GetBufferPointer(),
				cpBlob->GetBufferSize(),
				this->cpIL.ToCreator()
				);

			if (FAILED(hr))
				ERR(true, ho::D3D11::D3D11::GetErrorStr(hr), __WFILE__, __LINE__);

			//入力アセンブラに入力レイアウトを設定
			this->spD3D11Obj->GetcpDeviceContext()->IASetInputLayout(this->cpIL);
		}

	}

	//ピクセルシェーダ生成
	{
		ho::ComPtr<ID3DBlob *> cpBlob;

		CompileShaderFromFile(strFilename, "PSFunc", "ps_4_0", cpBlob); //ピクセルシェーダコンパイル

		hr = this->spD3D11Obj->GetcpDevice()->CreatePixelShader(cpBlob->GetBufferPointer(), cpBlob->GetBufferSize(), NULL, this->cpPS.ToCreator()); //ピクセルシェーダ生成
		if (FAILED(hr))
			ERR(true, ho::D3D11::D3D11::GetErrorStr(hr), __WFILE__, __LINE__);
	}

	//頂点バッファの設定
	{
		//頂点の定義
		CustomVertex vertices[] = {
			{ DirectX::XMFLOAT3(-1.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
			{ DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
			{ DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
			{ DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
			{ DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
			{ DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },
		};

		//バッファ情報構造体
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(CustomVertex) * 6;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		//サブリソースの設定
		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
		sd.pSysMem = vertices;

		//頂点バッファ生成
		hr = this->spD3D11Obj->GetcpDevice()->CreateBuffer(&bd, &sd, this->cpVB.ToCreator());
		if (FAILED(hr))
			ERR(true, ho::D3D11::D3D11::GetErrorStr(hr), __WFILE__, __LINE__);


	}

	//テクスチャの生成
	{

		D3D11_TEXTURE2D_DESC td;
		ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
		td.Width = this->spD3D11Obj->getWidth();
		td.Height = this->spD3D11Obj->getHeight();
		td.MipLevels = 1;
		td.ArraySize = 1;
		td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		td.SampleDesc.Count = 1;
		td.SampleDesc.Quality = 0;
		td.Usage = D3D11_USAGE_DYNAMIC;
		td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		td.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		td.MiscFlags = 0;

		hr = this->spD3D11Obj->GetcpDevice()->CreateTexture2D(&td, NULL, this->cpTexture.ToCreator());
		if (FAILED(hr))
			ERR(true, ho::D3D11::D3D11::GetErrorStr(hr), __WFILE__, __LINE__);

		{
			D3D11_MAPPED_SUBRESOURCE ms;
			hr = this->spD3D11Obj->GetcpDeviceContext()->Map(this->cpTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms); //マップ開始
			if (FAILED(hr))
				ERR(true, ho::D3D11::D3D11::GetErrorStr(hr), __WFILE__, __LINE__);

			for (int i = 0; i < 640 * 480; i++)
				((unsigned int *)ms.pData)[i] = (0xff << 24) + ((rand() % 255) << 16);

			this->spD3D11Obj->GetcpDeviceContext()->Unmap(this->cpTexture, 0); //マップ終了
		}
	}

	//シェーダリソースビューの生成
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MostDetailedMip = 0;
		desc.Texture2D.MipLevels = 1;

		hr = this->spD3D11Obj->GetcpDevice()->CreateShaderResourceView(this->cpTexture, &desc, this->cpSRV.ToCreator());
		if (FAILED(hr))
			ERR(true, ho::D3D11::D3D11::GetErrorStr(hr), __WFILE__, __LINE__);
	}

	//サンプラの作成
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SAMPLER_DESC));
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = desc.AddressV = desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		desc.MaxLOD = D3D11_FLOAT32_MAX;

		hr = this->spD3D11Obj->GetcpDevice()->CreateSamplerState(&desc, this->cpSampler.ToCreator());
		if (FAILED(hr))
			ERR(true, ho::D3D11::D3D11::GetErrorStr(hr), __WFILE__, __LINE__);
	}


}

//デストラクタ
Shader2DTexture::~Shader2DTexture()
{}

//描画
void Shader2DTexture::Draw()
{
	//シェーダを設定
	this->spD3D11Obj->GetcpDeviceContext()->VSSetShader(this->cpVS, NULL, 0);
	this->spD3D11Obj->GetcpDeviceContext()->GSSetShader(NULL, NULL, 0);
	this->spD3D11Obj->GetcpDeviceContext()->PSSetShader(this->cpPS, NULL, 0);

	//テクスチャとサンプラを設定
	this->spD3D11Obj->GetcpDeviceContext()->PSSetShaderResources(0, 1, this->cpSRV);
	this->spD3D11Obj->GetcpDeviceContext()->PSSetSamplers(0, 1, this->cpSampler);

	//入力アセンブラに頂点バッファを設定
	UINT Stride = sizeof(CustomVertex);
	UINT Offset = 0;
	this->spD3D11Obj->GetcpDeviceContext()->IASetVertexBuffers(0, 1, this->cpVB, &Stride, &Offset);

	//プリミティブの種類を設定
	this->spD3D11Obj->GetcpDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//描画
	this->spD3D11Obj->GetcpDeviceContext()->Draw(6, 0);
}

//画像バッファをセット
void Shader2DTexture::setBuffer(void *p)
{
	D3D11_MAPPED_SUBRESOURCE ms;
	HRESULT hr = this->spD3D11Obj->GetcpDeviceContext()->Map(this->cpTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms); //マップ開始
	if (FAILED(hr))
		ERR(true, ho::D3D11::D3D11::GetErrorStr(hr), __WFILE__, __LINE__);

	memcpy(ms.pData, p, 4 * 640 * 480);

	this->spD3D11Obj->GetcpDeviceContext()->Unmap(this->cpTexture, 0); //マップ終了

	return;
}

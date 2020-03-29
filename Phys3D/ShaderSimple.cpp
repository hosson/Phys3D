#include "ShaderSimple.h"
#include "hoLib\Error.h"

using namespace DirectX;

//コンストラクタ
ShaderSimple::ShaderSimple(const std::shared_ptr<ho::D3D11::D3D11> &spD3D11Obj)
: Shader_Base(spD3D11Obj)
{
	ho::tstring strFilename(TEXT("Simple.fx")); //シェーダのファイル名
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
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
		}

	}

	//ジオメトリシェーダ生成
	{
		ho::ComPtr<ID3DBlob *> cpBlob;

		CompileShaderFromFile(strFilename, "GSFunc", "gs_4_0", cpBlob); //ジオメトリシェーダコンパイル

		hr = this->spD3D11Obj->GetcpDevice()->CreateGeometryShader(cpBlob->GetBufferPointer(), cpBlob->GetBufferSize(), NULL, this->cpGS.ToCreator()); //ジオメトリシェーダ生成
		if (FAILED(hr))
			ERR(true, ho::D3D11::D3D11::GetErrorStr(hr), __WFILE__, __LINE__);
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
				{ XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT3(0, 0, -1.0f) },
				{ XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(0.0f, 0.2f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0, -1.0f) },
				{ XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0, 0, -1.0f) },
		};

		//バッファ情報構造体
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(CustomVertex) * 3;
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

	//定数バッファの設定
	{
		//構造体の設定
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
		bd.ByteWidth = sizeof(ConstantBuffer);
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;

		//定数バッファ生成
		hr = this->spD3D11Obj->GetcpDevice()->CreateBuffer(&bd, NULL, this->cpCB.ToCreator());
		if (FAILED(hr))
			ERR(true, ho::D3D11::D3D11::GetErrorStr(hr), __WFILE__, __LINE__);
	}
}

//デストラクタ
ShaderSimple::~ShaderSimple()
{}

//描画
void ShaderSimple::Draw()
{
	//シェーダを設定
	this->spD3D11Obj->GetcpDeviceContext()->VSSetShader(this->cpVS, NULL, 0);
	this->spD3D11Obj->GetcpDeviceContext()->GSSetShader(this->cpGS, NULL, 0);
	this->spD3D11Obj->GetcpDeviceContext()->PSSetShader(this->cpPS, NULL, 0);

	//定数バッファの設定
	ConstantBuffer cb;
	{
		//カメラ設定
		DirectX::XMVECTOR CameraPos = DirectX::XMVectorSet(0.0f, 0.0f, -0.75f, 0.0f);
		DirectX::XMVECTOR CameraTarget = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR CameraUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		//ワールド行列を初期化
		cb.mtrxWorld = XMMatrixIdentity();

		//ビュー行列を設定
		cb.mtrxView = XMMatrixLookAtLH(CameraPos, CameraTarget, CameraUp);

		//アスペクト比を算出
		FLOAT AspectRatio = (FLOAT)this->spD3D11Obj->getWidth() / (FLOAT)this->spD3D11Obj->getHeight();

		//射影行列を設定
		cb.mtrxProj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, AspectRatio, 0.01f, 1000.0f);

		cb.vecLight = XMFLOAT3(0, 1.0f, 0); //ライト方向（光源へ向かう方向）
	}

	//サブリソースを更新
	this->spD3D11Obj->GetcpDeviceContext()->UpdateSubresource(this->cpCB, 0, NULL, &cb, 0, 0);

	//ジオメトリシェーダに定数バッファを設定
	this->spD3D11Obj->GetcpDeviceContext()->GSSetConstantBuffers(0, 1, this->cpCB);

	//入力アセンブラに入力レイアウトを設定
	this->spD3D11Obj->GetcpDeviceContext()->IASetInputLayout(this->cpIL);

	//入力アセンブラに頂点バッファを設定
	UINT Stride = sizeof(CustomVertex);
	UINT Offset = 0;
	this->spD3D11Obj->GetcpDeviceContext()->IASetVertexBuffers(0, 1, this->cpVB, &Stride, &Offset);

	//プリミティブの種類を設定
	this->spD3D11Obj->GetcpDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//描画
	this->spD3D11Obj->GetcpDeviceContext()->Draw(3, 0);

	DrawInit(); //描画によって変更されたデバイスの状態を元に戻す

	return;
}

//頂点情報を指定して描画
void ShaderSimple::DrawUp(CustomVertex *const pVertex, const unsigned int &Vertices, unsigned int *const pIndex, const unsigned int &Indices, const XMMATRIX &mtrxWorld)
{
	ho::ComPtr<ID3D11Buffer *> cpVertexBuffer = CreateVertexBuffer(pVertex, Vertices); //頂点バッファ生成

	ho::ComPtr<ID3D11Buffer *> cpIndexBuffer;
	if (Indices) //頂点バッファを使用する場合
		cpIndexBuffer = CreateIndexBuffer(pIndex, Indices); //インデックスバッファ生成

	//シェーダを設定
	this->spD3D11Obj->GetcpDeviceContext()->VSSetShader(this->cpVS, NULL, 0);
	this->spD3D11Obj->GetcpDeviceContext()->GSSetShader(this->cpGS, NULL, 0);
	this->spD3D11Obj->GetcpDeviceContext()->PSSetShader(this->cpPS, NULL, 0);

	//定数バッファの設定
	ConstantBuffer cb;
	{
		//カメラ設定
		DirectX::XMVECTOR CameraPos = DirectX::XMVectorSet(0.0f,30.0f, -40.0f, 0.0f);
		DirectX::XMVECTOR CameraTarget = DirectX::XMVectorSet(0.0f, 10.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR CameraUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		//ワールド行列は引数の mtrxWorld を使う
		cb.mtrxWorld = mtrxWorld;

		//ビュー行列を設定
		cb.mtrxView = XMMatrixLookAtLH(CameraPos, CameraTarget, CameraUp);

		//アスペクト比を算出
		FLOAT AspectRatio = (FLOAT)this->spD3D11Obj->getWidth() / (FLOAT)this->spD3D11Obj->getHeight();

		//射影行列を設定
		cb.mtrxProj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PI * 0.3f, AspectRatio, 0.01f, 1000.0f);

		cb.vecLight = XMFLOAT3(0.7f, 0.7f, 0); //ライト方向（光源へ向かう方向）
	}

	//サブリソースを更新
	this->spD3D11Obj->GetcpDeviceContext()->UpdateSubresource(this->cpCB, 0, NULL, &cb, 0, 0);

	//ジオメトリシェーダに定数バッファを設定
	this->spD3D11Obj->GetcpDeviceContext()->GSSetConstantBuffers(0, 1, this->cpCB);

	//入力アセンブラに入力レイアウトを設定
	this->spD3D11Obj->GetcpDeviceContext()->IASetInputLayout(this->cpIL);

	//入力アセンブラに頂点バッファを設定
	UINT Stride = sizeof(CustomVertex);
	UINT Offset = 0;
	this->spD3D11Obj->GetcpDeviceContext()->IASetVertexBuffers(0, 1, cpVertexBuffer, &Stride, &Offset);

	if (Indices) //インデックスバッファを使用する場合
	{
		//入力アセンブラにインデックスバッファを設定
		this->spD3D11Obj->GetcpDeviceContext()->IASetIndexBuffer(cpIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	}


	//プリミティブの種類を設定
	this->spD3D11Obj->GetcpDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//描画
	if (Indices) //インデックスバッファを使用する場合
		this->spD3D11Obj->GetcpDeviceContext()->DrawIndexed(Indices, 0, 0);
	else
		this->spD3D11Obj->GetcpDeviceContext()->Draw(Vertices, 0);

	DrawInit(); //描画によって変更されたデバイスの状態を元に戻す

	return;
}

//頂点バッファ生成
ho::ComPtr<ID3D11Buffer *> ShaderSimple::CreateVertexBuffer(CustomVertex *const pVertex, const unsigned int &Vertices)
{
	ho::ComPtr<ID3D11Buffer *> cpVertexBuffer; //頂点バッファ

	//バッファ情報構造体
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CustomVertex) * Vertices;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	//サブリソースの設定
	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
	sd.pSysMem = pVertex;

	//頂点バッファ生成
	HRESULT hr = this->spD3D11Obj->GetcpDevice()->CreateBuffer(&bd, &sd, cpVertexBuffer.ToCreator());
	if (FAILED(hr))
		ERR(true, ho::D3D11::D3D11::GetErrorStr(hr), __WFILE__, __LINE__);

	return cpVertexBuffer;
}

//インデックスバッファ生成
ho::ComPtr<ID3D11Buffer *> ShaderSimple::CreateIndexBuffer(unsigned int *const pIndex, const unsigned int &Indices)
{
	ho::ComPtr<ID3D11Buffer *> cpIndexBuffer; //インデックスバッファ

	//バッファ情報構造体
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(unsigned int) * Indices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	//サブリソースの設定
	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
	sd.pSysMem = pIndex;

	//頂点バッファ生成
	HRESULT hr = this->spD3D11Obj->GetcpDevice()->CreateBuffer(&bd, &sd, cpIndexBuffer.ToCreator());
	if (FAILED(hr))
		ERR(true, ho::D3D11::D3D11::GetErrorStr(hr), __WFILE__, __LINE__);

	return cpIndexBuffer;

}

//描画によって変更されたデバイスの状態を元に戻す
void ShaderSimple::DrawInit()
{
	//シェーダ設定を解除
	this->spD3D11Obj->GetcpDeviceContext()->VSSetShader(NULL, NULL, 0);
	this->spD3D11Obj->GetcpDeviceContext()->GSSetShader(NULL, NULL, 0);
	this->spD3D11Obj->GetcpDeviceContext()->PSSetShader(NULL, NULL, 0);

	return;
}
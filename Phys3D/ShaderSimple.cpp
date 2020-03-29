#include "ShaderSimple.h"
#include "hoLib\Error.h"

using namespace DirectX;

//�R���X�g���N�^
ShaderSimple::ShaderSimple(const std::shared_ptr<ho::D3D11::D3D11> &spD3D11Obj)
: Shader_Base(spD3D11Obj)
{
	ho::tstring strFilename(TEXT("Simple.fx")); //�V�F�[�_�̃t�@�C����
	HRESULT hr;

	//���_�V�F�[�_����
	{
		ho::ComPtr<ID3DBlob *> cpBlob;

		CompileShaderFromFile(strFilename, "VSFunc", "vs_4_0", cpBlob); //���_�V�F�[�_�R���p�C��

		hr = this->spD3D11Obj->GetcpDevice()->CreateVertexShader(cpBlob->GetBufferPointer(), cpBlob->GetBufferSize(), NULL, this->cpVS.ToCreator()); //���_�V�F�[�_����
		if (FAILED(hr))
			ERR(true, ho::D3D11::D3D11::GetErrorStr(hr), __WFILE__, __LINE__);

		//���̓��C�A�E�g�ݒ�
		{
			//���̓��C�A�E�g��`
			D3D11_INPUT_ELEMENT_DESC layout[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			UINT numElements = ARRAYSIZE(layout);

			//���̓��C�A�E�g����
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

	//�W�I���g���V�F�[�_����
	{
		ho::ComPtr<ID3DBlob *> cpBlob;

		CompileShaderFromFile(strFilename, "GSFunc", "gs_4_0", cpBlob); //�W�I���g���V�F�[�_�R���p�C��

		hr = this->spD3D11Obj->GetcpDevice()->CreateGeometryShader(cpBlob->GetBufferPointer(), cpBlob->GetBufferSize(), NULL, this->cpGS.ToCreator()); //�W�I���g���V�F�[�_����
		if (FAILED(hr))
			ERR(true, ho::D3D11::D3D11::GetErrorStr(hr), __WFILE__, __LINE__);
	}

	//�s�N�Z���V�F�[�_����
	{
		ho::ComPtr<ID3DBlob *> cpBlob;

		CompileShaderFromFile(strFilename, "PSFunc", "ps_4_0", cpBlob); //�s�N�Z���V�F�[�_�R���p�C��

		hr = this->spD3D11Obj->GetcpDevice()->CreatePixelShader(cpBlob->GetBufferPointer(), cpBlob->GetBufferSize(), NULL, this->cpPS.ToCreator()); //�s�N�Z���V�F�[�_����
		if (FAILED(hr))
			ERR(true, ho::D3D11::D3D11::GetErrorStr(hr), __WFILE__, __LINE__);
	}

	//���_�o�b�t�@�̐ݒ�
	{
		//���_�̒�`
		CustomVertex vertices[] = {
				{ XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT3(0, 0, -1.0f) },
				{ XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(0.0f, 0.2f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0, -1.0f) },
				{ XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0, 0, -1.0f) },
		};

		//�o�b�t�@���\����
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(CustomVertex) * 3;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		//�T�u���\�[�X�̐ݒ�
		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
		sd.pSysMem = vertices;

		//���_�o�b�t�@����
		hr = this->spD3D11Obj->GetcpDevice()->CreateBuffer(&bd, &sd, this->cpVB.ToCreator());
		if (FAILED(hr))
			ERR(true, ho::D3D11::D3D11::GetErrorStr(hr), __WFILE__, __LINE__);
	}

	//�萔�o�b�t�@�̐ݒ�
	{
		//�\���̂̐ݒ�
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
		bd.ByteWidth = sizeof(ConstantBuffer);
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;

		//�萔�o�b�t�@����
		hr = this->spD3D11Obj->GetcpDevice()->CreateBuffer(&bd, NULL, this->cpCB.ToCreator());
		if (FAILED(hr))
			ERR(true, ho::D3D11::D3D11::GetErrorStr(hr), __WFILE__, __LINE__);
	}
}

//�f�X�g���N�^
ShaderSimple::~ShaderSimple()
{}

//�`��
void ShaderSimple::Draw()
{
	//�V�F�[�_��ݒ�
	this->spD3D11Obj->GetcpDeviceContext()->VSSetShader(this->cpVS, NULL, 0);
	this->spD3D11Obj->GetcpDeviceContext()->GSSetShader(this->cpGS, NULL, 0);
	this->spD3D11Obj->GetcpDeviceContext()->PSSetShader(this->cpPS, NULL, 0);

	//�萔�o�b�t�@�̐ݒ�
	ConstantBuffer cb;
	{
		//�J�����ݒ�
		DirectX::XMVECTOR CameraPos = DirectX::XMVectorSet(0.0f, 0.0f, -0.75f, 0.0f);
		DirectX::XMVECTOR CameraTarget = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR CameraUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		//���[���h�s���������
		cb.mtrxWorld = XMMatrixIdentity();

		//�r���[�s���ݒ�
		cb.mtrxView = XMMatrixLookAtLH(CameraPos, CameraTarget, CameraUp);

		//�A�X�y�N�g����Z�o
		FLOAT AspectRatio = (FLOAT)this->spD3D11Obj->getWidth() / (FLOAT)this->spD3D11Obj->getHeight();

		//�ˉe�s���ݒ�
		cb.mtrxProj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, AspectRatio, 0.01f, 1000.0f);

		cb.vecLight = XMFLOAT3(0, 1.0f, 0); //���C�g�����i�����֌����������j
	}

	//�T�u���\�[�X���X�V
	this->spD3D11Obj->GetcpDeviceContext()->UpdateSubresource(this->cpCB, 0, NULL, &cb, 0, 0);

	//�W�I���g���V�F�[�_�ɒ萔�o�b�t�@��ݒ�
	this->spD3D11Obj->GetcpDeviceContext()->GSSetConstantBuffers(0, 1, this->cpCB);

	//���̓A�Z���u���ɓ��̓��C�A�E�g��ݒ�
	this->spD3D11Obj->GetcpDeviceContext()->IASetInputLayout(this->cpIL);

	//���̓A�Z���u���ɒ��_�o�b�t�@��ݒ�
	UINT Stride = sizeof(CustomVertex);
	UINT Offset = 0;
	this->spD3D11Obj->GetcpDeviceContext()->IASetVertexBuffers(0, 1, this->cpVB, &Stride, &Offset);

	//�v���~�e�B�u�̎�ނ�ݒ�
	this->spD3D11Obj->GetcpDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//�`��
	this->spD3D11Obj->GetcpDeviceContext()->Draw(3, 0);

	DrawInit(); //�`��ɂ���ĕύX���ꂽ�f�o�C�X�̏�Ԃ����ɖ߂�

	return;
}

//���_�����w�肵�ĕ`��
void ShaderSimple::DrawUp(CustomVertex *const pVertex, const unsigned int &Vertices, unsigned int *const pIndex, const unsigned int &Indices, const XMMATRIX &mtrxWorld)
{
	ho::ComPtr<ID3D11Buffer *> cpVertexBuffer = CreateVertexBuffer(pVertex, Vertices); //���_�o�b�t�@����

	ho::ComPtr<ID3D11Buffer *> cpIndexBuffer;
	if (Indices) //���_�o�b�t�@���g�p����ꍇ
		cpIndexBuffer = CreateIndexBuffer(pIndex, Indices); //�C���f�b�N�X�o�b�t�@����

	//�V�F�[�_��ݒ�
	this->spD3D11Obj->GetcpDeviceContext()->VSSetShader(this->cpVS, NULL, 0);
	this->spD3D11Obj->GetcpDeviceContext()->GSSetShader(this->cpGS, NULL, 0);
	this->spD3D11Obj->GetcpDeviceContext()->PSSetShader(this->cpPS, NULL, 0);

	//�萔�o�b�t�@�̐ݒ�
	ConstantBuffer cb;
	{
		//�J�����ݒ�
		DirectX::XMVECTOR CameraPos = DirectX::XMVectorSet(0.0f,30.0f, -40.0f, 0.0f);
		DirectX::XMVECTOR CameraTarget = DirectX::XMVectorSet(0.0f, 10.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR CameraUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		//���[���h�s��͈����� mtrxWorld ���g��
		cb.mtrxWorld = mtrxWorld;

		//�r���[�s���ݒ�
		cb.mtrxView = XMMatrixLookAtLH(CameraPos, CameraTarget, CameraUp);

		//�A�X�y�N�g����Z�o
		FLOAT AspectRatio = (FLOAT)this->spD3D11Obj->getWidth() / (FLOAT)this->spD3D11Obj->getHeight();

		//�ˉe�s���ݒ�
		cb.mtrxProj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PI * 0.3f, AspectRatio, 0.01f, 1000.0f);

		cb.vecLight = XMFLOAT3(0.7f, 0.7f, 0); //���C�g�����i�����֌����������j
	}

	//�T�u���\�[�X���X�V
	this->spD3D11Obj->GetcpDeviceContext()->UpdateSubresource(this->cpCB, 0, NULL, &cb, 0, 0);

	//�W�I���g���V�F�[�_�ɒ萔�o�b�t�@��ݒ�
	this->spD3D11Obj->GetcpDeviceContext()->GSSetConstantBuffers(0, 1, this->cpCB);

	//���̓A�Z���u���ɓ��̓��C�A�E�g��ݒ�
	this->spD3D11Obj->GetcpDeviceContext()->IASetInputLayout(this->cpIL);

	//���̓A�Z���u���ɒ��_�o�b�t�@��ݒ�
	UINT Stride = sizeof(CustomVertex);
	UINT Offset = 0;
	this->spD3D11Obj->GetcpDeviceContext()->IASetVertexBuffers(0, 1, cpVertexBuffer, &Stride, &Offset);

	if (Indices) //�C���f�b�N�X�o�b�t�@���g�p����ꍇ
	{
		//���̓A�Z���u���ɃC���f�b�N�X�o�b�t�@��ݒ�
		this->spD3D11Obj->GetcpDeviceContext()->IASetIndexBuffer(cpIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	}


	//�v���~�e�B�u�̎�ނ�ݒ�
	this->spD3D11Obj->GetcpDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//�`��
	if (Indices) //�C���f�b�N�X�o�b�t�@���g�p����ꍇ
		this->spD3D11Obj->GetcpDeviceContext()->DrawIndexed(Indices, 0, 0);
	else
		this->spD3D11Obj->GetcpDeviceContext()->Draw(Vertices, 0);

	DrawInit(); //�`��ɂ���ĕύX���ꂽ�f�o�C�X�̏�Ԃ����ɖ߂�

	return;
}

//���_�o�b�t�@����
ho::ComPtr<ID3D11Buffer *> ShaderSimple::CreateVertexBuffer(CustomVertex *const pVertex, const unsigned int &Vertices)
{
	ho::ComPtr<ID3D11Buffer *> cpVertexBuffer; //���_�o�b�t�@

	//�o�b�t�@���\����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CustomVertex) * Vertices;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	//�T�u���\�[�X�̐ݒ�
	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
	sd.pSysMem = pVertex;

	//���_�o�b�t�@����
	HRESULT hr = this->spD3D11Obj->GetcpDevice()->CreateBuffer(&bd, &sd, cpVertexBuffer.ToCreator());
	if (FAILED(hr))
		ERR(true, ho::D3D11::D3D11::GetErrorStr(hr), __WFILE__, __LINE__);

	return cpVertexBuffer;
}

//�C���f�b�N�X�o�b�t�@����
ho::ComPtr<ID3D11Buffer *> ShaderSimple::CreateIndexBuffer(unsigned int *const pIndex, const unsigned int &Indices)
{
	ho::ComPtr<ID3D11Buffer *> cpIndexBuffer; //�C���f�b�N�X�o�b�t�@

	//�o�b�t�@���\����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(unsigned int) * Indices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	//�T�u���\�[�X�̐ݒ�
	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
	sd.pSysMem = pIndex;

	//���_�o�b�t�@����
	HRESULT hr = this->spD3D11Obj->GetcpDevice()->CreateBuffer(&bd, &sd, cpIndexBuffer.ToCreator());
	if (FAILED(hr))
		ERR(true, ho::D3D11::D3D11::GetErrorStr(hr), __WFILE__, __LINE__);

	return cpIndexBuffer;

}

//�`��ɂ���ĕύX���ꂽ�f�o�C�X�̏�Ԃ����ɖ߂�
void ShaderSimple::DrawInit()
{
	//�V�F�[�_�ݒ������
	this->spD3D11Obj->GetcpDeviceContext()->VSSetShader(NULL, NULL, 0);
	this->spD3D11Obj->GetcpDeviceContext()->GSSetShader(NULL, NULL, 0);
	this->spD3D11Obj->GetcpDeviceContext()->PSSetShader(NULL, NULL, 0);

	return;
}
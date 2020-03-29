#include "D3D11.h"
#include "tstring.h"
#include "OrderChecker.h"
#include "Error.h"


namespace ho
{
	namespace D3D11
	{
		//�R���X�g���N�^
		D3D11::D3D11(const int &Width, const int &Height)
			: Width(Width), Height(Height)
		{}

		//�f�X�g���N�^
		D3D11::~D3D11()
		{
			//�e��C���^�[�t�F�[�X���
			this->cpDSSRV = NULL; //�[�x�X�e���V���V�F�[�_���\�[�X�r���[���
			this->cpDSV = NULL; //�[�x�X�e���V���r���[���
			this->cpDST = NULL; //�[�x�X�e���V���e�N�X�`�����
			this->cpRTSRV = NULL; //�����_�[�^�[�Q�b�g�V�F�[�_���\�[�X�r���[���
			this->cpRTV = NULL; //�����_�[�^�[�Q�b�g�r���[���
			this->cpRTT = NULL; //�����_�[�^�[�Q�b�g�e�N�X�`���i�o�b�N�o�b�t�@�j���
			this->cpSwapChain = NULL; //�X���b�v�`�F�C�����
			this->cpDeviceContext = NULL; //�f�o�C�X�R���e�L�X�g���
			this->cpDevice = NULL; //�f�o�C�X���
		}

		//������
		void D3D11::Init(std::shared_ptr<D3D11> spD3D11Obj, HWND hWnd)
		{
			this->spDrawOrderCheckerObj.reset(new OrderChecker(2, ho::tstring(TEXT("Direct3D ��1�t���[�����̊J�n�ƏI���֐�"))));

			HRESULT hr;

			D3D_FEATURE_LEVEL d3dfl;

			//�X���b�v�`�F�[���\���̂̍쐬
			DXGI_SWAP_CHAIN_DESC sd;
			ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));
			sd.BufferCount = 2;															//2 = �_�u���o�b�t�@�����O�A3 = �g���v���o�b�t�@�����O
			sd.BufferDesc.Width = this->Width;											//�o�b�t�@�̉��s�N�Z����
			sd.BufferDesc.Height = this->Height;										//�o�b�t�@�̏c�s�N�Z����
			sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;							//�o�b�t�@�̃t�H�[�}�b�g
			sd.BufferDesc.RefreshRate.Numerator = 60;									//���t���b�V�����[�g�̕��q
			sd.BufferDesc.RefreshRate.Denominator = 1;									//���t���b�V�����[�g�̕���
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;	//�o�b�t�@�̎g����
			sd.OutputWindow = hWnd;														//�E�B���h�E�n���h��
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
			sd.Windowed = true;

			//D3D11 �f�o�C�X�쐬
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

			//�o�b�N�o�b�t�@�擾
			hr = this->cpSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)this->cpRTT.ToCreator());
			if (FAILED(hr))
				ERR(true, D3D11::GetErrorStr(hr), __WFILE__, __LINE__);

			//�����_�[�^�[�Q�b�g����
			hr = this->cpDevice->CreateRenderTargetView(this->cpRTT, NULL, this->cpRTV.ToCreator());
			if (FAILED(hr))
				ERR(true, D3D11::GetErrorStr(hr), __WFILE__, __LINE__);

			//�V�F�[�_���\�[�X�r���[����
			hr = this->cpDevice->CreateShaderResourceView(this->cpRTT, NULL, this->cpRTSRV.ToCreator());
			if (FAILED(hr))
				ERR(true, D3D11::GetErrorStr(hr), __WFILE__, __LINE__);

			//�[�x�X�e���V���o�b�t�@����
			{
				this->DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
				DXGI_FORMAT TextureFormat = DXGI_FORMAT_R24G8_TYPELESS;
				DXGI_FORMAT ResourceFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

				//�[�x�X�e���V���e�N�X�`���������̍\����
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

				//�[�x�X�e���V���e�N�X�`������
				hr = this->cpDevice->CreateTexture2D(&td, NULL, this->cpDST.ToCreator());
				if (FAILED(hr))
					ERR(true, D3D11::GetErrorStr(hr), __WFILE__, __LINE__);

				//�[�x�X�e���V���r���[�������̍\����
				D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
				ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
				dsvd.Format = this->DepthStencilFormat;
				dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; //���}���`�T���v���ݒ�ɂ��ύX���K�v
				dsvd.Texture2D.MipSlice = 0;

				//�[�x�X�e���V���r���[����
				hr = this->cpDevice->CreateDepthStencilView(this->cpDST, &dsvd, this->cpDSV.ToCreator());
				if (FAILED(hr))
					ERR(true, D3D11::GetErrorStr(hr), __WFILE__, __LINE__);

				//�V�F�[�_���\�[�X�r���[�������̍\����
				D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
				ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
				srvd.Format = ResourceFormat;
				srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D; //���}���`�T���v���ݒ�ɂ��ύX���K�v
				srvd.Texture2D.MostDetailedMip = 0;
				srvd.Texture2D.MipLevels = 1;

				//�[�x�X�e���V���V�F�[�_���\�[�X�r���[����
				hr = this->cpDevice->CreateShaderResourceView(this->cpDST, &srvd, this->cpDSSRV.ToCreator());
				if (FAILED(hr))
					ERR(true, D3D11::GetErrorStr(hr), __WFILE__, __LINE__);
			}

			//�f�o�C�X�R���e�L�X�g�Ƀ����_�[�^�[�Q�b�g��ݒ�
			this->cpDeviceContext->OMSetRenderTargets(1, this->cpRTV, this->cpDSV);

			//�r���[�|�[�g�\���̂̐���
			D3D11_VIEWPORT vp;
			ZeroMemory(&vp, sizeof(D3D11_VIEWPORT));
			vp.Width = (FLOAT)this->Width;
			vp.Height = (FLOAT)this->Height;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;

			//�f�o�C�X�R���e�L�X�g�Ƀr���[�|�[�g��ݒ�
			this->cpDeviceContext->RSSetViewports(1, &vp);
		}

		//1�t���[���̕`��J�n
		void D3D11::DrawFrameBegin()
		{
			this->spDrawOrderCheckerObj->Check(0); //���ԃ`�F�b�N

			FLOAT Color[] = { 0.0f, 0.0f, 0.5f, 0.0f }; //�������F

			//������
			this->cpDeviceContext->ClearRenderTargetView(this->cpRTV, Color); //�o�b�N�o�b�t�@������
			this->cpDeviceContext->ClearDepthStencilView(this->cpDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0); //�[�x�o�b�t�@������

			return;
		}

		//1�t���[���̕`��I��
		void D3D11::DrawFrameEnd()
		{
			this->spDrawOrderCheckerObj->Check(1); //���ԃ`�F�b�N

			this->cpSwapChain->Present(0, 0); //�o�b�N�o�b�t�@���t���b�v

			return;
		}

		//�G���[������擾
		tstring D3D11::GetErrorStr(const HRESULT &hr)
		{
			LPVOID string;

			//�G���[�\��������쐬
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
				str = (TCHAR *)string; //�ԋp�p�o�b�t�@�֑��

			LocalFree(string);

			return str;
		}

		//�G���[������擾
		void D3D11::GetErrorStr(tstring &str, const HRESULT &hr)
		{
			LPVOID string;

			//�G���[�\��������쐬
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				hr,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)&string,
				0,
				NULL);

			if (string != NULL)
				str = (TCHAR *)string; //�ԋp�p�o�b�t�@�֑��

			LocalFree(string);

			return;
		}



	}
}

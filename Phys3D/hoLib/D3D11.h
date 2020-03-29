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
			D3D11(const int &Width, const int &Height); //�R���X�g���N�^
			~D3D11(); //�f�X�g���N�^

			void Init(std::shared_ptr<D3D11> spD3D11Obj, HWND hWnd); //������

			void DrawFrameBegin(); //1�t���[���̕`��J�n
			void DrawFrameEnd(); //1�t���[���̕`��I��

			static tstring GetErrorStr(const HRESULT &hr); //�G���[������擾
			static void GetErrorStr(tstring &str, const HRESULT &hr); //�G���[������擾

			//�A�N�Z�b�T
			ho::ComPtr<ID3D11Device *> GetcpDevice() { return cpDevice; }
			ho::ComPtr<ID3D11DeviceContext *> GetcpDeviceContext() { return cpDeviceContext; }
			const int &getWidth() const { return Width; }
			const int &getHeight() const { return Height; }
		private:
			ho::ComPtr<ID3D11Device *> cpDevice; //�f�o�C�X
			ho::ComPtr<ID3D11DeviceContext *> cpDeviceContext; //�f�o�C�X�R���e�L�X�g
			ho::ComPtr<IDXGISwapChain *> cpSwapChain; //�X���b�v�`�F�C��
			ho::ComPtr<ID3D11Texture2D *> cpRTT; //�����_�[�^�[�Q�b�g�e�N�X�`���i�o�b�N�o�b�t�@�j
			ho::ComPtr<ID3D11RenderTargetView *> cpRTV; //�����_�[�^�[�Q�b�g�r���[
			ho::ComPtr<ID3D11ShaderResourceView *> cpRTSRV; //�����_�[�^�[�Q�b�g�V�F�[�_���\�[�X�r���[
			DXGI_FORMAT DepthStencilFormat; //�[�x�X�e���V���o�b�t�@�t�H�[�}�b�g
			ho::ComPtr<ID3D11Texture2D *> cpDST; //�[�x�X�e���V���e�N�X�`��
			ho::ComPtr<ID3D11DepthStencilView *> cpDSV; //�[�x�X�e���V���r���[ 
			ho::ComPtr<ID3D11ShaderResourceView *> cpDSSRV; //�[�x�X�e���V���V�F�[�_���\�[�X�r���[


			std::shared_ptr<OrderChecker> spDrawOrderCheckerObj; //�`��֐��Ăяo�������`�F�b�N�I�u�W�F�N�g

			const int Width; //�������̃s�N�Z����
			const int Height; //�c�����̃s�N�Z����
		};
	}
}

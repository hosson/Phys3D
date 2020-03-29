//DirectX Graphics�̃N���X

#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9tex.h>
#include <dxerr9.h>

#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "dxerr9.lib")

#include "Common.h"
#include "PT.h"
#include "ComPtr.hpp"
#include "tstring.h"

namespace ho {
	//�O���錾
	class D3DVERTEX;
	class D3DVERTEX_2D;
	class GetCharTexture;
	class DrawString;
	class D3DResourceManager;
	class MeshManager;
	class DrawDebug;

	class Direct3D
	{
	public:
		struct VIEWPORTCULLING //������J�����O�v�Z�p�\����
		{
			D3DXVECTOR3 vecNear[4], vecFar[4]; //�������8�_�̍��W
			D3DXVECTOR3 vecNFar, vecNNear, vecNLeft, vecNRight, vecNTop, vecNBottom; //�@���x�N�g��
		};
	public:
		Direct3D(HWND hWnd, LOGFONT &lf); //�R���X�g���N�^
		virtual ~Direct3D(); //�f�X�g���N�^
		bool Init(HWND hWnd, LOGFONT &lf); //������
		bool CreateObj1(); //�I�u�W�F�N�g�쐬1�i�f�o�C�X�쐬�O�j
		bool CreateObj2(LOGFONT lf); //�I�u�W�F�N�g�쐬2�i�f�o�C�X�쐬��j
		bool InitD3D(HWND hWnd); //Direct3D�̏�����
		bool InitRender(); //�����_�[�̏�����

		bool BeginDraw(); //�`��J�n�i�Ăяo���K�{�j
		void Draw(); //�t���[�����̕`��
		bool EndDraw(); //�`��I���i�Ăяo���K�{�j
		void OnLostDevice(); //���X�g��Ԃ̃f�o�C�X��ōs�������i���\�[�X����j
		bool DeviceReset(); //���X�g��Ԃ̃f�o�C�X�����Z�b�g

		void SetRenderTargetBackBuffer(); //�����_�[�^�[�Q�b�g���o�b�N�o�b�t�@�ɐݒ肷��

		virtual D3DXMATRIX SetWorldMatrix(D3DXMATRIX *pmtrxWorld); //���[���h�s���ݒ肷��
		virtual D3DMATERIAL9 SetMaterial(D3DMATERIAL9 *pMaterial); //�}�e���A����ݒ肷��


		DWORD GetUsableLightIndexNum(); //SetLight()�Ŏg����i�����ɂȂ��Ă���j�ŏ��̔ԍ����擾����
		void GetBillBoardMatrix(D3DXMATRIX &out, D3DXMATRIX &mtrxTransScale, const D3DXMATRIX *pmtrxView = NULL); //�r���[�s�񂩂�r���{�[�h�`��p�̃��[���h�s����擾
		::D3DXVECTOR3 GetScreenToWorld(::D3DXVECTOR3 &vecScreen, ::D3DXMATRIX *pmtrxView = NULL); //�C�ӂ̃r���[�s��̃X�N���[�����W���烏�[���h���W�𓾂�
		::D3DXVECTOR3 GetWorldToScreen(::D3DXVECTOR3 &vecWorld, ::D3DXMATRIX *pmtrxView = NULL); //���[���h���W����X�N���[�����W�𓾂�

		virtual PT<int> GetptClientSize() { return PT<int>(GetClientWidth(), GetClientHeight()); }
		virtual int GetClientWidth() const { return 640; } //�N���C�A���g�̈�̕��𓾂�
		virtual int GetClientHeight() const { return 480; } //�N���C�A���g�̈�̍����𓾂�

		static void GetErrorStr(tstring &str, HRESULT hr); //HRESULT�l����G���[��������擾����

		//�A�N�Z�b�T
		LPDIRECT3DDEVICE9 GetpD3DDevice();
		DrawString *GetpDrawStringObj() { return pDrawStringObj; }
		D3DResourceManager *GetpD3DResourceManagerObj() { return pD3DResourceManagerObj; }
		MeshManager *GetpMeshManagerObj() { return pMeshManagerObj; }
		bool GetDeviceLost() { return DeviceLost; }
		DrawDebug *GetpDrawDebugObj() { return pDrawDebugObj; }
	protected:
		LPDIRECT3D9 pD3D;
		LPDIRECT3DDEVICE9 pD3DDevice;
		DrawString *pDrawStringObj; //�����`��I�u�W�F�N�g�ւ̃|�C���^
		D3DResourceManager *pD3DResourceManagerObj; //Direct3D ���\�[�X�Ǘ��I�u�W�F�N�g�ւ̃|�C���^
		MeshManager *pMeshManagerObj; //���b�V���Ǘ��I�u�W�F�N�g�ւ̃|�C���^
		bool DeviceLost; //�f�o�C�X���X�g��Ԃ��ǂ���
		D3DPRESENT_PARAMETERS d3dpp; //�f�o�C�X�쐬���̃p�����[�^
		DrawDebug *pDrawDebugObj; //�f�o�b�O�`��I�u�W�F�N�g�ւ̃|�C���^

		virtual bool GetWindowed() { return true; } //�E�B���h�E���[�h���ǂ���
		virtual bool GetVSYNC() const { return true; } //VSYNC���������邩�ǂ���
	};

}

/*
���t���[���̕`��̍ŏ��� DrawBegin() ���A�Ō�� DrawEnd() ��K���Ăяo���B
InitRender() �� Draw() �͖{�̑��œK�؂Ȃ��̂�����Ďg���B
*/

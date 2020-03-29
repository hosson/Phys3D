#include "Direct3D.h"
#include "Debug.h"
#include <tchar.h>
#include "CharTexture.h"
#include "DrawString.h"
#include "Mesh.h"
#include "Math.h"
#include "D3DResource.h"
#include "DrawDebug.h"
#include "StringTextureCache.h"
#include "LibError.h"

namespace ho
{
	Direct3D::Direct3D(HWND hWnd, LOGFONT &lf) //�R���X�g���N�^
	{
		this->pD3D = NULL;
		this->pD3DDevice = NULL;
		this->pD3DResourceManagerObj = NULL;
		this->pMeshManagerObj = NULL;
		this->pDrawStringObj = NULL;
		this->pDrawDebugObj = NULL;

		this->DeviceLost = false;

		if (hWnd) //�p������ꍇ�� hWnd �� NULL ��n���悤�ɂ��A�p����̃R���X�g���N�^���炱�̏��������s��
			Init(hWnd, lf);
	}

	Direct3D::~Direct3D() //�f�X�g���N�^
	{
		SDELETE(pDrawDebugObj);
		SDELETE(pDrawStringObj);
		SDELETE(pMeshManagerObj);
		SDELETE(pD3DResourceManagerObj);
		RELEASE(pD3DDevice);
		RELEASE(pD3D);
	}

	//������
	bool Direct3D::Init(HWND hWnd, LOGFONT &lf)
	{
		CreateObj1(); //�I�u�W�F�N�g�쐬1
		if (InitD3D(hWnd)) //Direct3D�̏�����
		{
			InitRender(); //�����_�[��������
			CreateObj2(lf); //�I�u�W�F�N�g�쐬2
		} else {
			return false;
		}

		return true;
	}

	//�I�u�W�F�N�g�쐬1�i�f�o�C�X�쐬�O�j
	bool Direct3D::CreateObj1() 
	{
		pD3DResourceManagerObj = new D3DResourceManager(); //Direct3D ���\�[�X�Ǘ��I�u�W�F�N�g���쐬
		pMeshManagerObj = new MeshManager(); //���b�V���Ǘ��I�u�W�F�N�g���쐬

		return TRUE;
	}

	//�I�u�W�F�N�g�쐬2�i�f�o�C�X�쐬��j
	bool Direct3D::CreateObj2(LOGFONT lf) 
	{
		pDrawStringObj = new DrawString(this, pD3DDevice, &lf, false); //������`��I�u�W�F�N�g���쐬
		pDrawDebugObj = new ho::DrawDebug(this); //�f�o�b�O�`��I�u�W�F�N�g���쐬

		return TRUE;
	}

	bool Direct3D::InitD3D(HWND hWnd) //Direct3D�̏�����
	{
		HRESULT hr;

		try
		{
			//�`��Ώۂ̃E�B���h�E�n���h���擾
			if (!hWnd)
				throw Exception(TEXT("NULL �̃E�B���h�E�n���h�����n����܂����B"), __WFILE__, __LINE__, false);

			//Direct3D�I�u�W�F�N�g�̍쐬
			if (!(pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
				throw Exception(TEXT("Direct3D �I�u�W�F�N�g�̍쐬�Ɏ��s���܂����B"), __WFILE__, __LINE__, false);

			ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

			//���݂̃f�B�X�v���C���[�h�̎擾
			D3DDISPLAYMODE mode;
			if (FAILED(hr = pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode)))
			{
				tstring str;
				GetErrorStr(str, hr);
				str = TEXT("�f�B�X�v���C���[�h�̎擾�Ɏ��s���܂����B") + str;
				throw Exception(str, __WFILE__, __LINE__, false);
			}
			d3dpp.BackBufferFormat = mode.Format;

			//�f�o�C�X�쐬�̂��߂̃p�����[�^��ݒ�
			d3dpp.BackBufferWidth = GetClientWidth();							//��ʕ�
			d3dpp.BackBufferHeight = GetClientHeight();							//��ʍ���
			//d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;							//�o�b�N�o�b�t�@�̌`��
			d3dpp.BackBufferCount = 1;											//�o�b�N�o�b�t�@�̐�
			d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;							//�\�����@
			d3dpp.EnableAutoDepthStencil = TRUE;								//Z�o�b�t�@�L��
			d3dpp.AutoDepthStencilFormat = D3DFMT_D16;							//Z�o�b�t�@�̃t�H�[�}�b�g
			d3dpp.hDeviceWindow = hWnd;											//�`��Ώۂ̃E�B���h�E�n���h��
			d3dpp.Windowed = GetWindowed();										//�E�B���h�E���[�h
			d3dpp.Flags = NULL;														
			if (GetVSYNC())
				d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;			//VSYNC��������
			else
				d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;			//VSYNC�������Ȃ�

			//�f�o�C�X�̍쐬
			if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &pD3DDevice)))
				if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pD3DDevice)))
					if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &pD3DDevice)))
						if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pD3DDevice)))
							throw Exception(TEXT("�f�o�C�X�̍쐬�Ɏ��s���܂����B"), __WFILE__, __LINE__, false);
		}
		catch(Exception &e)
		{
			ER(e.strMsg.c_str(), e.pFile, e.Line, e.Continue);
			return false;
		}

		return true;
	}

	//�����_�[�̊e��ݒ���s���B�����̓\�t�g�ɂ���ē��e������ւ��̂ł���͈��B
	bool Direct3D::InitRender()
	{
		//�A���t�@�u�����f�B���O
		pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
		pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW); //�J�����O
		pD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE); //Z�o�b�t�@�����O�L��

		//�e�N�X�`���t�B���^�̐ݒ�
		pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		pD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		return true;
		/*
		pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1); //���_�t�H�[�}�b�g���w��


		//�����ϊ��̐ݒ�
		D3DXMATRIX matProj;
		D3DXMatrixPerspectiveFovLH(&matProj, 0.8f , float(GetClientWidth()) / float(GetClientHeight()), 2.0f, 800.0f);
		pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

		//View ���W�̐ݒ�
		D3DXMATRIX matView;
		D3DXMatrixLookAtLH(&matView, &D3DXVECTOR3(2.8f, 79.3f, -54.0f), &D3DXVECTOR3(-1.0f, -4.0f, 0.0f), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
		pD3DDevice->SetTransform(D3DTS_VIEW, &matView);




		//�e�N�X�`���t�B���^�̐ݒ�
		pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		pD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	/*	pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		pD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		

		//�}�e���A���ݒ�
		D3DMATERIAL9 mat;
		ZeroMemory(&mat, sizeof(D3DMATERIAL9));
		mat.Diffuse.r = mat.Diffuse.g = mat.Diffuse.b = mat.Diffuse.a = 1.0f; //Diffuse
		mat.Ambient.r = mat.Ambient.g =  mat.Ambient.b =  mat.Ambient.a = 0.1f; //Ambient
		mat.Specular.r =  mat.Specular.g = mat.Specular.b = mat.Specular.a = 1.0f; //Specular
		mat.Power = 10.0f; //Specular power
		mat.Emissive.r = mat.Emissive.g = mat.Emissive.b = 	mat.Emissive.a = 0.0f; //Emissive
		SetMaterial(&mat);
	
		//���C�g�ݒ�
		D3DLIGHT9 light;
		ZeroMemory(&light, sizeof(D3DLIGHT9));

		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Diffuse.r = light.Diffuse.g = light.Diffuse.b = light.Diffuse.a = 1.0f;
		light.Specular.r = light.Specular.g = light.Specular.b = light.Specular.a = 0.7f;
		light.Ambient.r = light.Ambient.g = light.Ambient.b = light.Ambient.a = 0.0f;
		light.Position = D3DXVECTOR3(0.0f, 50.0f, -10.0f); //���̈ʒu
		light.Direction = D3DXVECTOR3(0, -1.0f, 0); //���̕���
		light.Range = 200.0f; //���̓͂�����
		light.Attenuation0 = 0.3f; 
		light.Attenuation1 = 0.0002f; //�����ɔ�Ⴕ������
		light.Attenuation2 = 0.1f; //����2��ɔ�Ⴕ������

		pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
		pD3DDevice->SetLight(0, &light);
		pD3DDevice->LightEnable(0, TRUE);

		pD3DDevice->SetRenderState(D3DRS_AMBIENT, 0xffffffff); //�S�̂̊���
		*/
		return true;
	}

	//�`��J�n�i�Ăяo���K�{�j
	bool Direct3D::BeginDraw()
	{
		if (DeviceLost) //�f�o�C�X���X�g��Ԃ̏ꍇ
			return DeviceReset(); //�f�o�C�X���Z�b�g�����݂�

		return true;
	}

	//�t���[�����̕`��i���j
	void Direct3D::Draw() 
	{
		if (!pD3DDevice)
			return;

		pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0xff, 0xff, 0xff), 1.0f, 0); //�o�b�N�o�b�t�@�̃N���A

		pD3DDevice->BeginScene(); //�`��J�n

		/*������DrawPrimitiveUp�Ȃǂ̕`�惁�\�b�h���L�q*/

		pD3DDevice->EndScene(); //�`��I��

		return;
	}

	//�`��I���i�Ăяo���K�{�j
	bool Direct3D::EndDraw() 
	{
		if (!pD3DDevice)
			return true;

		if (pD3DDevice->Present(NULL, NULL, NULL, NULL) == D3DERR_DEVICELOST) //�f�o�C�X���X�g�̏ꍇ
		{
			OnLostDevice(); //���\�[�X����Ȃ�
			return false;
		}

		pDrawStringObj->FrameProcess(); //������L���b�V���̍œK���Ȃ�
		pD3DResourceManagerObj->Optimize(); //���\�[�X�Ǘ��N���X�̍œK��

		return true;
	}

	//���[���h�s���ݒ肷��
	D3DXMATRIX Direct3D::SetWorldMatrix(D3DXMATRIX *pmtrxWorld)
	{
		D3DXMATRIX mtrxReturn;
		pD3DDevice->GetTransform(D3DTS_WORLD, &mtrxReturn);

		pD3DDevice->SetTransform(D3DTS_WORLD, pmtrxWorld);

		return mtrxReturn; //�f�o�C�X�ɐݒ肳��Ă����Â��s�񂪕Ԃ�
	}

	//�}�e���A����ݒ肷��
	D3DMATERIAL9 Direct3D::SetMaterial(D3DMATERIAL9 *pMaterial)
	{
		D3DMATERIAL9 OldMaterial;
		pD3DDevice->GetMaterial(&OldMaterial);
		pD3DDevice->SetMaterial(pMaterial);

		return OldMaterial;
	}

	//LPDIRECT3DDEVICE9�𓾂�
	LPDIRECT3DDEVICE9 Direct3D::GetpD3DDevice() 
	{
		if (!pD3DDevice)
		{
			ER(TEXT("Direct3D�f�o�C�X�̎擾���o���Ă��܂���B"), __WFILE__, __LINE__, false);
			return NULL;
		}

		return pD3DDevice;
	}

	//SetLight()�Ŏg����i�����ɂȂ��Ă���j�ŏ��̔ԍ����擾����
	DWORD Direct3D::GetUsableLightIndexNum() 
	{
		const DWORD Max = 255; //��������ő�l
		BOOL Enable;

		for (DWORD i = 0; i < Max; i++)
		{
			pD3DDevice->GetLightEnable(i, &Enable); //�擾
			if (!Enable) //�����������ꍇ
				return i;
		}

		return 0; //������Ȃ������ꍇ�͂Ƃ肠����0��Ԃ�
	}

	//���݂̃r���[�s�񂩂�r���{�[�h�`��p�̃��[���h�s����擾
	void Direct3D::GetBillBoardMatrix(D3DXMATRIX &out, D3DXMATRIX &mtrxTransScale, const D3DXMATRIX *pmtrxView) 
	{
		D3DXMATRIX mtrxView;

		if (!pmtrxView) //�r���[�s�񂪓n����Ă��Ȃ��ꍇ
			pD3DDevice->GetTransform(D3DTS_VIEW, &mtrxView); //�f�o�C�X���猻�݂̃r���[�s����擾
		else
			mtrxView = *pmtrxView;

		D3DXMatrixInverse(&mtrxView, NULL, &mtrxView); //�r���[�s����t�s��ɕϊ�

		//�r���[�}�g���b�N�X�̋t�s��́A�I�u�W�F�N�g�̌���(��])�������l������̂ŕ��s�ړ��͖����ɂ���B
		mtrxView._41 = 0.0f;
		mtrxView._42 = 0.0f;
		mtrxView._43 = 0.0f;
   
		D3DXMATRIX mtrxScale, mtrxTrans; //�g��k���s��A���s�ړ��s��

		//�X�P�[�����O�}�g���b�N�X�𒊏o����
		D3DXMatrixIdentity(&mtrxScale);
		mtrxScale._11 = mtrxTransScale._11;
		mtrxScale._22 = mtrxTransScale._22;
		mtrxScale._33 = mtrxTransScale._33;

		//���s�ړ��}�g���b�N�X�𒊏o����
		D3DXMatrixIdentity(&mtrxTrans);
		mtrxTrans._41 = mtrxTransScale._41;
		mtrxTrans._42 = mtrxTransScale._42;
		mtrxTrans._43 = mtrxTransScale._43;

		//�X�P�[�����O���āA��]���āA���s�ړ�����B
		out = mtrxScale * mtrxView * mtrxTrans;

		return;
	}

	//�C�ӂ̃r���[�s��̃X�N���[�����W���烏�[���h���W�𓾂�
	::D3DXVECTOR3 Direct3D::GetScreenToWorld(::D3DXVECTOR3 &vecScreen, ::D3DXMATRIX *pmtrxView) 
	{
		D3DXMATRIX mtrxView, mtrxProj, mtrxViewPort; //���ˍs��A�r���[�|�[�g�s��

		if (!pmtrxView) //�r���[�s�񂪓n����Ă��Ȃ��ꍇ
			pD3DDevice->GetTransform(D3DTS_VIEW, &mtrxView); //�f�o�C�X���猻�݂̃r���[�s����擾
		else
			mtrxView = *pmtrxView;

		//�e�s����擾
		pD3DDevice->GetTransform(D3DTS_PROJECTION, &mtrxProj); //���ˍs��擾
		D3DVIEWPORT9 vp;
		pD3DDevice->GetViewport(&vp); //�r���[�|�[�g�擾
		D3DXMatrixIdentity(&mtrxViewPort);
		mtrxViewPort._11 = vp.Width * 0.5f;
		mtrxViewPort._22 = vp.Height * -0.5f;
		mtrxViewPort._41 = vp.Width * 0.5f;
		mtrxViewPort._42 = vp.Height * 0.5f;
		mtrxViewPort._33 = vp.MaxZ - vp.MinZ;
		mtrxViewPort._43 = vp.MinZ;

		//�e�s����t�s��ɕϊ�
		D3DXMATRIX mtrxViewInv; //�r���[�s��̋t�s��
		D3DXMatrixInverse(&mtrxViewInv, NULL, &mtrxView);
		D3DXMatrixInverse(&mtrxProj, NULL, &mtrxProj);
		D3DXMatrixInverse(&mtrxViewPort, NULL, &mtrxViewPort);

		//�ϊ�
		D3DXVECTOR3 vecReturn;
		D3DXMATRIX mtrx = mtrxViewPort * mtrxProj * mtrxViewInv;
		D3DXVec3TransformCoord(&vecReturn, &vecScreen, &mtrx);

		return vecReturn;
	}

	//���[���h���W����X�N���[�����W�𓾂�
	::D3DXVECTOR3 Direct3D::GetWorldToScreen(::D3DXVECTOR3 &vecWorld, ::D3DXMATRIX *pmtrxView) 
	{
		PT<int> ptScreen;

		D3DXMATRIX mtrxView, mtrxProj, mtrxViewPort; //�r���[�s��A���ˍs��A�r���[�|�[�g�s��

		//�e�s����擾

		if (!pmtrxView) //�r���[�s�񂪓n����Ă��Ȃ��ꍇ
			pD3DDevice->GetTransform(D3DTS_VIEW, &mtrxView); //�f�o�C�X���猻�݂̃r���[�s����擾
		else
			mtrxView = *pmtrxView;

		pD3DDevice->GetTransform(D3DTS_PROJECTION, &mtrxProj); //���ˍs��擾
		D3DVIEWPORT9 vp;
		pD3DDevice->GetViewport(&vp); //�r���[�|�[�g�擾
		D3DXMatrixIdentity(&mtrxViewPort);
		mtrxViewPort._11 = vp.Width * 0.5f;
		mtrxViewPort._22 = vp.Height * -0.5f;
		mtrxViewPort._41 = vp.Width * 0.5f;
		mtrxViewPort._42 = vp.Height * 0.5f;
		mtrxViewPort._33 = vp.MaxZ - vp.MinZ;
		mtrxViewPort._43 = vp.MinZ;

		//�ϊ�
		D3DXVECTOR3 vecReturn;
		D3DXMATRIX mtrx = mtrxView * mtrxProj * mtrxViewPort;
		D3DXVec3TransformCoord(&vecReturn, &vecWorld, &mtrx);

		return vecReturn;
	}

	//HRESULT�l����G���[��������擾����
	void Direct3D::GetErrorStr(tstring &str, HRESULT hr) 
	{
		ho::tPrint(str, TEXT("HRESULT=%08X (%s) ErrStr=%s Desc=%s\n"), hr, (FAILED(hr)) ? TEXT("FAILED") : TEXT("SUCCEEDED"), DXGetErrorString9(hr), DXGetErrorDescription9(hr));

		return;
	}

	//���X�g��Ԃ̃f�o�C�X��ōs�������i���\�[�X����j
	void Direct3D::OnLostDevice() 
	{
		DeviceLost = true;

		pD3DResourceManagerObj->OnLostDevice(); //���X�g�f�o�C�X���� D3DPOOL_DEFAULT �t���O�𔺂��č쐬���ꂽ���\�[�X�Ȃǂ����ׂĉ������
		pDrawStringObj->OnLostDevice(); //���X�g�f�o�C�X���̃��\�[�X���

		DeviceReset(); //���\�[�X�����̕��A����

		return;
	}

	//���X�g��Ԃ̃f�o�C�X�����Z�b�g
	bool Direct3D::DeviceReset()
	{
		if (D3DERR_DEVICENOTRESET == pD3DDevice->TestCooperativeLevel()) //�f�o�C�X�����A�ł���ꍇ
		{
			//d3dpp.Windowed = TRUE;
			HRESULT hr;
			if (FAILED(hr = pD3DDevice->Reset(&d3dpp))) //�f�o�C�X���Z�b�g
			{
				ho::tstring str;
				GetErrorStr(str, hr); //�G���[������擾

				return false; //���Z�b�g���s
			}

			pD3DResourceManagerObj->OnResetDevice(pD3DDevice); //���Z�b�g���ꂽ�f�o�C�X��ŁA D3DPOOL_DEFAULT �t���O�ō쐬���ꂽ���\�[�X���č쐬����
			DeviceLost = false;

			InitRender(); //�����_�[�ݒ�̏�����

			return true; //���Z�b�g����
		}

		return false; //���Z�b�g���s
	}

	//�����_�[�^�[�Q�b�g���o�b�N�o�b�t�@�ɐݒ肷��
	void Direct3D::SetRenderTargetBackBuffer() 
	{
		ho::ComPtr<LPDIRECT3DSURFACE9> pSurface;
		pD3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, pSurface.ToCreator()); //�o�b�N�o�b�t�@�̃T�[�t�F�C�X���擾
		pD3DDevice->SetRenderTarget(0, pSurface);
	
		return;
	}
}

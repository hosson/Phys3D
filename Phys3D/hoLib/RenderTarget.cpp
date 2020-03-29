#include "RenderTarget.h"
#include "Debug.h"
#include <d3dx9.h>
#include "Direct3D.h"
#include "D3DResource.h"
#include "../ShaderVertex.h"

namespace ho
{
	//�R���X�g���N�^
	RenderTarget::RenderTarget(Direct3D *pDirect3DObj, const PT<int> &ptSize, D3DFORMAT fmt, DWORD VTColor)
	{
		this->pDirect3DObj = pDirect3DObj;
		this->ptSize = ptSize;
		this->VTColor = VTColor;

		//�ʏ�̃����_�����O�p�̃e�N�X�`���쐬
		HRESULT hr;
		if (FAILED(hr = D3DXCreateTexture(pDirect3DObj->GetpD3DDevice(), ptSize.x, ptSize.y,
			1, D3DUSAGE_RENDERTARGET, fmt, D3DPOOL_DEFAULT, cpTex.ToCreator())))
		{
			tstring str;
			ho::Direct3D::GetErrorStr(str, hr);
			str = TEXT("�����_�����O�p�̃e�N�X�`���쐬�Ɏ��s���܂����B") + str;
			ERR(str.c_str(), __WFILE__, __LINE__, false);
		} else {
			pDirect3DObj->GetpD3DResourceManagerObj()->Regist(new ho::Texture(cpTex)); //���\�[�X�Ǘ��I�u�W�F�N�g�ɓo�^

			//�e�N�X�`���̓��e��������
			const LPDIRECT3DDEVICE9 pD3DDevice = pDirect3DObj->GetpD3DDevice();
			ho::ComPtr<LPDIRECT3DSURFACE9> cpOldSurface;
			pD3DDevice->GetRenderTarget(0, cpOldSurface.ToCreator());
			this->SetRenderTarget(0);
			pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
			pD3DDevice->SetRenderTarget(0, cpOldSurface);
		}
	}

	//�f�X�g���N�^
	RenderTarget::~RenderTarget()
	{
	}

	//�f�o�C�X�̃����_�[�^�[�Q�b�g�Ɏw��
	void RenderTarget::SetRenderTarget(int Index) 
	{
		if (!cpTex.GetpInterface())
			return;

		ComPtr<LPDIRECT3DSURFACE9> cpSurface;
		cpTex->GetSurfaceLevel(0, cpSurface.ToCreator()); //�e�N�X�`������T�[�t�F�C�X���擾

		pDirect3DObj->GetpD3DDevice()->SetRenderTarget(Index, cpSurface); //�w�肳�ꂽ�C���f�b�N�X�Ƀ����_�[�^�[�Q�b�g��ݒ�

		return;
	}

	//�e�N�X�`�������݂̃����_�[�^�[�Q�b�g�ɕ`��
	void RenderTarget::Render()
	{
		const LPDIRECT3DDEVICE9 pD3DDevice = pDirect3DObj->GetpD3DDevice();

		//�����_�[�^�[�Q�b�g�̃T�C�Y�ƃ����_�����O���̃J���[���璸�_�����쐬
		D3DVERTEX_2D vt[4];
		vt[0].SetVertex(0, 0, 0, VTColor, 0, 0);
		vt[1].SetVertex((float)ptSize.x, 0, 0, VTColor, 1.0f, 0);
		vt[2].SetVertex(0, (float)ptSize.y, 0, VTColor, 0, 1.0f);
		vt[3].SetVertex((float)ptSize.x, (float)ptSize.y, 0, VTColor, 1.0f, 1.0f);
		D3DVERTEX_2D::Offset(vt, 4);


		ho::ComPtr<IDirect3DBaseTexture9 *> cpOldTexture;
		pD3DDevice->GetTexture(0, cpOldTexture.ToCreator()); //�Â��e�N�X�`����ۑ�
		DWORD OldFVF;
		pD3DDevice->GetFVF(&OldFVF); //�Â����_�t�H�[�}�b�g��ۑ�

		pD3DDevice->SetTexture(0, cpTex); //�e�N�X�`����ݒ�
		pD3DDevice->SetFVF(ho::D3DVERTEX_2D::FVF);
		pD3DDevice->BeginScene();
		pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, ho::D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(ho::D3DVERTEX_2D));
		pD3DDevice->EndScene();

		pD3DDevice->SetFVF(OldFVF); //�Â����_�t�H�[�}�b�g�𕜌�
		pD3DDevice->SetTexture(0, cpOldTexture); //�Â��e�N�X�`���𕜌�

		return;
	}



	//�e�N�X�`�������݂̃����_�[�^�[�Q�b�g�ɕ`��
	void RenderTarget::Render(ho::PT<int> &ptSize, DWORD *pVtColor)
	{
		const LPDIRECT3DDEVICE9 pD3DDevice = pDirect3DObj->GetpD3DDevice();

		//�����_�[�^�[�Q�b�g�̃T�C�Y�ƃ����_�����O���̃J���[���璸�_�����쐬
		D3DVERTEX_2D vt[4];
		const DWORD NowVtColor = [&]()->DWORD //���_�J���[��I��
		{
			if (pVtColor) //�����Ŏw�肳��Ă���ꍇ
				return *pVtColor;
			return VTColor;
		}();
		vt[0].SetVertex(0, 0, 0, NowVtColor, 0, 0);
		vt[1].SetVertex((float)ptSize.x, 0, 0, NowVtColor, 1.0f, 0);
		vt[2].SetVertex(0, (float)ptSize.y, 0, NowVtColor, 0, 1.0f);
		vt[3].SetVertex((float)ptSize.x, (float)ptSize.y, 0, NowVtColor, 1.0f, 1.0f);
		D3DVERTEX_2D::Offset(vt, 4);


		ho::ComPtr<IDirect3DBaseTexture9 *> cpOldTexture;
		pD3DDevice->GetTexture(0, cpOldTexture.ToCreator()); //�Â��e�N�X�`����ۑ�
		DWORD OldFVF;
		pD3DDevice->GetFVF(&OldFVF); //�Â����_�t�H�[�}�b�g��ۑ�

		pD3DDevice->SetTexture(0, cpTex); //�e�N�X�`����ݒ�
		pD3DDevice->SetFVF(ho::D3DVERTEX_2D::FVF);
		pD3DDevice->BeginScene();
		pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, ho::D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(ho::D3DVERTEX_2D));
		pD3DDevice->EndScene();

		pD3DDevice->SetFVF(OldFVF); //�Â����_�t�H�[�}�b�g�𕜌�
		pD3DDevice->SetTexture(0, cpOldTexture); //�Â��e�N�X�`���𕜌�

		return;
	}

	//�C�ӂ̃����_�[�^�[�Q�b�g�փe�N�X�`����`��
	void RenderTarget::Render(RenderTarget *pRenderTargetObj, int Index, DWORD *pVtColor, LPDIRECT3DVERTEXDECLARATION9 pvd)
	{
		const LPDIRECT3DDEVICE9 pD3DDevice = pDirect3DObj->GetpD3DDevice();

		//�����_�[�^�[�Q�b�g�̃T�C�Y�ƃ����_�����O���̃J���[���璸�_�����쐬
		DWORD NowVtColor;
		if (pVtColor) //�����Œ��_�J���[���w�肳��Ă���ꍇ
			NowVtColor = *pVtColor;
		else
			NowVtColor = this->VTColor;
		D3DVERTEX_2D vt[4];
		vt[0].SetVertex(0, 0, 0, NowVtColor, 0, 0);
		vt[1].SetVertex((float)pRenderTargetObj->GetptSize().x, 0, 0, NowVtColor, 1.0f, 0);
		vt[2].SetVertex(0, (float)pRenderTargetObj->GetptSize().y, 0, NowVtColor, 0, 1.0f);
		vt[3].SetVertex((float)pRenderTargetObj->GetptSize().x, (float)pRenderTargetObj->GetptSize().y, 0, NowVtColor, 1.0f, 1.0f);
		D3DVERTEX_2D::Offset(vt, 4);


		pRenderTargetObj->SetRenderTarget(Index); //�����̃I�u�W�F�N�g�������_�[�^�[�Q�b�g�ɐݒ�

		ho::ComPtr<IDirect3DBaseTexture9 *> cpOldTexture;
		pD3DDevice->GetTexture(0, cpOldTexture.ToCreator()); //�Â��e�N�X�`����ۑ�
		DWORD OldFVF;
		pD3DDevice->GetFVF(&OldFVF); //�Â����_�t�H�[�}�b�g��ۑ�

		pD3DDevice->SetTexture(0, cpTex); //�e�N�X�`����ݒ�

		if (pvd) //���_�錾���w�肳��Ă���ꍇ
			pD3DDevice->SetVertexDeclaration(pvd);
		else
			pD3DDevice->SetFVF(ho::D3DVERTEX_2D::FVF);
	
		pD3DDevice->BeginScene();
		pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, ho::D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(ho::D3DVERTEX_2D));
		pD3DDevice->EndScene();

		pD3DDevice->SetFVF(OldFVF); //�Â����_�t�H�[�}�b�g�𕜌�
		pD3DDevice->SetTexture(0, cpOldTexture); //�Â��e�N�X�`���𕜌�

		return;
	}

	//�e�N�X�`����C�ӂ̐F�Ŗ��߂�
	void RenderTarget::Fill(const DWORD &Color)
	{
		ho::ComPtr<LPDIRECT3DSURFACE9> cpSurface;
		cpTex->GetSurfaceLevel(0, cpSurface.ToCreator()); //�e�N�X�`���̃T�[�t�F�C�X���擾

		pDirect3DObj->GetpD3DDevice()->ColorFill(cpSurface, NULL, Color);

		return;
	}
}

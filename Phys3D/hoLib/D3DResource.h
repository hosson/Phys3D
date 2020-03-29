//Direct3D ��ň������\�[�X�̃N���X
#pragma once
#include "Common.h"
#include "hoLib.h"
#include <list>
#include "Debug.h"
#include "Direct3D.h"
#include "ComPtr.hpp"
#include "LibError.h"

namespace ho
{
	class D3DResource;

	//���\�[�X�}�l�[�W��
	class D3DResourceManager
	{
	public:
		D3DResourceManager(); //�R���X�g���N�^
		~D3DResourceManager(); //�f�X�g���N�^
	
		void Regist(D3DResource *pD3DResourceObj); //���\�[�X��o�^����
		void Erase(void **ppInterface); //���\�[�X���폜����
		void Optimize(); //�œK���i���g�p���\�[�X���폜�j
		void OnLostDevice(); //���X�g�f�o�C�X���� D3DPOOL_DEFAULT �t���O�ō쐬���ꂽ���\�[�X�����
		void OnResetDevice(LPDIRECT3DDEVICE9 pD3DDevice); //���Z�b�g���ꂽ�f�o�C�X��ŁA D3DPOOL_DEFAULT �t���O�ō쐬���ꂽ���\�[�X���č쐬����
	private:
		std::list<D3DResource *> listpD3DResourceObj;
	};


	//�e�탊�\�[�X�̊��ƂȂ�N���X
	class D3DResource
	{
	public:
		D3DResource() {} //�R���X�g���N�^
		virtual ~D3DResource() {} //�f�X�g���N�^

		virtual ULONG GetRefCount() { return 0; } //COM�|�C���^�̎Q�ƃJ�E���^�̒l�𓾂�
		virtual void OnLostDevice() {} //���X�g�f�o�C�X���� D3DPOOL_DEFAULT �t���O�ō쐬���ꂽ���\�[�X�����
		virtual void OnResetDevice(LPDIRECT3DDEVICE9 pD3DDevice) {} //���Z�b�g���ꂽ�f�o�C�X��ŁA D3DPOOL_DEFAULT �t���O�ō쐬���ꂽ���\�[�X���č쐬����
		virtual void **Getpp() { return NULL; }
	};


	//�e�N�X�`���N���X
	class Texture : public D3DResource
	{
	public:
		Texture(ComPtr<LPDIRECT3DTEXTURE9> &cpTexture) //�R���X�g���N�^
		{
			this->cpTexture = cpTexture;
			if (cpTexture.GetpInterface())
				(cpTexture)->GetLevelDesc(0, &Desc); //�T�[�t�F�C�X�����擾
		}
		~Texture() //�f�X�g���N�^
		{
			if (this->cpTexture.GetRefCount() > 1) //�Q�Ɛ���2�ȏ�i���̃N���X�ȊO�ɂ�COM�|�C���^�����݂��Ă���ꍇ�j
				ER(TEXT("�܂��������Ă��Ȃ�COM�|�C���^�����݂��܂��B"),  __WFILE__, __LINE__, true);
		} 

		ULONG GetRefCount() { return this->cpTexture.GetRefCount(); } //COM�|�C���^�̎Q�ƃJ�E���^�̒l�𓾂�
		
		//���X�g�f�o�C�X���� D3DPOOL_DEFAULT �t���O�ō쐬���ꂽ���\�[�X�Ȃǂ����
		void OnLostDevice()
		{
			if (Desc.Pool == D3DPOOL_DEFAULT) //�쐬���� D3DPOOL_DEFAULT �̃t���O�𔺂��Ă����ꍇ
			{
				ComPtr<LPDIRECT3DTEXTURE9> cpEmptyTexture; //��̃e�N�X�`�����쐬
				this->cpTexture.Swap(cpEmptyTexture); //����ւ���
			}

			return;
		}

		//���Z�b�g���ꂽ�f�o�C�X��ŁA D3DPOOL_DEFAULT �t���O�ō쐬���ꂽ���\�[�X���č쐬
		void OnResetDevice(LPDIRECT3DDEVICE9 pD3DDevice)
		{
			if (Desc.Pool == D3DPOOL_DEFAULT)
			{
				ComPtr<LPDIRECT3DTEXTURE9> cpNewTexture;
				HRESULT hr = pD3DDevice->CreateTexture(Desc.Width, Desc.Height, 1, Desc.Usage, Desc.Format, Desc.Pool, cpNewTexture.ToCreator(), NULL);
				if (FAILED(hr))
				{
					tstring str;
					ho::Direct3D::GetErrorStr(str, hr);
					ER(str.c_str(),  __WFILE__, __LINE__, true);
				} else {
					this->cpTexture.Swap(cpNewTexture);
				}
			}

			return;
		}

		void **Getpp()
		{
			return (void **)cpTexture.GetppInterface();
		}

	private:
		ComPtr<LPDIRECT3DTEXTURE9> cpTexture;
		D3DSURFACE_DESC Desc; //�T�[�t�F�C�X���
	};


	//�T�[�t�F�C�X�N���X
	class Surface : public D3DResource
	{
	public:
		Surface(ComPtr<LPDIRECT3DSURFACE9> &cpSurface) //�R���X�g���N�^
		{
			this->cpSurface = cpSurface;
			if (cpSurface.GetpInterface())
				cpSurface->GetDesc(&Desc);
		}
		~Surface() //�f�X�g���N�^
		{
			if (this->cpSurface.GetRefCount() > 1) //�Q�Ɛ���2�ȏ�i���̃N���X�ȊO�ɂ�COM�|�C���^�����݂��Ă���ꍇ�j
				ER(TEXT("�܂��������Ă��Ȃ�COM�|�C���^�����݂��܂��B"), __WFILE__, __LINE__, true);
		} 

		ULONG GetRefCount() { return this->cpSurface.GetRefCount(); } //COM�|�C���^�̎Q�ƃJ�E���^�̒l�𓾂�

		//���X�g�f�o�C�X���� D3DPOOL_DEFAULT �t���O�ō쐬���ꂽ���\�[�X�����
		void OnLostDevice()
		{
			if (Desc.Pool == D3DPOOL_DEFAULT) //�쐬���� D3DPOOL_DEFAULT �̃t���O�𔺂��Ă����ꍇ
			{
				ComPtr<LPDIRECT3DSURFACE9> cpEmptySurface; //��̃T�[�t�F�C�X���쐬
				this->cpSurface.Swap(cpEmptySurface); //����ւ���
			}

			return;
		} 

		//���Z�b�g���ꂽ�f�o�C�X��ŁA D3DPOOL_DEFAULT �t���O�ō쐬���ꂽ���\�[�X���č쐬
		void OnResetDevice(LPDIRECT3DDEVICE9 pD3DDevice)
		{
			if (Desc.Pool == D3DPOOL_DEFAULT) //�쐬���� D3DPOOL_DEFAULT �̃t���O�𔺂��Ă����ꍇ
			{
				if (Desc.Usage == D3DUSAGE_DEPTHSTENCIL) //�[�x�X�e���V���o�b�t�@�̏ꍇ
				{
					pD3DDevice->CreateDepthStencilSurface(Desc.Width, Desc.Height, Desc.Format, Desc.MultiSampleType, Desc.MultiSampleQuality, TRUE,  this->cpSurface.ToCreator(), NULL);
				} else if (Desc.Usage == D3DUSAGE_RENDERTARGET) //�����_�[�^�[�Q�b�g�̏ꍇ
				{
					pD3DDevice->CreateRenderTarget(Desc.Width, Desc.Height, Desc.Format, Desc.MultiSampleType, Desc.MultiSampleQuality, Desc.Pool, this->cpSurface.ToCreator(), NULL);
				} else { //����ȊO�i�ʏ�̃T�[�t�F�C�X�j�̏ꍇ
					pD3DDevice->CreateOffscreenPlainSurface(Desc.Width, Desc.Height, Desc.Format, Desc.Pool, this->cpSurface.ToCreator(), NULL);
				}
			}

			return;
		}

	private:
		ComPtr<LPDIRECT3DSURFACE9> cpSurface;
		D3DSURFACE_DESC Desc; //�T�[�t�F�C�X���
	};


	//�X�v���C�g�N���X
	class Sprite : public D3DResource
	{
	public:
		Sprite(ComPtr<LPD3DXSPRITE> &cpSprite) { this->cpSprite = cpSprite;	} //�R���X�g���N�^
		~Sprite() //�f�X�g���N�^
		{
			if (this->cpSprite.GetRefCount() > 1) //�Q�Ɛ���2�ȏ�i���̃N���X�ȊO�ɂ�COM�|�C���^�����݂��Ă���ꍇ�j
				ER(TEXT("�܂��������Ă��Ȃ�COM�|�C���^�����݂��܂��B"), __WFILE__, __LINE__, true);
		} 

	
		//���X�g�f�o�C�X���� D3DPOOL_DEFAULT �t���O�ō쐬���ꂽ���\�[�X�Ȃǂ����
		void OnLostDevice()
		{
			cpSprite->OnLostDevice();

			return;
		}

		//���Z�b�g���ꂽ�f�o�C�X��ŁA D3DPOOL_DEFAULT �t���O�ō쐬���ꂽ���\�[�X���č쐬
		void OnResetDevice(LPDIRECT3DDEVICE9 pD3DDevice)
		{
			cpSprite->OnResetDevice();

			return;
		}

		ULONG GetRefCount() { return this->cpSprite.GetRefCount(); } //COM�|�C���^�̎Q�ƃJ�E���^�̒l�𓾂�
		void **Getpp() { return (void **)cpSprite.GetppInterface(); }
	private:
		ComPtr<LPD3DXSPRITE> cpSprite;
	};

	//�t�H���g�N���X
	class Font : public D3DResource
	{
	public:
		Font(ComPtr<LPD3DXFONT> &cpFont) { this->cpFont = cpFont; } //�R���X�g���N�^
		~Font() //�f�X�g���N�^
		{
			if (this->cpFont.GetRefCount() > 1) //�Q�Ɛ���2�ȏ�i���̃N���X�ȊO�ɂ�COM�|�C���^�����݂��Ă���ꍇ�j
				ER(TEXT("�܂��������Ă��Ȃ�COM�|�C���^�����݂��܂��B"), __WFILE__, __LINE__, true);
		} 
	
		//���X�g�f�o�C�X���� D3DPOOL_DEFAULT �t���O�ō쐬���ꂽ���\�[�X�Ȃǂ����
		void OnLostDevice()
		{
			cpFont->OnLostDevice();

			return;
		}

		//���Z�b�g���ꂽ�f�o�C�X��ŁA D3DPOOL_DEFAULT �t���O�ō쐬���ꂽ���\�[�X���č쐬
		void OnResetDevice(LPDIRECT3DDEVICE9 pD3DDevice)
		{
			cpFont->OnResetDevice();

			return;
		}

		ULONG GetRefCount() { return this->cpFont.GetRefCount(); } //COM�|�C���^�̎Q�ƃJ�E���^�̒l�𓾂�
		void **Getpp() { return (void **)cpFont.GetppInterface(); }
	private:
		ComPtr<LPD3DXFONT> cpFont;
	};

	//�G�t�F�N�g�N���X
	class Effect : public D3DResource
	{
	public:
		Effect(ComPtr<LPD3DXEFFECT> &cpEffect) { this->cpEffect = cpEffect; } //�R���X�g���N�^
		~Effect() //�f�X�g���N�^
		{
			if (this->cpEffect.GetRefCount() > 1) //�Q�Ɛ���2�ȏ�i���̃N���X�ȊO�ɂ�COM�|�C���^�����݂��Ă���ꍇ�j
				ER(TEXT("�܂��������Ă��Ȃ�COM�|�C���^�����݂��܂��B"), __WFILE__, __LINE__, true);
		} 
	
		//���X�g�f�o�C�X���� D3DPOOL_DEFAULT �t���O�ō쐬���ꂽ���\�[�X�Ȃǂ����
		void OnLostDevice()
		{
			cpEffect->OnLostDevice();

			return;
		}

		//���Z�b�g���ꂽ�f�o�C�X��ŁA D3DPOOL_DEFAULT �t���O�ō쐬���ꂽ���\�[�X���č쐬
		void OnResetDevice(LPDIRECT3DDEVICE9 pD3DDevice)
		{
			cpEffect->OnResetDevice();

			return;
		}

		ULONG GetRefCount() { return this->cpEffect.GetRefCount(); } //COM�|�C���^�̎Q�ƃJ�E���^�̒l�𓾂�
		void **Getpp() { return (void **)cpEffect.GetppInterface(); }
	private:
		ComPtr<LPD3DXEFFECT> cpEffect;
	};

}

/*�g����

��������
Direct3D �N���X�Ȃǂ� Direct3D ���Ǘ�����N���X�Ȃǂ� D3DResourceManager �N���X��
�I�u�W�F�N�g������������1�����쐬����B
�v���O�����I���̃^�C�~���O�ł��� D3DResourceManager �N���X�͍폜����B

�����\�[�X�쐬
�e�N�X�`���Ȃǂ��쐬���邽�т� Texture �Ȃǂ̔h���N���X�𐶐�����
D3DResourceManager::Regist() ���\�b�h�œo�^����B

�����\�[�X�폜
�폜�� D3DResourceManager::Optimize() ���\�b�h���Ăяo�����Ƃɂ���čs����̂ŁA
����𖈃t���[�����ȂǂŌĂяo���B
���̂Ƃ��A Texture �Ȃǂ̍쐬����COM�|�C���^�����̏ꏊ�Ɏc���Ă���ƍ폜����Ȃ��B

*/

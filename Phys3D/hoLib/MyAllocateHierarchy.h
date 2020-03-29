#pragma once
#include <d3dx9.h>
#include "Common.h"
#include <d3d9.h>

namespace ho {
	//�e�t���[���Ƀ��[���h�ϊ��p�̍s���ǉ������N���X
	class D3DXFRAME_WORLD : public D3DXFRAME
	{
	public:
		D3DXMATRIX WorldTransMatrix; //�`�掞�̃��[���h�ϊ��p�s��
		D3DXMATRIX InitTransMatrix; //������Ԃ̕ϊ��s��

		DWORD GetNumFacesReflect(); //�ʂ̐��̍��v���擾����ċA�֐�
		D3DXFRAME_WORLD *FindFrame(LPSTR str); //���O����t���[������������ċA�֐�
	};

	//�}�e���A���\���̂Ƀe�N�X�`���ւ̃|�C���^��ǉ������N���X
	/*class D3DXMATERIAL_TEXTURE : public D3DXMATERIAL
	{
	public:
		LPDIRECT3DTEXTURE9 pD3DTexture;
	};*/

	//ID3DXAllocateHierarchy�N���X���p�������N���X
	class MyAllocateHierarchy : public ID3DXAllocateHierarchy
	{
	public:
		MyAllocateHierarchy(float AmbientRatio = 0.4f); //�R���X�g���N�^ 0.31f
		~MyAllocateHierarchy(); //�f�X�g���N�^
	
		STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame); //�t���[���̍쐬
		STDMETHOD(CreateMeshContainer)(THIS_ LPCSTR Name, CONST D3DXMESHDATA *pMeshData, CONST D3DXMATERIAL *pMaterials, CONST D3DXEFFECTINSTANCE *pEffectInstances, DWORD NumMaterials, CONST DWORD *pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER *ppNewMeshContainer); //���b�V���R���e�i�쐬
		STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree); //�t���[���̍폜
		STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree); //���b�V���R���e�i�̍폜

		void SetInitTransMatrix(D3DXFRAME_WORLD *pFrame); //������Ԃ̕ϊ��s���ۑ��i�ċA�֐��j

		//�A�N�Z�b�T
		D3DXFRAME_WORLD **GetppRootFrame() { return &pRootFrame; }
	protected:
	private:
		D3DXFRAME_WORLD *pRootFrame; //���[�g�t���[���ւ̃|�C���^
		float AmbientRatio; //�A���r�G���g�J���[�����肷��Ƃ��̌W��

		LPSTR StrCopy(LPCSTR pSrc); //������̗̈�m�ۂƃR�s�[
	};
}

/*Direct3D�ł̃A�j���[�V�����p��ID3DXAllocateHierarchy�N���X���p�������N���X�B
�A�j���[�V�����t����x�t�@�C����ǂݍ��ޏꍇ�ɁAD3DXLoadMeshHierarchyFromX�֐���
�����ɂ��� MyAllocateHierarchy�I�u�W�F�N�g�ւ̃|�C���^��n���B*/
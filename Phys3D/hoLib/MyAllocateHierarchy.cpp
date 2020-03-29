#include "MyAllocateHierarchy.h"
#include <stdlib.h>
#include "Direct3D.h"

namespace ho {
	//�R���X�g���N�^
	MyAllocateHierarchy::MyAllocateHierarchy(float AmbientRatio) 
	{
		pRootFrame = NULL;
		this->AmbientRatio = AmbientRatio;
	}

	//�f�X�g���N�^
	MyAllocateHierarchy::~MyAllocateHierarchy()
	{
		if (pRootFrame) //���[�g�t���[�����ݒ肳��Ă���ꍇ
			DestroyFrame(pRootFrame); //�t���[�����폜
	}

	//�t���[���̐V�K�쐬
	HRESULT MyAllocateHierarchy::CreateFrame(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame)
	{
		//�t���[����V������������
		ho::D3DXFRAME_WORLD *pFrame = new ho::D3DXFRAME_WORLD;
		ZeroMemory(pFrame, sizeof(D3DXFRAME));
		pFrame->Name = new char[strlen(Name) + 1];
		strcpy_s(pFrame->Name, strlen(Name) + 1, Name);

		*ppNewFrame = pFrame;

		return D3D_OK;
	}

	//���b�V���R���e�i�쐬
	HRESULT MyAllocateHierarchy::CreateMeshContainer(THIS_ LPCSTR Name, CONST D3DXMESHDATA *pMeshData, CONST D3DXMATERIAL *pMaterials, CONST D3DXEFFECTINSTANCE *pEffectInstances, DWORD NumMaterials, CONST DWORD *pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER *ppNewMeshContainer)
	{
		//���b�V���R���e�i�I�u�W�F�N�g���쐬
		D3DXMESHCONTAINER *pMC = new D3DXMESHCONTAINER;
		ZeroMemory(pMC, sizeof(D3DXMESHCONTAINER));
	
		//���O�̃R�s�[
		pMC->Name = StrCopy(Name);

		//���b�V���f�[�^���R�s�[
		switch (pMeshData->Type) //���b�V���̃^�C�v�ɂ���ĕ���
		{
		case D3DXMESHTYPE_MESH: //�ʏ탁�b�V��
			pMC->MeshData.pMesh = pMeshData->pMesh;
			pMC->MeshData.pMesh->AddRef();
			break;
		case D3DXMESHTYPE_PMESH: //�v���O���b�V�u���b�V��
			pMC->MeshData.pPMesh = pMeshData->pPMesh;
			pMC->MeshData.pPMesh->AddRef();
			break;
		case D3DXMESHTYPE_PATCHMESH: //�p�b�`���b�V��
			pMC->MeshData.pPatchMesh = pMeshData->pPatchMesh;
			pMC->MeshData.pPatchMesh->AddRef();
			break;
		}
		pMC->MeshData.Type = pMeshData->Type; //�^�C�v���R�s�[

		//�}�e���A���f�[�^�̃R�s�[
		pMC->pMaterials = new D3DXMATERIAL[NumMaterials]; //�}�e���A���̐������z����m��
		for (DWORD i = 0; i < NumMaterials; i++)
		{
			pMC->pMaterials[i].MatD3D = pMaterials[i].MatD3D;

			//�A���r�G���g�F�������f�B�t���[�Y������ AmbientRatio ����v�Z����
			pMC->pMaterials[i].MatD3D.Ambient = pMaterials[i].MatD3D.Diffuse;
			pMC->pMaterials[i].MatD3D.Ambient.r *= AmbientRatio;
			pMC->pMaterials[i].MatD3D.Ambient.g *= AmbientRatio;
			pMC->pMaterials[i].MatD3D.Ambient.b *= AmbientRatio;

			//((D3DXMATERIAL_TEXTURE *)pMC->pMaterials)[i].MatD3D.Power *= 4.0f; //�X�y�L�������g�U������
			/*Power �� Blender ��ł� Hardness �ɑΉ����ABlender ��� 511 �� 1000.0f �ƂȂ�B
			�l���傫���قǁA�X�y�L��������_�ɏW������B
			*/

			pMC->pMaterials[i].pTextureFilename = StrCopy(pMaterials[i].pTextureFilename);
		}
		//�}�e���A���̐�
		pMC->NumMaterials = NumMaterials;

		//�G�t�F�N�g�f�[�^
		const D3DXEFFECTINSTANCE *pEI = pEffectInstances;
		pMC->pEffects = new D3DXEFFECTINSTANCE;
		ZeroMemory(pMC->pEffects, sizeof(D3DXEFFECTINSTANCE));
		if (pEI)
		{
			pMC->pEffects->pEffectFilename = StrCopy(pEI->pEffectFilename);
			pMC->pEffects->NumDefaults = pEI->NumDefaults;
			pMC->pEffects->pDefaults = new D3DXEFFECTDEFAULT[pEI->NumDefaults];

			D3DXEFFECTDEFAULT *pDIST = pEI->pDefaults; // �R�s�[��
			D3DXEFFECTDEFAULT *pCOPY = pMC->pEffects->pDefaults; // �R�s�[��
			for(DWORD i = 0; i < pEI->NumDefaults; i++)
			{
				pCOPY[i].pParamName = StrCopy(pDIST[i].pParamName);
				DWORD NumBytes = pCOPY[i].NumBytes = pDIST[i].NumBytes;
				pCOPY[i].Type = pDIST[i].Type;
			   if(pDIST[i].Type <= D3DXEDT_DWORD)
			   {
				  pCOPY[i].pValue = new DWORD[NumBytes];
				  memcpy(pCOPY[i].pValue, pDIST[i].pValue, NumBytes);
			   }
			}
		}

		//�אڃ|���S���C���f�b�N�X
		DWORD NumPolygon = pMeshData->pMesh->GetNumFaces();
		pMC->pAdjacency = new DWORD[NumPolygon * 3];
		memcpy(pMC->pAdjacency, pAdjacency, NumPolygon * 3 * sizeof(DWORD));

		//�X�L�����
		if (pSkinInfo)
		{	
			pMC->pSkinInfo = pSkinInfo;
			pMC->pSkinInfo->AddRef();
		}

		*ppNewMeshContainer = pMC; //���b�V���R���e�i�\���̂̃|�C���^�������̃|�C���^�֓n��

		return D3D_OK;
	}

	//�t���[���폜
	HRESULT MyAllocateHierarchy::DestroyFrame(THIS_ LPD3DXFRAME pFrameToFree)
	{
		SDELETES(pFrameToFree->Name);

		if (pFrameToFree->pMeshContainer)
			DestroyMeshContainer(pFrameToFree->pMeshContainer);

		if (pFrameToFree->pFrameSibling)
			DestroyFrame(pFrameToFree->pFrameSibling);

		if (pFrameToFree->pFrameFirstChild)
			DestroyFrame(pFrameToFree->pFrameFirstChild);

		SDELETE(pFrameToFree);

		return D3D_OK;
	}

	//���b�V���R���e�i�폜
	HRESULT MyAllocateHierarchy::DestroyMeshContainer(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree)
	{
		SDELETE(pMeshContainerToFree->Name); //���O

		//���b�V��
		RELEASE(pMeshContainerToFree->MeshData.pMesh);

		//�}�e���A��
		for (DWORD i = 0; i < pMeshContainerToFree->NumMaterials; i++)
			SDELETES(pMeshContainerToFree->pMaterials[i].pTextureFilename);
		SDELETES(pMeshContainerToFree->pMaterials);

		//�G�t�F�N�g
		for (DWORD i = 0; i < pMeshContainerToFree->pEffects->NumDefaults; i++)
		{
			SDELETES(pMeshContainerToFree->pEffects->pDefaults[i].pParamName);
			SDELETES(pMeshContainerToFree->pEffects->pDefaults[i].pValue);
		}
		SDELETES(pMeshContainerToFree->pEffects->pEffectFilename);
		SDELETES(pMeshContainerToFree->pEffects->pDefaults);
		SDELETE(pMeshContainerToFree->pEffects);

		//�אڃ|���S���C���f�b�N�X
		SDELETE(pMeshContainerToFree->pAdjacency);
		RELEASE(pMeshContainerToFree->pSkinInfo);

		SDELETE(pMeshContainerToFree);

		return D3D_OK;
	}

	//������̗̈�m�ۂƃR�s�[
	LPSTR MyAllocateHierarchy::StrCopy(LPCSTR pSrc) 
	{
		if (!pSrc)
			return NULL;

		LPSTR str = new char[strlen(pSrc) + 1];
		strcpy_s(str, strlen(pSrc) + 1, pSrc);

		return str;
	}

	//������Ԃ̕ϊ��s���ۑ��i�ċA�֐��j
	void MyAllocateHierarchy::SetInitTransMatrix(D3DXFRAME_WORLD *pFrame) 
	{
		pFrame->InitTransMatrix = pFrame->TransformationMatrix; //������Ԃ̕ϊ��s���ۑ�

		if (pFrame->pFrameFirstChild) //�q���t���[�������݂���ꍇ
			SetInitTransMatrix((D3DXFRAME_WORLD *)pFrame->pFrameFirstChild); //�ċA���s

		if (pFrame->pFrameSibling) //�Z��t���[�������݂���ꍇ
			SetInitTransMatrix((D3DXFRAME_WORLD *)pFrame->pFrameSibling); //�ċA���s

		return;
	}

	//�ʂ̐����擾����ċA�֐�
	DWORD D3DXFRAME_WORLD::GetNumFacesReflect() 
	{
		DWORD Faces = 0;

		D3DXMESHCONTAINER *pMeshContainer = this->pMeshContainer;
		while (pMeshContainer)
		{
			Faces += pMeshContainer->MeshData.pMesh->GetNumFaces();
			pMeshContainer = pMeshContainer->pNextMeshContainer;
		}

		if (this->pFrameFirstChild) //�q���t���[�������݂���ꍇ
			Faces += ((D3DXFRAME_WORLD *)this->pFrameFirstChild)->GetNumFacesReflect(); //�ċA���s

		if (this->pFrameSibling) //�Z��t���[�������݂���ꍇ
			Faces += ((D3DXFRAME_WORLD *)this->pFrameSibling)->GetNumFacesReflect(); //�ċA���s

		return Faces;
	}

	//���O����t���[������������ċA�֐�
	D3DXFRAME_WORLD *D3DXFRAME_WORLD::FindFrame(LPSTR str) 
	{
		if (!strcmp(this->Name, str)) //���O����v�����ꍇ
			return this;

		if (this->pFrameFirstChild) //�q���t���[�������݂���ꍇ
		{
			D3DXFRAME_WORLD *pFindFrame = ((D3DXFRAME_WORLD *)this->pFrameFirstChild)->FindFrame(str); //�q���t���[����T��
			if (pFindFrame) //�������Ă����ꍇ
				return pFindFrame;
		}

		if (this->pFrameSibling) //���̌Z��t���[�������݂���ꍇ
		{
			D3DXFRAME_WORLD *pFindFrame = ((D3DXFRAME_WORLD *)this->pFrameSibling)->FindFrame(str); //�Z��t���[����T��
			if (pFindFrame) //�������Ă����ꍇ
				return pFindFrame;
		}

		return NULL;
	}
}
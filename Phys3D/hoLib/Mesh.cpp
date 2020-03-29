#include "Mesh.h"
#include "Common.h"
#include "Debug.h"
#include "D3DResource.h"
#include "hoLib.h"
#include "WorldTransMtrxStack.h"
#include "MyAllocateHierarchy.h"
#include "Direct3D.h"
#include "Math.h"
#include "Path.h"
#include <algorithm>
#include "Vector.h"

namespace ho {

//�R���X�g���N�^
MeshManager::MeshManager()
{
}

//�f�X�g���N�^
MeshManager::~MeshManager() 
{
	//�S�Ẵ��b�V���I�u�W�F�N�g���폜
	for (std::vector<Mesh *>::iterator itr = vectorpMeshObj.begin(); itr != vectorpMeshObj.end(); itr++)
		SDELETE(*itr);
}

//���b�V���I�u�W�F�N�g��o�^
DWORD MeshManager::AddMesh(Mesh *pMeshObj) 
{
	vectorpMeshObj.push_back(pMeshObj);

	return vectorpMeshObj.size(); //�o�^���ꂽ���b�V���̃C���f�b�N�X���Ԃ�
}

//���b�V���I�u�W�F�N�g���폜
void MeshManager::DeleteMesh(Mesh *pMeshObj) 
{
	//�Y�����郁�b�V���I�u�W�F�N�g��T���č폜
	for (std::vector<Mesh *>::iterator itr = vectorpMeshObj.begin(); itr != vectorpMeshObj.end(); itr++)
		if (*itr == pMeshObj)
			SDELETE(*itr);

	return;
}

//���b�V���I�u�W�F�N�g���폜
void MeshManager::DeleteMesh(LPD3DXMESH pMesh) 
{
	//�Y�����郁�b�V���I�u�W�F�N�g��T���č폜
	for (std::vector<Mesh *>::iterator itr = vectorpMeshObj.begin(); itr != vectorpMeshObj.end(); itr++)
		if (*itr) //���b�V���I�u�W�F�N�g�����݂���ꍇ
			if ((*itr)->GetpD3DXMesh() == pMesh)
				SDELETE(*itr);

	return;
}









//�R���X�g���N�^�i�쐬�ς݂̃��b�V�����ށj
Mesh::Mesh(LPD3DXMESH pD3DXMesh)
{
	this->pDirect3DObj = NULL;
	this->pD3DResourceManagerObj = NULL;
	this->pD3DXMesh = pD3DXMesh;
	this->pAH = NULL;

	Options = pD3DXMesh->GetOptions(); //�쐬���̃I�v�V������ۑ�

	CalcRadius(); //���a�i���_����ł��������_�܂ł̋����j���v�Z����
}

//�R���X�g���N�^�i�t�@�C�����烍�[�h�j
Mesh::Mesh(ho::Direct3D *pDirect3DObj, TCHAR *pFilename, D3DResourceManager *pD3DResourceManagerObj, double TextureSizeRatio) 
{
	this->pDirect3DObj = pDirect3DObj;
	this->pD3DDevice = pDirect3DObj->GetpD3DDevice();
	this->pAC = NULL;
	this->pAH = NULL;
	this->pFrame = NULL;
	this->pD3DResourceManagerObj = pD3DResourceManagerObj;

	HRESULT hr;
	LPD3DXBUFFER pMaterialBuf; //�}�e���A���o�b�t�@�̃|�C���^
	DWORD MaterialNum; //�}�e���A���̐�
	
	hr = D3DXLoadMeshFromX(pFilename, D3DXMESH_MANAGED, pD3DDevice, NULL, &pMaterialBuf, NULL, &MaterialNum, &pD3DXMesh); //�t�@�C�����烁�b�V�������[�h

	if (FAILED(hr)) //���b�V���쐬�Ɏ��s�����ꍇ
	{
		tstring str;
		ho::Direct3D::GetErrorStr(str, hr);
		str = TEXT("���b�V���̍쐬�Ɏ��s���܂����B") + str;
		ER(str.c_str(), __WFILE__, __LINE__, true);
		pD3DXMesh = NULL;
	} else {
		Options = pD3DXMesh->GetOptions(); //�쐬���̃I�v�V������ۑ�

		//�œK��
		int Faces = GetNumFaces(); //�|���S�������擾
		std::vector<DWORD> vectorAdjacency(Faces * 3);
		pD3DXMesh->GenerateAdjacency(0.00001f, &vectorAdjacency.at(0));
		pD3DXMesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE, &vectorAdjacency.at(0), 0, 0, 0);

		int Faces2 = GetNumFaces();
		tstring str;
		ho::tPrint(str, TEXT("%s �����[�h���܂����B(%d/%d Polygon)"), pFilename, Faces, Faces2);
		ER(str.c_str(), __WFILE__, __LINE__, NULL);

		D3DXMATERIAL *pMaterial = (D3DXMATERIAL *)pMaterialBuf->GetBufferPointer(); //�}�e���A���\���̂̃|�C���^�𓾂�
		D3DXIMAGE_INFO d3dxii;

		tstring strDir = ho::GetDirectory(pFilename); //�t�@�C��������f�B���N�g�����i�v���O�����{�̂���X�t�@�C���܂ł̑��΃p�X�j�݂̂𒊏o

		//�}�e���A���̐������}�e���A���\���̂���}�e���A���ƃe�N�X�`�����擾
		for (DWORD i = 0; i < MaterialNum; i++)
		{
			//�}�e���A���̃A���r�G���g���f�B�t���[�Y��1/4�̖��邳�ɂ���
			pMaterial[i].MatD3D.Ambient = pMaterial[i].MatD3D.Diffuse;
			pMaterial[i].MatD3D.Ambient.r *= 0.25f;
			pMaterial[i].MatD3D.Ambient.g *= 0.25f;
			pMaterial[i].MatD3D.Ambient.b *= 0.25f;

			pMaterial[i].MatD3D.Power *= 4.0f; //�X�y�L�������g�U������

			listMaterial.push_back(pMaterial[i].MatD3D); //�}�e���A�������X�g�ɒǉ�

			ComPtr<LPDIRECT3DTEXTURE9> cpTexture;
			if (pMaterial[i].pTextureFilename && lstrlen((LPCWSTR)pMaterial[i].pTextureFilename) > 0) //�e�N�X�`���̃t�@�C�������ݒ肳��Ă���ꍇ
			{
				tstring strFilename = ho::CharToWChar(pMaterial[i].pTextureFilename); //�e�N�X�`���t�@�C�������}���`�o�C�g���烏�C�h�����ɕϊ�
				strFilename = strDir + strFilename; //�ŏ��Ƀf�B���N�g�����i�v���O�����{�̂���X�t�@�C���܂ł̑��΃p�X�j��ǉ�

				D3DXGetImageInfoFromFile((TCHAR *)strFilename.c_str(), &d3dxii); //�e�N�X�`���̏����擾
				hr = D3DXCreateTextureFromFileEx(pD3DDevice, (TCHAR *)strFilename.c_str(), UINT(d3dxii.Width * TextureSizeRatio), UINT(d3dxii.Height * TextureSizeRatio), 0, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, &d3dxii, NULL, cpTexture.ToCreator()); //�t�@�C������e�N�X�`����ǂ�
				if (FAILED(hr))
					ER(TEXT("�e�N�X�`���̃��[�h�Ɏ��s���܂����B"), __WFILE__, __LINE__, true);
			}
			listcpTexture.push_back(cpTexture); //�e�N�X�`�������X�g�ɒǉ�
			if (pD3DResourceManagerObj && cpTexture.GetppInterface())
				pD3DResourceManagerObj->Regist(new ho::Texture(cpTexture)); //�e�N�X�`���}�l�[�W���ɒǉ�
		}
		
		RELEASE(pMaterialBuf);	//�}�e���A���o�b�t�@�̉��
		this->pD3DDevice = pD3DDevice;

		CalcRadius(); //���a�i���_����ł��������_�܂ł̋����j���v�Z����
	}
}

//�R���X�g���N�^�i�t�@�C������A�j���[�V�����t���Ń��[�h�j
Mesh::Mesh(ho::Direct3D *pDirect3DObj, TCHAR *pFilename, D3DResourceManager *pTextureManagerObj, BOOL Animation) 
{
	this->pDirect3DObj = pDirect3DObj;
	this->pD3DDevice = pDirect3DObj->GetpD3DDevice();
	this->pD3DXMesh = NULL;
	this->pD3DResourceManagerObj = pD3DResourceManagerObj;
	this->pAH = NULL;

	pAH = new ho::MyAllocateHierarchy; //�t���[���K�w�\���N���X�̃I�u�W�F�N�g�쐬

	HRESULT hr = D3DXLoadMeshHierarchyFromX(pFilename, D3DXMESH_MANAGED, pD3DDevice, pAH, NULL, (D3DXFRAME **)&pFrame, &pAC);
	if (FAILED(hr))
	{
		tstring str;
		ho::Direct3D::GetErrorStr(str, hr);
		str = TEXT("�A�j���[�V�����t�����b�V���̃��[�h�Ɏ��s���܂����B") + str;
		ER(str.c_str(), __WFILE__, __LINE__, true);
		SDELETE(pAH);
	} else {
		*pAH->GetppRootFrame() = pFrame; //�t���[���K�w�\���N���X�̃I�u�W�F�N�g�փ��[�g�t���[���ւ̃|�C���^��ۑ�
		pAH->SetInitTransMatrix(pFrame); //������Ԃ̕ϊ��s���ۑ��i�ċA�֐��j
		this->pD3DDevice = pD3DDevice;

		int Faces = GetNumFaces(); //�|���S�������擾
		tstring str;
		ho::tPrint(str, TEXT("%s �����[�h���܂����B(%d Polygon)"), pFilename, Faces);
		ER(str.c_str(), __WFILE__, __LINE__, true);

		CalcRadius(); //���a�i���_����ł��������_�܂ł̋����j���v�Z����
	}
}

//�R���X�g���N�^�i���b�V���o�b�t�@����쐬�j
Mesh::Mesh(MeshBuffer *pMeshBufferObj, ho::Direct3D *pDirect3DObj, D3DResourceManager *pD3DResourceManagerObj) 
{
	this->pDirect3DObj = pDirect3DObj;
	this->pD3DDevice = pDirect3DObj->GetpD3DDevice();
	this->pAC = NULL;
	this->pAH = NULL;
	this->pFrame = NULL;
	this->pD3DResourceManagerObj = pD3DResourceManagerObj;

	HRESULT hr;
	hr = D3DXCreateMesh(pMeshBufferObj->GetIndicesNum() / 3, pMeshBufferObj->GetVerticesNum(), D3DXMESH_VB_MANAGED | D3DXMESH_IB_MANAGED, ho::D3DVERTEX::ve, pD3DDevice, &pD3DXMesh); //���b�V�����쐬

	if (FAILED(hr)) //���b�V���쐬�Ɏ��s�����ꍇ
	{
		tstring str;
		ho::Direct3D::GetErrorStr(str, hr);
		str = TEXT("���b�V���̍쐬�Ɏ��s���܂����B") + str;
		ER(str.c_str(), __WFILE__, __LINE__, true);
		pD3DXMesh = NULL;
	} else {
		Options = pD3DXMesh->GetOptions(); //�쐬���̃I�v�V������ۑ�

		CopyToBuffer(pMeshBufferObj); //�쐬�������b�V���փ��b�V���o�b�t�@����f�[�^���R�s�[����
		
		std::map<int, MeshBuffer::SUBSET> *pMapSubset = pMeshBufferObj->GetpmapSubset(); //�T�u�Z�b�g�̃}�b�v���擾
		//���ׂẴT�u�Z�b�g�𑖍�
		for (std::map<int, MeshBuffer::SUBSET>::iterator itr = pMapSubset->begin(); itr != pMapSubset->end(); itr++)
		{
			listMaterial.push_back(itr->second.Material); //�}�e���A�����R�s�[
			listcpTexture.push_back(itr->second.cpTexture); //�e�N�X�`�����R�s�[
			if (pD3DResourceManagerObj)
				pD3DResourceManagerObj->Regist(new ho::Texture(itr->second.cpTexture)); //�e�N�X�`���}�l�[�W���ɒǉ�
		}
		this->pD3DDevice = pD3DDevice;

		CalcRadius(); //���a�i���_����ł��������_�܂ł̋����j���v�Z����
	}
}

//�f�X�g���N�^
Mesh::~Mesh() 
{
	RELEASE(pD3DXMesh);
	SDELETE(pAH);
}

//�쐬�������b�V���փ��b�V���o�b�t�@����f�[�^���R�s�[����
BOOL Mesh::CopyToBuffer(MeshBuffer *pMeshBufferObj) 
{
	HRESULT hr;
	LPVOID pBuffer; //�o�b�t�@�ւ̃|�C���^
	std::map<int, MeshBuffer::SUBSET> *pMapSubset = pMeshBufferObj->GetpmapSubset(); //�T�u�Z�b�g�̃}�b�v���擾

	hr = pD3DXMesh->LockVertexBuffer(0, &pBuffer); //���_�o�b�t�@�����b�N
	if (FAILED(hr)) //���b�N�Ɏ��s�����ꍇ
	{
		tstring str;
		ho::Direct3D::GetErrorStr(str, hr);
		str = TEXT("���_�o�b�t�@�̃��b�N�Ɏ��s���܂����B") + str;
		ER(str.c_str(), __WFILE__, __LINE__, true);
		return FALSE;
	}
	//���ׂẴT�u�Z�b�g�𑖍�
	for (std::map<int, MeshBuffer::SUBSET>::iterator itr = pMapSubset->begin(); itr != pMapSubset->end(); itr++)
	{
		int CopySize = sizeof(ho::D3DVERTEX) * itr->second.vectorVt.size(); //�R�s�[����T�C�Y
		memcpy(pBuffer, &itr->second.vectorVt.at(0), CopySize); //���_���𒸓_�o�b�t�@�փR�s�[
		pBuffer = (BYTE *)pBuffer + CopySize; //�R�s�[�������������_�o�b�t�@��i�߂�
	}
	hr = pD3DXMesh->UnlockVertexBuffer(); //���_�o�b�t�@�̃��b�N����
	if (FAILED(hr)) //���b�N�����Ɏ��s�����ꍇ
	{
		tstring str;
		ho::Direct3D::GetErrorStr(str, hr);
		str = TEXT("���_�o�b�t�@�̃��b�N�����Ɏ��s���܂����B") + str;
		ER(str.c_str(), __WFILE__, __LINE__, true);
		return FALSE;
	}

	hr = pD3DXMesh->LockIndexBuffer(0, &pBuffer); //�C���f�b�N�X�o�b�t�@�����b�N
	if (FAILED(hr)) //���b�N�Ɏ��s�����ꍇ
	{
		tstring str;
		ho::Direct3D::GetErrorStr(str, hr);
		str = TEXT("�C���f�b�N�X�o�b�t�@�̃��b�N�Ɏ��s���܂����B") + str;
		ER(str.c_str(), __WFILE__, __LINE__, true);
		return FALSE;
	}
	//���ׂẴT�u�Z�b�g�𑖍�
	int IndexOffset = 0; //�C���f�b�N�X�̃I�t�Z�b�g
	for (std::map<int, MeshBuffer::SUBSET>::iterator itr = pMapSubset->begin(); itr != pMapSubset->end(); itr++)
	{
		for (DWORD i = 0; i < itr->second.vectorIndex.size(); i++)
		{
			*(WORD *)pBuffer = IndexOffset + itr->second.vectorIndex.at(i); //�I�t�Z�b�g�������ăR�s�[
			pBuffer = (WORD *)pBuffer + 1; //�R�s�[�������������_�o�b�t�@��i�߂�
		}
		IndexOffset += itr->second.vectorIndex.size(); //�R�s�[�����C���f�b�N�X�̕������I�t�Z�b�g�����Z
	}
	hr = pD3DXMesh->UnlockIndexBuffer(); //�C���f�b�N�X�o�b�t�@�̃��b�N����
	if (FAILED(hr)) //���b�N�����Ɏ��s�����ꍇ
	{
		tstring str;
		ho::Direct3D::GetErrorStr(str, hr);
		str = TEXT("�C���f�b�N�X�o�b�t�@�̃��b�N�����Ɏ��s���܂����B") + str;
		ER(str.c_str(), __WFILE__, __LINE__, true);
		return FALSE;
	}

	std::vector<D3DXATTRIBUTERANGE> vectorAR; //�����\���̔z��
	D3DXATTRIBUTERANGE ar; //�����\����
	ZeroMemory(&ar, sizeof(D3DXATTRIBUTERANGE));
	DWORD i = 0;
	for (std::map<int, MeshBuffer::SUBSET>::iterator itr = pMapSubset->begin(); itr != pMapSubset->end(); itr++)
	{
		ar.AttribId = i;
		ar.FaceStart += ar.FaceCount; //�ʂ̊J�n��
		ar.FaceCount = itr->second.vectorIndex.size() / 3; //�ʂ̐�
		ar.VertexStart += ar.VertexCount; //���_�̊J�n�ʒu
		ar.VertexCount = itr->second.vectorVt.size(); //���_�̐�

		vectorAR.push_back(ar); //�z��ɒǉ�
		i++;
	}
	hr = pD3DXMesh->SetAttributeTable(&vectorAR.at(0), vectorAR.size()); //���b�V���֑�����ݒ�
	if (FAILED(hr)) //�����̐ݒ�Ɏ��s�����ꍇ
	{
		tstring str;
		ho::Direct3D::GetErrorStr(str, hr);
		str = TEXT("�����̐ݒ�Ɏ��s���܂����B") + str;
		ER(str.c_str(), __WFILE__, __LINE__, true);
		return FALSE;
	}

	return TRUE;
}

//�`��
void Mesh::Draw() 
{
	if (!pD3DDevice) //�����̏ꍇ
		return;

	//�f�o�C�X�̐ݒ��ۑ�
	DWORD OldFVF;
	pD3DDevice->GetFVF(&OldFVF);
	D3DMATERIAL9 OldMaterial;
	pD3DDevice->GetMaterial(&OldMaterial);
	IDirect3DBaseTexture9 *pOldTexture;
	pD3DDevice->GetTexture(0, &pOldTexture);
	DWORD TextureStageState[7];
	for (int i = 0; i < 7; i++)
		pD3DDevice->GetTextureStageState(0, D3DTEXTURESTAGESTATETYPE(i), &TextureStageState[i]);

	//�F�A���������A���Ƀe�N�X�`���ƒ��_�̃f�B�t���[�Y��������
	pD3DDevice->SetTextureStageState(0,	D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pD3DDevice->SetTextureStageState(0,	D3DTSS_COLOROP, D3DTOP_MODULATE);
	pD3DDevice->SetTextureStageState(0,	D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pD3DDevice->SetTextureStageState(0,	D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pD3DDevice->SetTextureStageState(0,	D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pD3DDevice->SetTextureStageState(0,	D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	if (pD3DXMesh)
		DrawNormal(); //�W�����b�V���̕`��
	else
		DrawAnimation(); //�A�j���[�V�����t�����b�V���̕`��

	//�f�o�C�X�̐ݒ�𕜌�
	pD3DDevice->SetTexture(0, pOldTexture);
	pD3DDevice->SetMaterial(&OldMaterial);
	pD3DDevice->SetFVF(OldFVF);
	for (int i = 0; i < 7; i++)
		pD3DDevice->SetTextureStageState(0, D3DTEXTURESTAGESTATETYPE(i), TextureStageState[i]);

	return;
}

//�W�����b�V���̕`��
void Mesh::DrawNormal()
{
	DWORD Subset; //�T�u�Z�b�g��
	pD3DXMesh->GetAttributeTable(NULL, &Subset); //�T�u�Z�b�g�����擾
	//Subset = listMaterial.size();

	//���_�t�H�[�}�b�g���擾
	D3DVERTEXELEMENT9 ve[MAX_FVF_DECL_SIZE];
	pD3DXMesh->GetDeclaration(ve);
	/*
	//���_�錾���쐬
	IDirect3DVertexDeclaration9 *pVertexDec;
	pD3DDevice->CreateVertexDeclaration(ve, &pVertexDec);
	//���_�錾��ݒ�
	pD3DDevice->SetVertexDeclaration(pVertexDec);
	*/

	//�S�ẴT�u�Z�b�g�𑖍�
	std::list<D3DMATERIAL9>::iterator itrMaterial = listMaterial.begin();
	std::list<ComPtr<LPDIRECT3DTEXTURE9>>::iterator itrcpTexture = listcpTexture.begin();
	for (DWORD i = 0; i < Subset; i++)
	{
		pDirect3DObj->SetMaterial(&(D3DMATERIAL9)*itrMaterial);
		pD3DDevice->SetTexture(0, *itrcpTexture);

		//�����_�J���[�Ȃǂ̓`�B���@�͍čl����
		/*
		if (ve[2].Usage == 10) //���_�J���[���܂ޏꍇ
			pDirect3DObj->SetShaderTechnique(TRUE); //�V�F�[�_�̃e�N�j�b�N�̐ݒ�
		else
			pDirect3DObj->SetShaderTechnique(FALSE); //�V�F�[�_�̃e�N�j�b�N�̐ݒ�
			*/
		//pDirect3DObj->ShaderBeginDraw(); //�V�F�[�_�̕`��J�n
		pD3DXMesh->DrawSubset(i);

		//pDirect3DObj->ShaderEndDraw(); //�V�F�[�_�̕`��I��

		itrMaterial++;
		itrcpTexture++;
	}

	return;
}

//�A�j���[�V�����t�����b�V���̕`��
void Mesh::DrawAnimation()
{
	D3DXMATRIX mtrxWorld; //���[���h�ϊ��s��
	pD3DDevice->GetTransform(D3DTS_WORLD, &mtrxWorld);
	WorldTransMtrxStack WTMS(&mtrxWorld); //�s��X�^�b�N�N���X�̃I�u�W�F�N�g�쐬
	std::list<ho::D3DXFRAME_WORLD *> *plistpDrawFrame; //�`��t���[�����X�g�ւ̃|�C���^

	WTMS.UpdateFrame(pFrame); //�t���[���̃��[���h�ϊ��s����X�V
	plistpDrawFrame = WTMS.GetplistpDrawFrame(); //�`��t���[�����X�g�ւ̃|�C���^���擾

	//�`��p�t���[�����X�g�𑖍����ď��Ԃɕ`��
	int i, MaterialNum;
	for (std::list<ho::D3DXFRAME_WORLD *>::iterator itr = plistpDrawFrame->begin(); itr != plistpDrawFrame->end(); itr++)
	{
		pDirect3DObj->SetWorldMatrix(&(*itr)->WorldTransMatrix); //�f�o�C�X�փ��[���h�ϊ��s���ݒ�
		MaterialNum = (*itr)->pMeshContainer->NumMaterials;
		for (i = 0; i < MaterialNum; i++)
		{
			pDirect3DObj->SetMaterial(&((*itr)->pMeshContainer->pMaterials)[i].MatD3D); //�}�e���A����ݒ�
			//pD3DDevice->SetTexture(0, ((D3DXMATERIAL_TEXTURE *)(*itr)->pMeshContainer->pMaterials)[i].pD3DTexture); //�e�N�X�`����ݒ�
			//pDirect3DObj->SetShaderTechnique(FALSE); //�V�F�[�_�̃e�N�j�b�N�̐ݒ�

			//pDirect3DObj->ShaderBeginDraw(); //�V�F�[�_�̕`��J�n
			(*itr)->pMeshContainer->MeshData.pMesh->DrawSubset(i);
			//pDirect3DObj->ShaderEndDraw(); //�V�F�[�_�̕`��I��
		}
	}

	return;
}

//�ʂ̐��̍��v���擾
DWORD Mesh::GetNumFaces() 
{
	DWORD Faces = 0;

	if (pD3DXMesh) //�W�����b�V���̏ꍇ
		Faces = pD3DXMesh->GetNumFaces();
	else if (pFrame) //�A�j���[�V�����t�����b�V���̏ꍇ
		Faces = pFrame->GetNumFacesReflect();
	
	return Faces;
}

//�t���[��������t���[���ւ̃|�C���^���������Ď擾����
ho::D3DXFRAME_WORLD *Mesh::FindpFrame(LPSTR str) 
{
	if (!pFrame) //�A�j���[�V������x�t�@�C���ł͂Ȃ��ꍇ
		return NULL;

	return pFrame->FindFrame(str);
}

//���a�i���_����ł��������_�܂ł̋����j���v�Z����
void Mesh::CalcRadius() 
{
	if (pD3DXMesh) //�A�j���[�V�����Ȃ��̃��b�V���̏ꍇ
	{
		DWORD BytesPerVertex = pD3DXMesh->GetNumBytesPerVertex(); //���_1������̃o�C�g�����擾
		DWORD Vertices = pD3DXMesh->GetNumVertices(); //���_�����擾

		double MaxDistance = 0, Distance;
		LPVOID pBuffer;
		pD3DXMesh->LockVertexBuffer(NULL, &pBuffer); //���_�o�b�t�@�����b�N
		for (DWORD i = 0; i < Vertices; i++) //���ׂĂ̒��_�𑖍�
		{
			Distance = ((D3DXVECTOR3 *)((BYTE *)pBuffer + BytesPerVertex * i))->GetPower(); //���_����̋������v�Z
			if (Distance > MaxDistance)
				MaxDistance = Distance;
		}
		pD3DXMesh->UnlockVertexBuffer(); //���_�o�b�t�@�̃��b�N������
		Radius = (float)MaxDistance;
	} else { //�A�j���[�V�����t���̃��b�V���̏ꍇ
		D3DXMATRIX mtrx;
		D3DXMatrixIdentity(&mtrx); //��s����쐬
		WorldTransMtrxStack WTMS(&mtrx); //�s��X�^�b�N�N���X�̃I�u�W�F�N�g�쐬
		std::list<ho::D3DXFRAME_WORLD *> *plistpDrawFrame; //�`��t���[�����X�g�ւ̃|�C���^

		WTMS.UpdateFrame(pFrame); //�t���[���̃��[���h�ϊ��s����X�V
		plistpDrawFrame = WTMS.GetplistpDrawFrame(); //�`��t���[�����X�g�ւ̃|�C���^���擾
		
		double MaxDistance = 0, Distance;

		for (std::list<ho::D3DXFRAME_WORLD *>::iterator itr = plistpDrawFrame->begin(); itr != plistpDrawFrame->end(); itr++) //�t���[�����X�g�𑖍�
		{
			LPD3DXMESH pD3DXMesh = (*itr)->pMeshContainer->MeshData.pMesh;
			DWORD BytesPerVertex = pD3DXMesh->GetNumBytesPerVertex(); //���_1������̃o�C�g�����擾
			DWORD Vertices = pD3DXMesh->GetNumVertices(); //���_�����擾

			D3DXVECTOR3 vecVertex;
			LPVOID pBuffer;
			pD3DXMesh->LockVertexBuffer(NULL, &pBuffer); //���_�o�b�t�@�����b�N
			for (DWORD i = 0; i < Vertices; i++) //���ׂĂ̒��_�𑖍�
			{
				vecVertex = *((D3DXVECTOR3 *)((BYTE *)pBuffer + BytesPerVertex * i)); //���_��񂩂���W���擾
				D3DXVec3TransformCoord(&vecVertex, &vecVertex, &(*itr)->WorldTransMatrix); //���W�����݂̃t���[���̍s��ŕϊ�
				Distance = vecVertex.GetPower(); //���_����̋������v�Z
				if (Distance > MaxDistance)
					MaxDistance = Distance;
			}
			pD3DXMesh->UnlockVertexBuffer(); //���_�o�b�t�@�̃��b�N������

		}
		
		Radius = (float)MaxDistance;
	}

	return;
}










//�T�u�Z�b�g��ǉ�
BOOL MeshBuffer::CreateSubset(int Index, SUBSET *pSubset) 
{
	mapSubset.insert(std::pair<int, SUBSET>(Index, *pSubset));

	return TRUE;
}

//�T�u�Z�b�g�ɒ��_���ƒ��_�C���f�b�N�X��ǉ�
BOOL MeshBuffer::AddVertex(int SubsetIndex, const D3DVERTEX *pVt, int Vertices, const WORD *pIndex, int Indices)
{
	if (!mapSubset.count(SubsetIndex)) //�T�u�Z�b�g�����݂��Ȃ��ꍇ
		return FALSE;

	int IndexOffset = mapSubset.at(SubsetIndex).vectorVt.size(); //���_�C���f�b�N�X�ԍ��̃I�t�Z�b�g

	//���_����ǉ�
	for (int i = 0; i < Vertices; i++)
		mapSubset.at(SubsetIndex).vectorVt.push_back(pVt[i]);

	//���_�C���f�b�N�X��ǉ�
	for (int i = 0; i < Indices; i++)
		mapSubset.at(SubsetIndex).vectorIndex.push_back(pIndex[i] + IndexOffset);

	return TRUE;
}

//���_���̍��v�𓾂�
DWORD MeshBuffer::GetVerticesNum()
{
	DWORD Size = 0;

	//�S�ẴT�u�Z�b�g�𑖍�
	for (std::map<int, SUBSET>::iterator itr = mapSubset.begin(); itr != mapSubset.end(); itr++)
		Size += itr->second.vectorVt.size();

	return Size;
}

//���_�C���f�b�N�X���̍��v�𓾂�
DWORD MeshBuffer::GetIndicesNum() 
{
	DWORD Size = 0;

	//�S�ẴT�u�Z�b�g�𑖍�
	for (std::map<int, SUBSET>::iterator itr = mapSubset.begin(); itr != mapSubset.end(); itr++)
		Size += itr->second.vectorIndex.size();

	return Size;
}







	//ID3DXMesh�̒��_��񂩂�[�̈ʒu���擾
	D3DXVECTOR3 GetD3DXMeshCornerPos(LPD3DXMESH pMesh, int Axis, bool Max, bool World)
	{
		int NumVertices = pMesh->GetNumVertices(); //���b�V���̒��_�����擾
		if (NumVertices <= 0) //���_�����݂��Ȃ��ꍇ
			return D3DXVECTOR3(0, 0, 0);

		std::vector<D3DXVECTOR3> vectorVec;
		LPVOID pBuffer;
		HRESULT hr = pMesh->LockVertexBuffer(D3DLOCK_READONLY, &pBuffer); //���_�o�b�t�@�����b�N
		if (D3D_OK == hr) //���������ꍇ
		{
			DWORD BytesPerVertex = pMesh->GetNumBytesPerVertex(); //���_���1������̃o�C�g���𓾂�
			D3DXVECTOR3 vec;

			//�S�Ă̒��_�𑖍�
			for (int i = 0; i < NumVertices; i++)
			{
				vec.x = *(float *)pBuffer;
				vec.y = *((float *)pBuffer + 1);
				vec.z = *((float *)pBuffer + 2);
				//if (World) //���f���̌��_���猩���烏�[���h���W�Ŕ�r����ꍇ
					//D3DXVec3TransformCoord(&vec, &vec, &(*itr)->WorldTransMatrix); //���[���h�s��ϊ��ō��W��ϊ�
				vectorVec.push_back(vec);
				pBuffer = (BYTE *)pBuffer + BytesPerVertex;
			}

			pMesh->UnlockVertexBuffer(); //���b�N����
		} else {
			tstring str;
			Direct3D::GetErrorStr(str, hr);
			ER(str.c_str(), __WFILE__, __LINE__, true);
			return D3DXVECTOR3(0, 0, 0);
		}

		if (Max) //�ő�l���~�����ꍇ
		{
			if (Axis == 0) //X���̏ꍇ
				std::sort(vectorVec.begin(), vectorVec.end(), D3DXVector3SortX2); 
			else if (Axis == 1) //Y���̏ꍇ
				std::sort(vectorVec.begin(), vectorVec.end(), D3DXVector3SortY2); 
			else if (Axis == 2) //Z���̏ꍇ
				std::sort(vectorVec.begin(), vectorVec.end(), D3DXVector3SortZ2);
		} else { //�ŏ��l���~�����ꍇ
			if (Axis == 0) //X���̏ꍇ
				std::sort(vectorVec.begin(), vectorVec.end(), D3DXVector3SortX1); 
			else if (Axis == 1) //Y���̏ꍇ
				std::sort(vectorVec.begin(), vectorVec.end(), D3DXVector3SortY1); 
			else if (Axis == 2) //Z���̏ꍇ
				std::sort(vectorVec.begin(), vectorVec.end(), D3DXVector3SortZ1);
		}

		return vectorVec.at(0);
	}

	bool D3DXVector3SortX1(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2) { return vec1.x < vec2.x; }
	bool D3DXVector3SortX2(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2) { return vec1.x > vec2.x; }
	bool D3DXVector3SortY1(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2) { return vec1.y < vec2.y; }
	bool D3DXVector3SortY2(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2) { return vec1.y > vec2.y; }
	bool D3DXVector3SortZ1(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2) { return vec1.z < vec2.z; }
	bool D3DXVector3SortZ2(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2) { return vec1.z > vec2.z; }
}

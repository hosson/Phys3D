//���b�V�����܂ރN���X

#pragma once
#include <d3dx9mesh.h>
#include <vector>
#include "D3DVertex.h"
#include <list>
#include <map>
#include "MyAllocateHierarchy.h"
#include "ComPtr.hpp"

namespace ho
{

	class Mesh;
	class D3DResourceManager;

	class MeshManager
	{
	public:
		MeshManager(); //�R���X�g���N�^
		~MeshManager(); //�f�X�g���N�^

		DWORD AddMesh(Mesh *pMeshObj); //���b�V���I�u�W�F�N�g��o�^
		void DeleteMesh(Mesh *pMeshObj); //���b�V���I�u�W�F�N�g���폜
		void DeleteMesh(LPD3DXMESH pMeshObj); //���b�V���I�u�W�F�N�g���폜

		LPD3DXMESH GetpMeshObj(DWORD Index) { vectorpMeshObj.at(Index); } //���b�V���𓾂�
	private:
		std::vector<Mesh *> vectorpMeshObj; //���b�V���I�u�W�F�N�g�ւ̃|�C���^�̃��X�g
		D3DResourceManager *pD3DResourceManagerObj; //���\�[�X�Ǘ��I�u�W�F�N�g�ւ̃|�C���^
	};

	class MeshBuffer;

	class Mesh
	{
	public:
		Mesh(LPD3DXMESH pD3DXMesh); //�R���X�g���N�^�i�쐬�ς݂̃��b�V�����ށj
		Mesh(ho::Direct3D *pDirect3DObj, TCHAR *pFilename, D3DResourceManager *pD3DResourceManagerObj, double TextureSizeRatio = 1.0); //�R���X�g���N�^�i�t�@�C�����烍�[�h�j
		Mesh(ho::Direct3D *pDirect3DObj, TCHAR *pFilename, D3DResourceManager *pD3DResourceManagerObj, BOOL Animation); //�R���X�g���N�^�i�t�@�C������A�j���[�V�����t���Ń��[�h�j
		Mesh(MeshBuffer *pMeshBufferObj, ho::Direct3D *pDirect3DObj, D3DResourceManager *pD3DResourceManagerObj); //�R���X�g���N�^�i���b�V���o�b�t�@����쐬�j
		~Mesh(); //�f�X�g���N�^

		void Draw(); //�`��
		DWORD GetNumFaces(); //�ʂ̐��̍��v���擾
		ho::D3DXFRAME_WORLD *Mesh::FindpFrame(LPSTR str); //�t���[��������t���[���ւ̃|�C���^���������Ď擾����

		//�A�N�Z�b�T
		LPDIRECT3DDEVICE9 GetpD3DDevice() { return pD3DDevice; }
		LPD3DXMESH GetpD3DXMesh() { return pD3DXMesh; }
		std::list<D3DMATERIAL9> *GetplistMaterial() { return &listMaterial; }
		float GetRadius() { return Radius; }
	private:
		LPDIRECT3DDEVICE9 pD3DDevice; //�֘A�t����ꂽ�f�o�C�X�i���[�h���s�ȂǂŖ����̃��b�V���ɂ�NULL���ݒ肳���j
		ho::Direct3D *pDirect3DObj;
		LPD3DXMESH pD3DXMesh; //���b�V���ւ̃|�C���^
		std::list<D3DMATERIAL9> listMaterial; //�}�e���A�����X�g
		std::list<ComPtr<LPDIRECT3DTEXTURE9>> listcpTexture; //�e�N�X�`�����X�g
		DWORD Options; //�쐬����GetOptions�œ���ꂽ�l
		D3DResourceManager *pD3DResourceManagerObj; //Direct3D ���\�[�X�Ǘ��I�u�W�F�N�g�ւ̃|�C���^
		MyAllocateHierarchy *pAH; //�t���[���K�w�I�u�W�F�N�g�ւ̃|�C���^
		D3DXFRAME_WORLD *pFrame; //�t���[���K�w�̐擪�t���[���ւ̃|�C���^
		ID3DXAnimationController *pAC; //�A�j���[�V�����R���g���[��
		float Radius; //���a

		BOOL CopyToBuffer(MeshBuffer *pMeshBufferObj); //�쐬�������b�V���փ��b�V���o�b�t�@����f�[�^���R�s�[����
		void DrawNormal(); //�W�����b�V���̕`��
		void DrawAnimation(); //�A�j���[�V�����t�����b�V���̕`��
		DWORD GetNumFacesReflect(D3DXFRAME_WORLD *pFrame); //�ʂ̐����擾����ċA�֐�
		void CalcRadius(); //���a�i���_����ł��������_�܂ł̋����j���v�Z����
	};

	class MeshBuffer
	{
	public:
		struct SUBSET //1�̃}�e���A���ƃe�N�X�`�������������_���̃T�u�Z�b�g
		{
			std::vector<D3DVERTEX> vectorVt; //���_���z��
			std::vector<WORD> vectorIndex; //���_�C���f�b�N�X�z��
			D3DMATERIAL9 Material; //�}�e���A��
			ComPtr<LPDIRECT3DTEXTURE9> cpTexture; //�e�N�X�`��
		};
	public:
		BOOL CreateSubset(int Index, SUBSET *pSubset); //�T�u�Z�b�g��ǉ�
		BOOL AddVertex(int SubsetIndex, const D3DVERTEX *pVt, int Vertices, const WORD *pIndex, int Indices); //�T�u�Z�b�g�ɒ��_���ƒ��_�C���f�b�N�X��ǉ�
		DWORD GetVerticesNum(); //���_���̍��v�𓾂�
		DWORD GetIndicesNum(); //���_�C���f�b�N�X���̍��v�𓾂�

		//�A�N�Z�b�T
		std::map<int, SUBSET> *GetpmapSubset() { return &mapSubset; }
	private:
		std::map<int, SUBSET> mapSubset;
	};

	//ID3DXMesh�̒��_��񂩂�[�̈ʒu���擾
	D3DXVECTOR3 GetD3DXMeshCornerPos(LPD3DXMESH pMesh, int Axis, bool Max, bool World);
	bool D3DXVector3SortX1(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2);
	bool D3DXVector3SortX2(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2);
	bool D3DXVector3SortY1(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2);
	bool D3DXVector3SortY2(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2);
	bool D3DXVector3SortZ1(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2);
	bool D3DXVector3SortZ2(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2);
}

/*LPD3DXMESH �����b�s���O����Mesh�N���X�Ƃ�����Ǘ�����
MeshManager�N���X�ō\�������B

���g������
�E�v���O�������Ɉ����MeshManager�N���X�����B
�ELPD3DXMESH���쐬������Mesh�N���X�ŕ�݂��̂܂�MeshManager�ɓo�^����B
�E���b�V���̍폜���́AMeshManager::DeleteMesh���g�������ō폜��������������B
�E���I�ɒ��_����ǉ��������ꍇ�́AMeshBuffer�����MeshBuffer::Add�Œ��_��ǉ������̂��A
�@Mesh�̃R���X�g���N�^�ł��̃|�C���^��n���B
�E�R���X�g���N�^�Ɉ����Ƀe�N�X�`���}�l�[�W����n���ƁA���b�V���폜���ɕt������e�N�X�`����
�@�����ō폜�����B������NULL�ɂ���Ǝ����ł͍폜����Ȃ��B

 ��Mesh�N���X��Radius�ɂ��ā�
 �E������J�����O���̔���Ɏg���邪�A���f�����[�h���̂݌v�Z�����̂ŁA�A�j���[�V��������Ƃ���
 ���m�Ȕ��a�̓A�j���[�V�������̒��_���W����̍Čv�Z���K�v�ɂȂ�B
*/

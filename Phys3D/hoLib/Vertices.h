//Direct3D�p�̒��_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@�����b�s���O�����N���X

#pragma once

#include <d3d9.h>
#pragma comment (lib, "d3d9.lib")
#include <vector>
#include "D3DVertex.h"

namespace ho {
	class Vertices
	{
	public:
		Vertices(); //�R���X�g���N�^
		~Vertices(); //�f�X�g���N�^

		void Add(ho::D3DVERTEX *pD3DVERTEX, int Vertices, WORD *pIndex, int Indexes, UINT Primitives); //���_�ƃC���f�b�N�X��ǉ�
		void CreateBuffer(LPDIRECT3DDEVICE9 pD3DDevice, DWORD Usage = 0, D3DPOOL Pool = D3DPOOL_MANAGED); //���_�o�b�t�@���C���f�b�N�X�o�b�t�@�𐶐�
		void Clear(); //�ǉ����̒��_�ƃC���f�b�N�X���폜
		void Release(); //���_�o�b�t�@���C���f�b�N�X�o�b�t�@�����
		void Draw(LPDIRECT3DDEVICE9 pD3DDevice, D3DXMATRIX *pmtrx); //�`��
		float GetMaxRadius(); //�쐬�ςݒ��_�o�b�t�@����ő唼�a���v�Z���Ď擾����
	private:
		IDirect3DVertexBuffer9 *pVertexBuffer; //���_�o�b�t�@�ւ̃|�C���^
		IDirect3DIndexBuffer9 *pIndexBuffer; //�C���f�b�N�X�o�b�t�@�ւ̃|�C���^

		std::vector<ho::D3DVERTEX> vectorD3DVERTEX; //���_�z��
		std::vector<WORD> vectorIndex; //���_�C���f�b�N�X
		UINT Primitives; //�|���S����
	};
}

/*Direct3D�̒��_�o�b�t�@���C���f�b�N�X�o�b�t�@�����b�s���O���Ă���B
D3DVERTEX�N���X�̒��_�t�H�[�}�b�g���g�p����B

�R���X�g���N�^�Ő����������_�ł͂܂��o�b�t�@�͍���Ȃ��B
Add()���\�b�h�Œ��_�����R�ɒǉ����I��������_�ŁACreateBuffer()���\�b�h���s���ƁA
���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@����������ADraw()���\�b�h�ɂ��`�悪�\�ƂȂ�B
Clear()���\�b�h�Œǉ����̓������_�����폜�ARelease()���\�b�h�Ŋe�o�b�t�@���������B
�f�X�g���N�^�ł͉���������I�ɍs����B

Add()���\�b�h�ɂ�钸�_�C���f�b�N�X�̒ǉ�����x���s���Ȃ������ꍇ�ɂ́A
DrawIndexedPrimitive�ł͂Ȃ�DrawPrimitive�ɂ�蒸�_�C���f�b�N�X�����̕`�悪�s����B
*/
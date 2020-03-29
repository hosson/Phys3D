#include "Vertices.h"
#include "Direct3D.h"
#include "Math.h"

namespace ho
{
	//�R���X�g���N�^
	Vertices::Vertices()
	{
		pVertexBuffer = NULL;
		pIndexBuffer = NULL;

		Clear(); //�ǉ����̒��_�ƃC���f�b�N�X���폜
	}

	//�f�X�g���N�^
	Vertices::~Vertices()
	{
		Release(); //���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@�����
	}

	//���_�ƃC���f�b�N�X��ǉ�
	void Vertices::Add(D3DVERTEX *pD3DVERTEX, int Vertices, WORD *pIndex, int Indexes, UINT Primitives)
	{
		int OldVertices = vectorD3DVERTEX.size(); //�ǉ��O�̒��_�����L��

		//���_�̒ǉ�
		if (pD3DVERTEX && Vertices)
			for (int i = 0; i < Vertices; i++)
				vectorD3DVERTEX.push_back(pD3DVERTEX[i]);

		//�C���f�b�N�X�̒ǉ�
		if (pIndex && Indexes)
			for (int i = 0; i < Indexes; i++)
				vectorIndex.push_back(OldVertices + pIndex[i]);

		this->Primitives += Primitives; //�|���S������ǉ�

		return;
	}

	//���_�o�b�t�@���C���f�b�N�X�o�b�t�@�𐶐�
	void Vertices::CreateBuffer(LPDIRECT3DDEVICE9 pD3DDevice, DWORD Usage, D3DPOOL Pool)
	{
		Release(); //���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@�����

		//���_�o�b�t�@���쐬
		if (vectorD3DVERTEX.size())
		{
			pD3DDevice->CreateVertexBuffer(vectorD3DVERTEX.size() * sizeof(D3DVERTEX), Usage, D3DVERTEX::FVF, Pool, &pVertexBuffer, NULL);
			if (pVertexBuffer) //���_�o�b�t�@�̍쐬�ɐ��������ꍇ
			{
				LPVOID pBuffer; //�o�b�t�@�̓��e�������|�C���^
				pVertexBuffer->Lock(0, 0, &pBuffer, 0); //�o�b�t�@�����b�N
				memcpy(pBuffer, &vectorD3DVERTEX.at(0), vectorD3DVERTEX.size() * sizeof(D3DVERTEX)); //���_�z��̓��e���o�b�t�@�փR�s�[
				pVertexBuffer->Unlock(); //�o�b�t�@�̃��b�N����
			}
		}

		//�C���f�b�N�X�o�b�t�@���쐬
		if (vectorIndex.size())
		{
			pD3DDevice->CreateIndexBuffer(vectorIndex.size() * sizeof(WORD), Usage, D3DFMT_INDEX16, Pool, &pIndexBuffer, NULL);
			if (pIndexBuffer) //�C���f�b�N�X�o�b�t�@�̍쐬�ɐ��������ꍇ
			{
				LPVOID pBuffer; //�o�b�t�@�̓��e�������|�C���^
				pIndexBuffer->Lock(0, 0, &pBuffer, 0); //�o�b�t�@�����b�N
				memcpy(pBuffer, &vectorIndex.at(0), vectorIndex.size() * sizeof(WORD)); //�C���f�b�N�X�z��̓��e���o�b�t�@�փR�s�[
				pIndexBuffer->Unlock(); //�o�b�t�@�̃��b�N����
			}
		}

		return;
	}

	//�ǉ����̒��_�ƃC���f�b�N�X���폜
	void Vertices::Clear()
	{
		vectorD3DVERTEX.clear();
		vectorD3DVERTEX.resize(0);

		vectorIndex.clear();
		vectorIndex.resize(0);

		Primitives = 0;

		return;
	}

	//���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@�����
	void Vertices::Release()
	{
		RELEASE(pVertexBuffer);
		RELEASE(pIndexBuffer);

		return;
	}

	//�`��
	void Vertices::Draw(LPDIRECT3DDEVICE9 pD3DDevice, D3DXMATRIX *pmtrx) 
	{
		if (!pD3DDevice || !pVertexBuffer)
			return;

		D3DXMATRIX mtrxOld; //���[���h�ϊ��s��ۑ��p
		pD3DDevice->GetTransform(D3DTS_WORLD, &mtrxOld); //���[���h�s���ۑ�
		pD3DDevice->SetTransform(D3DTS_WORLD, pmtrx); //�f�o�C�X�Ƀ}�g���b�N�X��ݒ�

		pD3DDevice->SetStreamSource(0, pVertexBuffer, 0, sizeof(D3DVERTEX)); //���_�o�b�t�@���f�o�C�X�̃X�g���[���ɃZ�b�g
		if (pIndexBuffer) //�C���f�b�N�X�o�b�t�@�����݂���ꍇ
		{
			pD3DDevice->SetIndices(pIndexBuffer); //�C���f�b�N�X�o�b�t�@���Z�b�g
			pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vectorD3DVERTEX.size(), 0, Primitives); //���_�C���f�b�N�X����ŕ`��
		} else { //���݂��Ȃ��ꍇ
			pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, Primitives); //���_�C���f�b�N�X�Ȃ��ŕ`��
		}

		pD3DDevice->SetTransform(D3DTS_WORLD, &mtrxOld); //�f�o�C�X�̃��[���h�ϊ��s��𕜌�

		return;
	}

	//�쐬�ςݒ��_�o�b�t�@����ő唼�a���v�Z���Ď擾����
	float Vertices::GetMaxRadius() 
	{
		/*��Ɏ�����J�����O���ȂǂɎg����l���Ԃ邪�A���̓s�x�v�Z������̂ŁA
		�`�斈�ɂ��̊֐����Ăяo�����Ƃ͔����邱�Ƃ��]�܂����B*/

		float MaxRadius = 0; //�ő唼�a

		if (pVertexBuffer) //���_�o�b�t�@�����݂���ꍇ
		{
			D3DVERTEXBUFFER_DESC vbd;
			pVertexBuffer->GetDesc(&vbd); //�o�b�t�@�����擾
			int Num = vbd.Size / vbd.Size; //���_�����v�Z

			LPVOID pBuffer; //�o�b�t�@�̓��e�������|�C���^
			pVertexBuffer->Lock(0, 0, &pBuffer, 0); //�o�b�t�@�����b�N

			//�S�Ă̒��_�̍��W�𑖍����Č��_����ł��������W�܂ł̋��������߂Ĕ��a�Ƃ���
			D3DXVECTOR3 vec;
			float Radius;
			MaxRadius = FLT_MIN;
			for (int i = 0; i < Num; i++)
			{
				//���_���̒�������W�𔲂��o��
				vec.x = *(float *)((BYTE *)pBuffer + vbd.Size * i);
				vec.y = *(((float *)((BYTE *)pBuffer + vbd.Size * i)) + 1);
				vec.z = *(((float *)((BYTE *)pBuffer + vbd.Size * i)) + 1);
				Radius = vec.GetPower(); //���_������W�܂ł̋������v�Z���Ĕ��a�Ƃ���
				if (Radius > MaxRadius)
					MaxRadius = Radius;
			}

			pVertexBuffer->Unlock(); //�o�b�t�@�̃��b�N����
		}

		return MaxRadius;
	}
}

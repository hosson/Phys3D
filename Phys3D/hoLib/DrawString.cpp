//������`��N���X
#include "DrawString.h"
#include "CharTexture.h"
#include "StringTextureCache.h"

namespace ho {

//�R���X�g���N�^
DrawString::DrawString(Direct3D *pDirect3DObj, LPDIRECT3DDEVICE9 pD3DDevice, LOGFONT *pLF, BOOL Cache)
{
	this->pDirect3DObj = pDirect3DObj;
	this->pD3DDevice = pD3DDevice;
	this->pGetCharTextureObj = new GetCharTexture(*pLF, pD3DDevice); //�����e�N�X�`���擾�I�u�W�F�N�g���쐬
	this->pStringTextureCacheManagerObj = NULL;
	if (Cache)
		pStringTextureCacheManagerObj = new StringTextureCacheManager(pD3DDevice, this); //������e�N�X�`���L���b�V���I�u�W�F�N�g���쐬

	//6�̒��_�ŕ\���ꂽ�����`�̒��_�C���f�b�N�X��ݒ�
	Index[0] = 0;
	Index[1] = 1;
	Index[2] = 2;
	Index[3] = 2;
	Index[4] = 1;
	Index[5] = 3;

	//�J�����g�ݒ�
	dwCurrentColor = 0xffffffff;
	CurrentHeight = pGetCharTextureObj->GetLOGFONT().lfHeight;
}

//�f�X�g���N�^
DrawString::~DrawString() 
{
	SDELETE(pStringTextureCacheManagerObj);
	SDELETE(pGetCharTextureObj);
}

//�`��i�J�����g�ݒ�ŕ`��j
void DrawString::Draw(int x, int y, TCHAR *lpStr)
{
	DrawAlg(x, y, lpStr, dwCurrentColor, CurrentHeight, 0);

	return;
}

//������`��i�����T�C�Y�̍������w��j
void DrawString::Draw(int x, int y, TCHAR *lpStr, int Height)
{
	DrawAlg(x, y, lpStr, dwCurrentColor, Height, 0);

	return;
}

//������`��i�����̐F���w��j
void DrawString::Draw(int x, int y, TCHAR *lpStr, DWORD dwColor)
{
	DrawAlg(x, y, lpStr, dwColor, CurrentHeight, 0);

	return;
}

//������`��i�����T�C�Y�̍����ƐF���w��j
void DrawString::Draw(int x, int y, TCHAR *lpStr, int Height, DWORD dwColor)
{
	DrawAlg(x, y, lpStr, dwColor, Height, 0);

	return;
}

//�����w�肵�ĕ�����`��
int DrawString::DrawWidth(int x, int y, TCHAR *pStr, int Width, int Height) 
{
	return DrawAlg(x, y, pStr, dwCurrentColor, Height, Width);
}

//1�t���[�����̏���
void DrawString::FrameProcess() 
{
	if (pStringTextureCacheManagerObj)
		pStringTextureCacheManagerObj->Optimize(); //�L���b�V���̍œK��

	return;
}

//�f�o�C�X���X�g���̃��\�[�X�������
void DrawString::OnLostDevice()
{
	if (pStringTextureCacheManagerObj)
		pStringTextureCacheManagerObj->Clear(); //�L���b�V���폜

	return;
}

//�`��̓�������
int DrawString::DrawAlg(int x, int y, TCHAR *lpStr, DWORD dwColor, int Height, int LimitWidth)
{	
	//���_�t�H�[�}�b�g�̐ݒ�
	DWORD OldFVF; //�ύX�O�̒��_�t�H�[�}�b�g
	pD3DDevice->GetFVF(&OldFVF); //���_�t�H�[�}�b�g��ۑ�
	ho::ComPtr<IDirect3DBaseTexture9 *> cpOldTexture; //�ύX�O�̃e�N�X�`��
	pD3DDevice->GetTexture(0, cpOldTexture.ToCreator()); //�e�N�X�`����ۑ�
	pD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1); //���_�t�H�[�}�b�g��2D�Ɏw��

	int Drawed = 0; //�`�悵��������
	
	if (pStringTextureCacheManagerObj) //������L���b�V���g�p��
	{
		StringTextureCache *pStringTextureCacheObj = pStringTextureCacheManagerObj->GetpStringTextureCache(lpStr, Height, LimitWidth); //�L���b�V�����擾

		ho::PT<int> ptTextureSize = pStringTextureCacheObj->GetptTextureSize(); //������e�N�X�`���̃T�C�Y���擾

		//���_����ݒ�
		vt[0].SetVertex((float)x, (float)y, 0, dwColor, 0.0f, 0.0f);
		vt[1].SetVertex(float(x + ptTextureSize.x), (float)y, 0, dwColor, 1.0f, 0.0f);
		vt[2].SetVertex((float)x, float(y + ptTextureSize.y), 0, dwColor, 0.0f, 1.0f);
		vt[3].SetVertex(float(x + ptTextureSize.x), float(y + ptTextureSize.y), 0, dwColor, 1.0f, 1.0f);
		D3DVERTEX_2D::Offset(vt, 4);

		pD3DDevice->SetTexture(0, pStringTextureCacheObj->GetcpTexture()); //�L���b�V�����當����̃e�N�X�`�����擾���Đݒ�
		pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, Index, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //�`��

		Drawed = pStringTextureCacheObj->GetCharNum(); //������̕��������擾
	} else {
		POINT ptTexture; //�����e�N�X�`���̕��ƍ���
		double Ratio; //�e�N�X�`���Ǝ��ۂɕ`�悷��T�C�Y�̔䗦

		//1���������o���ĕ`�揈�����s��
		int BeginX = x; //�`��J�n�ʒu��X���W
		while (*lpStr) //lpStr�̒��g��NULL�����ɂȂ�܂ŌJ��Ԃ�
		{
			ptTexture = pGetCharTextureObj->GetWidthHeight(*lpStr); //�����̃e�N�X�`���̕��ƍ������擾
			Ratio = (double)Height / (double)pGetCharTextureObj->GetLOGFONT().lfHeight; ////�e�N�X�`���Ǝ��ۂɕ`�悷��T�C�Y�̍����̔䗦���v�Z
			ptTexture.x = LONG(ptTexture.x * Ratio);
			ptTexture.y = LONG(ptTexture.y * Ratio);

			if (LimitWidth) //������̕��������ݒ肳��Ă���ꍇ
				if (x + ptTexture.x - BeginX > LimitWidth) //���̕�����`�悷��Ɛ������𒴂��Ă��܂��ꍇ
					break; //while���[�v�𔲂���

			//���_����ݒ�
			vt[0].SetVertex((float)x - 0.5f, (float)y - 0.5f, 0, dwColor, 0.0f, 0.0f);
			vt[1].SetVertex(float(x + ptTexture.x - 0.5f), (float)y - 0.5f, 0, dwColor, 1.0f, 0.0f);
			vt[2].SetVertex((float)x - 0.5f, float(y + ptTexture.y - 0.5f), 0, dwColor, 0.0f, 1.0f);
			vt[3].SetVertex(float(x + ptTexture.x - 0.5f), float(y + ptTexture.y - 0.5f), 0, dwColor, 1.0f, 1.0f);

			pD3DDevice->SetTexture(0, pGetCharTextureObj->GetTexture(*lpStr)); //�����̃e�N�X�`�����f�o�C�X�ɃZ�b�g
			pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, Index, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //�`��
			Drawed++;

			x += ptTexture.x; //���`���������̕��𑫂��A���̕����̕`��ʒu�ɂ���

			lpStr++; //���̕����փ|�C���^���ړ�
		}
	}

	pD3DDevice->SetFVF(OldFVF); //���_�t�H�[�}�b�g�����ɖ߂��Ă���
	pD3DDevice->SetTexture(0, cpOldTexture); //�e�N�X�`����߂��Ă���

	return Drawed;
}

}
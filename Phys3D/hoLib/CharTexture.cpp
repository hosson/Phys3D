//�����e�N�X�`���������N���X
#include "CharTexture.h"
#include <Windows.h>
#include "hoLib.h"
#include "LibError.h"

namespace ho {
	//�R���X�g���N�^
	GetCharTexture::GetCharTexture(LOGFONT lf, LPDIRECT3DDEVICE9 pD3DDevice)
	{
		this->pD3DDevice = pD3DDevice;

		Objects = 0;
		ZeroMemory(&pCharTextureObj, sizeof(CharTexture *) * 65536); //�|�C���^�z���������

		this->lf = lf; //�������镶���̃t�H���g����ۑ�
	}

	//�f�X�g���N�^
	GetCharTexture::~GetCharTexture()
	{
		DeleteAllObj(); //�S�ẴI�u�W�F�N�g���폜
	}

	//�S�ẴI�u�W�F�N�g���폜
	void GetCharTexture::DeleteAllObj()
	{
		for (int i = 0; i < 65536; i++)
			SDELETE(pCharTextureObj[i]);

		Objects = 0;

		return;
	}

	//�C�ӂ̕����̃e�N�X�`���擾
	LPDIRECT3DTEXTURE9 GetCharTexture::GetTexture(TCHAR Char)
	{
		if (!pCharTextureObj[Char]) //�e�N�X�`�����܂����݂��Ȃ��ꍇ
		{
			pCharTextureObj[Char] = new CharTexture(pD3DDevice, Char, this); //�e�N�X�`���I�u�W�F�N�g���쐬
			Objects++;
		}

		return pCharTextureObj[Char]->GetTexture();
	}

	//�C�ӂ̕����̃e�N�X�`���̕��ƍ������擾
	POINT GetCharTexture::GetWidthHeight(TCHAR Char)
	{
		if (!pCharTextureObj[Char]) //�e�N�X�`�����܂����݂��Ȃ��ꍇ
		{
			pCharTextureObj[Char] = new CharTexture(pD3DDevice, Char, this); //�e�N�X�`���I�u�W�F�N�g���쐬
			Objects++;
		}

		return pCharTextureObj[Char]->GetWidthHeight();
	}













	//�R���X�g���N�^
	CharTexture::CharTexture(LPDIRECT3DDEVICE9 pD3DDevice, TCHAR MakeChar, GetCharTexture *pGetCharTexture) 
	{
		this->pGetCharTexture = pGetCharTexture;

		MakeTexture(pD3DDevice, MakeChar, pGetCharTexture->GetLOGFONT()); //�����̃e�N�X�`���쐬
	}

	//�f�X�g���N�^
	CharTexture::~CharTexture() 
	{
		RELEASE(pD3DTexture);
	}


	BOOL CharTexture::MakeTexture(LPDIRECT3DDEVICE9 pD3DDevice, TCHAR MakeChar, LOGFONT lf)
	{
		HRESULT hr;
		BOOL Space = FALSE; //�쐬���镶�����X�y�[�X���Ńe�N�X�`���T�C�Y��0�̏ꍇTRUE�ɂȂ�

		HDC hDC = CreateCompatibleDC(NULL); //���݂̃X�N���[���ƌ݊����̂���f�o�C�X�R���e�L�X�g�n���h�����擾

		//�f�o�C�X�R���e�L�X�g�n���h���Ƀt�H���g��ݒ�
		HFONT hFont = CreateFontIndirect(&lf);
		SelectObject(hDC, hFont);

		// �����R�[�h�擾
		TCHAR *c = &MakeChar;
		UINT code = 0;
	#if _UNICODE //unicode�̏ꍇ
		code = (UINT)*c; //�����R�[�h�͒P���Ƀ��C�h������UINT�ϊ�
	#else
		// 2�o�C�g�����̃R�[�h��[�����R�[�h]*256 + [�擱�R�[�h]
		if (IsDBCSLeadByte((BYTE)MakeChar)) //�w�肳�ꂽ�������擱�R�[�h�������ꍇ
			code = (BYTE)c[0] << 8 | (BYTE)c[1];
		else
			code = c[0];
	#endif

		//�����̃r�b�g�}�b�v�f�[�^�擾
		GLYPHMETRICS gm;
		CONST MAT2 Mat = {{0,1}, {0,0}, {0,0}, {0,1}};
		DWORD dwSize = GetGlyphOutline(hDC, code, GGO_GRAY8_BITMAP, &gm, 0, NULL, &Mat); //�K�v�̃o�b�t�@�T�C�Y�擾
		if (!dwSize) //�X�y�[�X�ȂǂŃT�C�Y��0�������ꍇ
		{
			dwSize = lf.lfHeight * (lf.lfHeight / 2); //LOGFONT�̏c���Əc��/2�̉����̃T�C�Y���w��
			Space = TRUE;
		}
		LPBYTE lpBuf = (LPBYTE)GlobalAlloc(GPTR, dwSize); //�������擾
		GetGlyphOutline(hDC, code, GGO_GRAY8_BITMAP, &gm, dwSize, lpBuf, &Mat);

		//TEXTMETRIC�擾
		TEXTMETRIC tm;
		GetTextMetrics(hDC, &tm);

		//��̃e�N�X�`���쐬
		if (Space) //�������X�y�[�X�̏ꍇ
		{
			Width = lf.lfHeight / 2;
			Height = lf.lfHeight;
		} else {
			Width = gm.gmCellIncX;
			Height = tm.tmHeight;
		}
		hr = pD3DDevice->CreateTexture(Width, Height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pD3DTexture, NULL); 
		if (FAILED(hr))
		{
			tstring  str;
			ho::tPrint(str, TEXT("[%s] �e�N�X�`���̍쐬�Ɏ��s���܂����B\nWIDTH %d  HEIGHT %d"), &MakeChar, Width, Height);
			ER(str.c_str(), __WFILE__, __LINE__, true);
			Direct3D::GetErrorStr(str, hr);
			ER(str.c_str(),  __WFILE__, __LINE__, true);
			return FALSE;
		}
		//�e�N�X�`�������b�N
		D3DLOCKED_RECT lockRect;
		hr = pD3DTexture->LockRect(0, &lockRect, NULL, 0);
		if (FAILED(hr))
		{
			tstring  str;
			ho::tPrint(str, TEXT("[%s] �e�N�X�`���̃��b�N�Ɏ��s���܂����B\nWIDTH %d  HEIGHT %d"), &MakeChar, Width, Height);
			ER(str.c_str(),  __WFILE__, __LINE__, true);
			Direct3D::GetErrorStr(str, hr);
			ER(str.c_str(),  __WFILE__, __LINE__, true);
			return FALSE;
		}

		//�e�N�X�`��������
		ZeroMemory(lockRect.pBits, lockRect.Pitch * Height);

		//�t�H���g�r�b�g�}�b�v�̃s�b�`�v�Z
		int pitch = (gm.gmBlackBoxX + 3) & 0xfffc; //�r�b�g�}�b�v�̃s�b�`�͕K��4�̔{���ɂȂ邽��

		//�t�H���g�̃r�b�g�}�b�v���e�N�X�`���֏�������
		if (!Space) //�������X�y�[�X�ł͂Ȃ��ꍇ
		{
			BYTE src;
			int OffsetX = gm.gmptGlyphOrigin.x; //�I�t�Z�b�g�ʒu��X���W
			int OffsetY = tm.tmAscent - gm.gmptGlyphOrigin.y; //�I�t�Z�b�g�ʒu��Y���W
			int xMax = gm.gmBlackBoxX; //�����o�������̉E�[
			if (xMax > Width) //�e�N�X�`���̕�Width�𒴂��Ȃ��悤�ɂ���iGetGlyphOutline�ɂ��`���o���͕���4Pixel�P�ʂƂȂ��Ă��邽�߁j
				xMax = Width;

			for (int y = 0; y < int(gm.gmBlackBoxY); y++)
			{
				for (int x = 0; x < xMax; x++)
				{
					if ((y + OffsetY) * Width + x + OffsetX >= Width * Height)
					{
						goto LABEL1;
					}
					src = (*(lpBuf + y * pitch + x) * 255) / 64; //���l�̎擾
					*((LPDWORD)lockRect.pBits + (y + OffsetY) * Width + x + OffsetX) = 0x00ffffff | (src << 24);
				}
			}
		}
	LABEL1:

		//�e�N�X�`�����A�����b�N
		pD3DTexture->UnlockRect(0);

		//�������J��
		GlobalFree(lpBuf);
		DeleteObject(hFont);
		DeleteDC(hDC);

		return TRUE;
	}

}

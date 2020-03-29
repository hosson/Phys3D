#include "DrawDebug.h"
#include "Direct3D.h"
#include "Debug.h"
#include "D3DResource.h"

namespace ho
{
	//�R���X�g���N�^
	DrawDebug::DrawDebug(Direct3D *pDirect3DObj)
	{
		this->pDirect3DObj = pDirect3DObj;
		LPDIRECT3DDEVICE9 pD3DDevice = pDirect3DObj->GetpD3DDevice();
		HRESULT hr;
		Enable = TRUE;

		//�f�o�b�O�\���p�t�H���g�̐ݒ�
		D3DXFONT_DESC FontDesc;
		ZeroMemory(&FontDesc, sizeof(D3DXFONT_DESC));
		FontDesc.Height = 16;
	
		//�t�H���g�쐬
		hr = D3DXCreateFontIndirect(pD3DDevice, &FontDesc, cpD3DDbgFont.ToCreator());
		if (FAILED(hr))
		{
			tstring str;
			Direct3D::GetErrorStr(str, hr);
			str = TEXT("�t�H���g�쐬�Ɏ��s���܂����B") + str;
			ER(str.c_str(), __WFILE__, __LINE__, true);
			Enable =  FALSE; //�������Ɏ��s����
		} else {
			pDirect3DObj->GetpD3DResourceManagerObj()->Regist(new ho::Font(cpD3DDbgFont)); //���\�[�X�}�l�[�W���ɓo�^
		}

		//�t�H���g�\���p�X�v���C�g�쐬
		hr = D3DXCreateSprite(pD3DDevice, cpD3DDbgSprite.ToCreator());
		if (FAILED(hr))
		{
			tstring str;
			Direct3D::GetErrorStr(str, hr);
			str = TEXT("�X�v���C�g�쐬�Ɏ��s���܂����B") + str;
			ER(str.c_str(), __WFILE__, __LINE__, true);
			Enable =  FALSE; //�������Ɏ��s����
		} else {
			pDirect3DObj->GetpD3DResourceManagerObj()->Regist(new ho::Sprite(cpD3DDbgSprite)); //���\�[�X�}�l�[�W���ɓo�^
		}
	}

	//�f�X�g���N�^
	DrawDebug::~DrawDebug()
	{
	}

	//�������ǉ�
	void DrawDebug::Print(TCHAR *pStr)
	{
		listpStr.push_back(tstring(pStr)); //����������X�g�ɒǉ�

		return;
	}

	//�`��
	void DrawDebug::Draw(int Y)
	{
		if (!Enable)
			return;

		cpD3DDbgSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE); //�X�v���C�g�`��J�n

		RECT rect;
		int LineHeight = 20; //1�s�̍���
		for (std::list<tstring>::iterator itr = listpStr.begin(); itr != listpStr.end(); itr++) //�����񃊃X�g�𑖍�
		{
			SetRect(&rect, 0, Y, 200, LineHeight);
			cpD3DDbgFont->DrawText(cpD3DDbgSprite, itr->c_str(), -1, &rect, DT_NOCLIP, 0xff00ff00);
			Y += LineHeight;
		}

		cpD3DDbgSprite->End(); //�X�v���C�g�`��I��

		listpStr.clear(); //�����񃊃X�g������

		return;
	}
}

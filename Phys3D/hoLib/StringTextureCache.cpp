#include "StringTextureCache.h"
#include "DrawString.h"
#include "CharTexture.h"
#include "Debug.h"
#include "hoLib.h"
#include "Direct3D.h"
#include "D3DResource.h"

//#include "DetectMemoryLeak.h"


namespace ho
{
	//�R���X�g���N�^
	StringTextureCacheManager::StringTextureCacheManager(LPDIRECT3DDEVICE9 pD3DDevice, DrawString *pDrawStringObj)
	{
		this->pD3DDevice = pD3DDevice;
		this->pDrawStringObj = pDrawStringObj;
		this->Time = 0;
	}

	//�f�X�g���N�^
	StringTextureCacheManager::~StringTextureCacheManager()
	{
		Clear(); //�S�ẴL���b�V�����폜
	}

	//�C�ӂ̕�����̃e�N�X�`���̃L���b�V�����擾
	StringTextureCache *StringTextureCacheManager::GetpStringTextureCache(TCHAR *pStr, int Height, int LimitWidth)
	{
		for (std::list<StringTextureCache *>::iterator itr = listpStringTextureCacheObj.begin(); itr != listpStringTextureCacheObj.end(); itr++) //������e�N�X�`���L���b�V���̃��X�g�𑖍�
		{
			if ((*itr)->Compare(pStr)) //�L���b�V�������������ꍇ
			{
				(*itr)->Use(Time); //�L���b�V�����g�p����邱�Ƃ�`����
				return *itr; //�L���b�V����Ԃ�
			}
		}

		if (listpStringTextureCacheObj.size() == 500) //�L���b�V���T�C�Y��500�𒴂����ꍇ
			ER(TEXT("������e�N�X�`���L���b�V���̍œK���̌ĂіY����m�F���Ă��������B"), __WFILE__, __LINE__, true); //�G���[���b�Z�[�W���M

		//�L���b�V����������Ȃ������̂ŁA�L���b�V�����쐬���Ă����Ԃ�
		StringTextureCache *pStringTextureCacheObj = new StringTextureCache(pStr, Height, LimitWidth, pD3DDevice, this); //������e�N�X�`���L���b�V�����쐬
		listpStringTextureCacheObj.push_back(pStringTextureCacheObj);
		return pStringTextureCacheObj;
	}

	//�œK��
	void StringTextureCacheManager::Optimize() 
	{
		//�����ԂȂǂ̂��������g�p�ȃL���b�V�����폜
		for (std::list<StringTextureCache *>::iterator itr = listpStringTextureCacheObj.begin(); itr != listpStringTextureCacheObj.end();) //������e�N�X�`���L���b�V���̃��X�g�𑖍�
		{
			if ((*itr)->GetLastUseTime() < Time) //�O��̍œK���ȍ~�g���Ă��Ȃ��ꍇ
			{
				SDELETE(*itr);
				itr = listpStringTextureCacheObj.erase(itr);
			} else {
				itr++;
			}
		}

		Time++;

		return;
	}

	//�S�ẴL���b�V�����폜
	void StringTextureCacheManager::Clear()
	{
		for (std::list<StringTextureCache *>::iterator itr = listpStringTextureCacheObj.begin(); itr != listpStringTextureCacheObj.end();) //������e�N�X�`���L���b�V���̃��X�g�𑖍�
		{
			SDELETE(*itr);
			itr = listpStringTextureCacheObj.erase(itr);
		}

		return;
	}







	//�R���X�g���N�^
	StringTextureCache::StringTextureCache(TCHAR *pStr, int Height, int LimitWidth, LPDIRECT3DDEVICE9 pD3DDevice, StringTextureCacheManager *pStringTextureCacheManagerObj)
	{

		this->pStringTextureCacheManagerObj = pStringTextureCacheManagerObj;
		this->str = pStr;
		this->CharNum = 0;
		this->LastUseTime = pStringTextureCacheManagerObj->GetTime();

		LOGFONT lf = pStringTextureCacheManagerObj->GetpDrawStringObj()->GetpGetCharTextureObj()->GetLOGFONT(); //�t�H���g�����擾

		CalcPtStringSize(&this->ptTextureSize, &this->CharNum, pStr, Height, LimitWidth, &lf); //������e�N�X�`���̃T�C�Y���v�Z����

		CreateCacheTexture(Height, LimitWidth, pD3DDevice, pStringTextureCacheManagerObj, &lf); //�L���b�V���ƂȂ�e�N�X�`�����쐬����
	}

	//�L���b�V���ƂȂ�e�N�X�`�����쐬����
	void StringTextureCache::CreateCacheTexture(int Height, int LimitWidth, LPDIRECT3DDEVICE9 pD3DDevice, StringTextureCacheManager *pStringTextureCacheManagerObj, LOGFONT *pLF) 
	{
		HRESULT hr = pD3DDevice->CreateTexture(ptTextureSize.x, ptTextureSize.y, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, cpTexture.ToCreator(), NULL); //������̃e�N�X�`�����쐬

		if (FAILED(hr))
		{
			tstring  str;
			ho::tPrint(str, TEXT("[%s] �e�N�X�`���̍쐬�Ɏ��s���܂����B\nWIDTH %d  HEIGHT %d"), (TCHAR *)str.c_str(), ptTextureSize.x, ptTextureSize.y);
			ER(str.c_str(), __WFILE__, __LINE__, true);
			Direct3D::GetErrorStr(str, hr);
			ER(str.c_str(), __WFILE__, __LINE__, true);
		} else {
			pStringTextureCacheManagerObj->GetpDrawStringObj()->GetpDirect3DObj()->GetpD3DResourceManagerObj()->Regist(new ho::Texture(cpTexture)); //�e�N�X�`�������\�[�X�Ǘ��N���X�ɓo�^

			D3DVERTEX_2D vt[4]; //�`��p���_���
			POINT ptTexture; //�����e�N�X�`���T�C�Y�擾�p
			int x = 0; //�`��ʒu
			DWORD dwColor = 0xffffffff;

			ho::ComPtr<IDirect3DBaseTexture9 *> cpOldTexture;
			pD3DDevice->GetTexture(0, cpOldTexture.ToCreator()); //���݂̃e�N�X�`����ۑ�

			DWORD AlphaEnable;
			pD3DDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &AlphaEnable); //�A���t�@�u�����h�̏�Ԃ�ۑ�
			pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE); //�A���t�@�u�����h��off

			ho::ComPtr<LPDIRECT3DSURFACE9> cpOldSurface;
			hr = pD3DDevice->GetRenderTarget(0, cpOldSurface.ToCreator()); //���݂̃����_�[�^�[�Q�b�g��ۑ�

			ho::ComPtr<LPDIRECT3DSURFACE9> cpTexSurface; //������e�N�X�`���̃T�[�t�F�C�X�ւ̃|�C���^
			hr = cpTexture->GetSurfaceLevel(0, cpTexSurface.ToCreator()); //������e�N�X�`���̃T�[�t�F�C�X���擾
			hr = pD3DDevice->SetRenderTarget(0, cpTexSurface); //�����_�����O�Ώۂ𕶎���e�N�X�`���̃T�[�t�F�C�X�֐ݒ�

			//�������e�N�X�`����֕`��
			int i = 0;
			for (TCHAR *p = (TCHAR *)str.c_str(); *p != NULL; p++) //�������擪����1����������
			{
				if (i >= this->CharNum)
					break;

				ptTexture = pStringTextureCacheManagerObj->GetpDrawStringObj()->GetpGetCharTextureObj()->GetWidthHeight(*p); //�e�N�X�`���̃T�C�Y���擾
				double Ratio = (double)Height / (double)pLF->lfHeight; //�e�N�X�`���Ǝ��ۂɕ`�悷��T�C�Y�̍����̔䗦���v�Z
				ptTexture.x = int(ptTexture.x * Ratio);
				ptTexture.y = int(ptTexture.y * Ratio);
	
				pD3DDevice->SetTexture(0, pStringTextureCacheManagerObj->GetpDrawStringObj()->GetpGetCharTextureObj()->GetTexture(*p)); //�e�N�X�`�����Z�b�g
	
				//���_����ݒ�
				vt[0].SetVertex((float)x, (float)0, 0, dwColor, 0.0f, 0.0f);
				vt[1].SetVertex(float(x + ptTexture.x), (float)0, 0, dwColor, 1.0f, 0.0f);
				vt[2].SetVertex((float)x, float(ptTexture.y), 0, dwColor, 0.0f, 1.0f);
				vt[3].SetVertex(float(x + ptTexture.x), float(ptTexture.y), 0, dwColor, 1.0f, 1.0f);
				D3DVERTEX_2D::Offset(vt, 4);

				pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //�`��

				x += ptTexture.x; //�`��ʒu���E�ֈړ�
				i++;
			}

			pD3DDevice->SetTexture(0, cpOldTexture); //�e�N�X�`�������ɖ߂��Ă���
			pD3DDevice->SetRenderTarget(0, cpOldSurface); //�����_�����O�Ώۂ����Ƃ̃T�[�t�F�C�X�֖߂��Ă���
			pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, AlphaEnable); //�A���t�@�u�����h�����ɖ߂��Ă���
		}

		return;
	}

	//�f�X�g���N�^
	StringTextureCache::~StringTextureCache()
	{
	}

	//�����̕�����ƃe�N�X�`���̕����񂪓������ǂ������r����
	BOOL StringTextureCache::Compare(TCHAR *pStr) 
	{
		if (str.compare(pStr) == 0) //�����񂪈�v�����ꍇ
			return TRUE;

		return FALSE;
	}

	//������̃T�C�Y���v�Z����
	void StringTextureCache::CalcPtStringSize(ho::PT<int> *pptSize, int *pCharNum, TCHAR *pStr, int Height, int LimitWidth, LOGFONT *pLF) 
	{
		*pCharNum = 0;
		POINT Point;
		ho::PT<int> ptSize(0, 0);
		for (TCHAR *p = pStr; *p != NULL; p++) //�������擪����1����������
		{
			Point = this->pStringTextureCacheManagerObj->GetpDrawStringObj()->GetpGetCharTextureObj()->GetWidthHeight(*p);
			double Ratio = (double)Height / (double)pLF->lfHeight; //�e�N�X�`���Ǝ��ۂɕ`�悷��T�C�Y�̍����̔䗦���v�Z
			Point.x = LONG(Point.x * Ratio);
			Point.y = LONG(Point.y * Ratio);

			if (LimitWidth) //���������ݒ肳��Ă���ꍇ
				if (ptSize.x + Point.x > LimitWidth) //����ȏ�̕������͉��������𒴂��Ă��܂��ꍇ
					break;

			ptSize.x += Point.x;
			if (ptSize.y < Point.y)
				ptSize.y = Point.y;
			(*pCharNum)++;
		}

		*pptSize = ptSize;

		return;
	}
	
}


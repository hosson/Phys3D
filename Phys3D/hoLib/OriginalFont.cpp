#include "OrigianlFont.h"
#include "Direct3D.h"
#include "LibError.h"

namespace ho
{
	//�R���X�g���N�^
	OriginalFontManager::OriginalFontManager()
	{
	}

	//�f�X�g���N�^
	OriginalFontManager::~OriginalFontManager()
	{
		for (DWORD i = 0; i < vectorpOriginalFontObj.size(); i++)
			SDELETE(vectorpOriginalFontObj.at(i));
	}

	//�Ǝ��t�H���g�I�u�W�F�N�g��ǉ�
	void OriginalFontManager::Add(OriginalFont *pOriginalFontObj, DWORD Index)
	{
		if (vectorpOriginalFontObj.size() <= Index)
			vectorpOriginalFontObj.resize(Index + 1);

		vectorpOriginalFontObj.at(Index) = pOriginalFontObj;

		return;
	}

	//�Ǝ��t�H���g�I�u�W�F�N�g���擾
	OriginalFont *OriginalFontManager::GetpOriginalFontObj(DWORD Index)
	{
		if (vectorpOriginalFontObj.size() <= Index) //�͈͊O�̏ꍇ
		{
			ER(TEXT("�͈͊O�̓Ǝ��t�H���g���I������܂����B"), __WFILE__, __LINE__, false);
			return NULL;
		}

		return vectorpOriginalFontObj.at(Index);
	}





	//�R���X�g���N�^
	OriginalFont::OriginalFont(LPDIRECT3DDEVICE9 pD3DDevice, const TCHAR *pArchiveName, const TCHAR *pTextureFilename, const TCHAR *pFontInfoFilename, bool Debug)
	{
		//�t�H���g�̃e�N�X�`����ǂ�
		ho::Archive ArchiveObj(pArchiveName, Debug); //�A�[�J�C�u���J��

		void *p = ArchiveObj.GetFilePointer(pTextureFilename); //�t�@�C���|�C���^���擾
		DWORD FileSize = ArchiveObj.GetFileSize(); //�t�@�C���T�C�Y�擾

		D3DXIMAGE_INFO d3dxii;
		try
		{
			HRESULT hr;

			if (FAILED(hr = D3DXGetImageInfoFromFileInMemory(p, FileSize, &d3dxii)))
			{
				ho::tstring str;
				ho::Direct3D::GetErrorStr(str, hr);
				str = TEXT("�e�N�X�`�����̎擾�Ɏ��s���܂����B") + str;
				throw ho::Exception(str, __WFILE__, __LINE__, false);
			}

			if (FAILED(hr = D3DXCreateTextureFromFileInMemoryEx(pD3DDevice,	p, FileSize, d3dxii.Width, d3dxii.Height, 0, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR, 0, &d3dxii, NULL, &pTexture))) //����������e�N�X�`�����쐬
			{
				ho::tstring str;
				ho::Direct3D::GetErrorStr(str, hr);
				str = TEXT("�e�N�X�`�����J���̂Ɏ��s���܂����B") + str;
				throw ho::Exception(str, __WFILE__, __LINE__, false);
			}
		}
		catch (ho::Exception &e)
		{
			ER(e.strMsg.c_str(), e.pFile, e.Line, e.Continue);
		}
	
		ArchiveObj.EndFilePointer(); //�t�@�C���|�C���^���I��


		//�t�H���g���t�@�C�����當���I�u�W�F�N�g���쐬
		ZeroMemory(pFontCharObj, sizeof(FontChar *) * 65536); //�����N���X�ւ̃|�C���^�}�b�v��������

		
		p = ArchiveObj.GetFilePointer(pFontInfoFilename); //�t�H���g���t�@�C���̃|�C���^���擾

		ho::CSVReader CSVReaderObj(p); //CSV���[�_�I�u�W�F�N�g���쐬

		ho::tstring str;
		int x, y, w, h; //�t�H���g���t�@�C���Ɋi�[����Ă����e�N�X�`����̃s�N�Z�����W
		float tu1, tu2, tv1, tv2;
		while (true)
		{
			CSVReaderObj.Read(&str); //������ǂ�
			if (str.size() == 0) //�������󔒂������ꍇ
				break; //�t�@�C���I�[�ɓ��B�����̂Ń��[�v�𔲂���

			CSVReaderObj.Read(&x);
			CSVReaderObj.Read(&y);
			CSVReaderObj.Read(&w);
			CSVReaderObj.Read(&h);

			//�s�N�Z�����W����e�N�Z�����W���v�Z
			tu1 = float((double)x / (double)d3dxii.Width);
			tv1 = float((double)y / (double)d3dxii.Height);
			tu2 = float(double(x + w) / (double)d3dxii.Width);
			tv2 = float(double(y + h) / (double)d3dxii.Height);

			pFontCharObj[str.at(0)] = new FontChar(w, h, tu1, tu2, tv1, tv2); //�����I�u�W�F�N�g���쐬���ă|�C���^�}�b�v�Ɋi�[
		}

		ArchiveObj.EndFilePointer(); //�A�[�J�C�u�̃t�@�C���|�C���^�g�p���I������
		
	}

	//�f�X�g���N�^
	OriginalFont::~OriginalFont()
	{
		for (int i = 0; i < 65536; i++)
			SDELETE(pFontCharObj[i]);

		RELEASE(pTexture);
	}

	//�C�ӂ̕�����`��ɕK�v�Ȓ��_���̔z��𐶐�����
	void OriginalFont::CreateDrawVt(TCHAR *pStr, float z, std::vector<D3DXVECTOR3> &vectorvecVt, std::vector<D3DXVECTOR2> &vectorvecUV, std::vector<WORD> &vectorIndex, int Height)
	{
		vectorvecVt.clear();
		vectorvecUV.clear();
		vectorIndex.clear();

		float x = 0, y = 0; //���_�ʒu
		D3DXVECTOR2 *pUV;
		WORD Index = 0;
		float w, h;
		double Ratio;
		while (*pStr) //������ NULL �ɂȂ�܂ő���
		{
			//���_���W��ǉ�
			w = (float)pFontCharObj[*pStr]->GetptWH().x;
			h = (float)pFontCharObj[*pStr]->GetptWH().y;
			if (Height) //�������w�肳��Ă���ꍇ
				Ratio = Height / h; //�T�C�Y�̔䗦���v�Z
			else
				Ratio = 1.0;
			w = float(w * Ratio);
			h = float(h * Ratio);
			vectorvecVt.push_back(D3DXVECTOR3(x, y, z));
			vectorvecVt.push_back(D3DXVECTOR3(x + w, y, z));
			vectorvecVt.push_back(D3DXVECTOR3(x, y + h, z));
			vectorvecVt.push_back(D3DXVECTOR3(x + w, y + h, z));
			x += w; //���_���W�����̕����̈ʒu�֑���

			//UV���W��ǉ�
			pUV = pFontCharObj[*pStr]->Getpuv(); //�����̃e�N�X�`������4�_�̍��W�z��ւ̃|�C���^���擾
			for (int i = 0; i < 4; i++)
				vectorvecUV.push_back(pUV[i]); //UV ���W��ǉ�

			//���_�C���f�b�N�X��ǉ�
			vectorIndex.push_back(Index);
			vectorIndex.push_back(Index + 1);
			vectorIndex.push_back(Index + 2);
			vectorIndex.push_back(Index + 2);
			vectorIndex.push_back(Index + 1);
			vectorIndex.push_back(Index + 3);
			Index += 4;

			pStr++; //���̕����ֈړ�
		}

		return;
	}

	//������ƍ�������`�掞�̕����v�Z����
	float OriginalFont::GetWidth(TCHAR *pStr, int Height) 
	{
		float x = 0; //���_�ʒu
		float w, h;
		double Ratio;

		while (*pStr) //������ NULL �ɂȂ�܂ő���
		{
			//���_���W��ǉ�
			w = (float)pFontCharObj[*pStr]->GetptWH().x;
			h = (float)pFontCharObj[*pStr]->GetptWH().y;
			if (Height) //�������w�肳��Ă���ꍇ
				Ratio = Height / h; //�T�C�Y�̔䗦���v�Z
			else
				Ratio = 1.0;
			w = float(w * Ratio);
			h = float(h * Ratio);
			x += w; //���_���W�����̕����̈ʒu�֑���



			pStr++; //���̕����ֈړ�
		}

		return x;
	}




	//�R���X�g���N�^
	FontChar::FontChar(int Width, int Height, float tu1, float tu2, float tv1, float tv2)
	{
		ptWH = ho::PT<int>(Width, Height);

		uv[0] = D3DXVECTOR2(tu1, tv1);
		uv[1] = D3DXVECTOR2(tu2, tv1);
		uv[2] = D3DXVECTOR2(tu1, tv2);
		uv[3] = D3DXVECTOR2(tu2, tv2);
	}

	//�f�X�g���N�^
	FontChar::~FontChar()
	{
	}
}

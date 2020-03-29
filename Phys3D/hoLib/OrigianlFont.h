//�摜����Ǝ��t�H���g���g���N���X
#pragma once
#include <vector>
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "CSVReader.h"
#include "Archive.h"
#include "PT.h"

namespace ho
{
	class OriginalFont;
	class FontChar;

	class OriginalFontManager
	{
	public:
		OriginalFontManager(); //�R���X�g���N�^
		~OriginalFontManager(); //�f�X�g���N�^

		void Add(OriginalFont *pOriginalFontObj, DWORD Index); //�Ǝ��t�H���g�I�u�W�F�N�g��ǉ�
		OriginalFont *GetpOriginalFontObj(DWORD Index); //�Ǝ��t�H���g�I�u�W�F�N�g���擾
	private:
		std::vector<OriginalFont *>vectorpOriginalFontObj;
	};

	class OriginalFont
	{
	public:
		OriginalFont(LPDIRECT3DDEVICE9 pD3DDevice, const TCHAR *pArchiveName, const TCHAR *pTextureFilename, const TCHAR *pFontInfoFilename, bool Debug); //�R���X�g���N�^
		virtual ~OriginalFont(); //�f�X�g���N�^

		void CreateDrawVt(TCHAR *pStr, float z, std::vector<D3DXVECTOR3> &vectorvecVt, std::vector<D3DXVECTOR2> &vectorvecUV, std::vector<WORD> &vectorIndex, int Height = 0); //�C�ӂ̕�����`��ɕK�v�Ȓ��_���̔z��𐶐�����
		float GetWidth(TCHAR *pStr, int Height); //������ƍ�������`�掞�̕����v�Z����
	protected:
		LPDIRECT3DTEXTURE9 pTexture;
	private:
		FontChar *pFontCharObj[65536]; //�����}�b�v
	};

	//1������\���N���X
	class FontChar
	{
	public:
		FontChar(int Width, int Height, float tu1, float tu2, float tv1, float tv2); //�R���X�g���N�^
		~FontChar(); //�f�X�g���N�^

		//�A�N�Z�b�T
		ho::PT<int> &GetptWH() { return ptWH; }
		D3DXVECTOR2 *Getpuv() { return uv; }
	private:
		ho::PT<int> ptWH; //���ƍ����i�s�N�Z���P�ʁj
		D3DXVECTOR2 uv[4]; //�e�N�X�`����̍��W
	};
}
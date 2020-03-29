//Direct3D �����p�̃f�o�b�O�`��N���X

#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <list>
#include "ComPtr.hpp"
#include "tstring.h"

namespace ho
{
	class Direct3D;

	class DrawDebug
	{
	public:
		DrawDebug(Direct3D *pDirect3DObj); //�R���X�g���N�^
		~DrawDebug(); //�f�X�g���N�^

		void Print(TCHAR *pStr); //�������ǉ�
		void Draw(int Y = 0); //�`��
	private:
		Direct3D *pDirect3DObj;
		std::list<tstring> listpStr; //�`�敶���񃊃X�g
		ho::ComPtr<LPD3DXFONT> cpD3DDbgFont; //�f�o�b�O�\���p�t�H���g
		ho::ComPtr<LPD3DXSPRITE> cpD3DDbgSprite; //�f�o�b�O�\���p�X�v���C�g
		BOOL Enable; //�������������I���������ǂ���
	};
}

/*Direct3D�N���X�������Ɏ����I�ɍ����B

�v���O�������Ƀf�o�b�O��������o�͂������ꏊ�� Print() ���\�b�h���Ăяo���B
IDirect3DDevice9::Presetn() �̎�O������� Draw() ���\�b�h���Ăяo���ƁA
Print() ���\�b�h�œo�^���ꂽ�����񂪕`�悳���B���̂Ƃ������̕�����o�b�t�@��
�N���A�����B

*/
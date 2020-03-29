//�����e�N�X�`���N���X�Q
#pragma once
#include "Direct3D.h"
#include <windows.h>

namespace ho {

	class CharTexture; //�O���錾

	//CharTexture�N���X����K�؂ȕ����𓾂�N���X
	class GetCharTexture
	{
	public:
		GetCharTexture(LOGFONT lf, LPDIRECT3DDEVICE9 pD3DDevice); //�R���X�g���N�^
		~GetCharTexture(); //�f�X�g���N�^
		LPDIRECT3DTEXTURE9 GetTexture(TCHAR Char); //�C�ӂ̕����̃e�N�X�`���擾
		POINT GetWidthHeight(TCHAR Char); //�C�ӂ̕����̃e�N�X�`���̕��ƍ������擾

		LOGFONT GetLOGFONT() { return lf; }
		int GetObjects() { return Objects; }

	private:
		LOGFONT lf;
		int Objects; //�쐬���ꂽ�e�N�X�`���̐�
		CharTexture *pCharTextureObj[65536]; //2Byte�̕����R�[�h���̃|�C���^�z��

		LPDIRECT3DDEVICE9 pD3DDevice;
		void DeleteAllObj(); //�S�ẴI�u�W�F�N�g���폜

	};

	//�����e�N�X�`�������N���X
	class CharTexture
	{
	public:
		CharTexture(LPDIRECT3DDEVICE9 pD3DDevice, TCHAR MakeChar,  GetCharTexture *pGetCharTexture); //�R���X�g���N�^
		~CharTexture(); //�f�X�g���N�^
		LPDIRECT3DTEXTURE9 GetTexture() { return pD3DTexture; }
		POINT GetWidthHeight() { POINT pt = {Width, Height}; return pt; }
	private:
		int Width, Height; //�e�N�X�`���̃T�C�Y
		LPDIRECT3DTEXTURE9 pD3DTexture; //�����̏������܂ꂽ�e�N�X�`��
		TCHAR Character; //�ێ����Ă��镶��
		GetCharTexture *pGetCharTexture; //�e�N���X�̃I�u�W�F�N�g�ւ̃|�C���^

		BOOL MakeTexture(LPDIRECT3DDEVICE9 pD3DDevice, TCHAR MakeChar, LOGFONT); //�e�N�X�`���쐬
	};

}

/*�g�p���@�Ȃ�

�v���O�����J�n���ɕ����\���Ɏg�p����t�H���g����LOGFONT�\���̂ɓ����GetCharTexture�I�u�W�F�N�g��1��
�쐬����BLOGFONT�\���̂̃t�H���g�T�C�Y�̃e�N�X�`��������邱�ƂɂȂ�B
�e�N�X�`���𓾂�ɂ́AGetCharTexture�N���X��GetTexture()�֐��ցA�����������̕����R�[�h��n���B
�e�N�X�`���̐����Ɋւ��ẮAGetTexture()���Ă΂ꂽ���_�ł܂��쐬����Ă��Ȃ������͍쐬����A
���łɍ쐬���ꂽ�����Ɋւ��Ă͉ߋ��ɍ�����e�N�X�`�����g����B�܂�A��x������e�N�X�`���́A
GetCharTexture�I�u�W�F�N�g���j�������܂ŕۑ������B����ɂ��A�t�H���g�����ɂ�����CPU�̕��ׂ�
�}���邱�Ƃ��o����B�������A�쐬���ꂽ�e�N�X�`���͂ǂ�ǂ�r�f�I��������ɗ��܂��Ă����A
�r�f�I�J�[�h��̃r�f�I���������s������ƃ��C�����������g�p���n�߁A�p�t�H�[�}���X�̒ቺ�Ɍq���鋰�ꂪ����B
�Ⴆ�΁A32Pixel*32Pixel�A32Bit�J���[�A5000��ނ̕������g�p�����ꍇ�ɂ̓r�f�I��������20MB�g�p����B
���Ǔ_�Ƃ��ẮA�����Ԏg�p���Ă��Ȃ��e�N�X�`���͎����I�ɔj������Ȃǂ�����B*/

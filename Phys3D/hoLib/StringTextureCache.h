//������e�N�X�`���̃L���b�V���N���X
#pragma once
#include <list>
#include <d3d9.h>
#include "PT.h"
#include "ComPtr.hpp"
#include "tstring.h"

namespace ho
{
	class DrawString;
	class StringTextureCache;

	class StringTextureCacheManager
	{
	public:
		StringTextureCacheManager(LPDIRECT3DDEVICE9 pD3DDevice, DrawString *pDrawStringObj); //�R���X�g���N�^
		~StringTextureCacheManager(); //�f�X�g���N�^

		StringTextureCache *GetpStringTextureCache(TCHAR *pStr, int Height, int LimitiWidth); //�C�ӂ̕�����̃e�N�X�`�����擾
		void Optimize(); //�œK��
		void Clear(); //�S�ẴL���b�V�����폜

		//�A�N�Z�b�T
		DrawString *GetpDrawStringObj() { return pDrawStringObj; }
		DWORD GetTime() { return Time; }
	private:
		LPDIRECT3DDEVICE9 pD3DDevice;
		DrawString *pDrawStringObj;
		std::list<StringTextureCache *> listpStringTextureCacheObj;
		DWORD Time; //Optimize() ���Ă΂ꂽ��
	};

	class StringTextureCache
	{
	public:
		StringTextureCache(TCHAR *pStr, int Height, int LimitWidth, LPDIRECT3DDEVICE9 pD3DDevice, StringTextureCacheManager *pStringTextureCacheManagerObj); //�R���X�g���N�^
		~StringTextureCache(); //�f�X�g���N�^

		BOOL Compare(TCHAR *pStr); //�����̕�����ƃe�N�X�`���̕����񂪓������ǂ������r����
		void Use(DWORD Time) { this->LastUseTime = Time; } //�g�p����i�Ō�Ɏg�p���ꂽ���Ԃ��X�V�����j

		//�A�N�Z�b�T
		ho::ComPtr<LPDIRECT3DTEXTURE9> GetcpTexture() { return cpTexture; }
		ho::PT<int> GetptTextureSize() { return ptTextureSize; }
		int GetCharNum() { return CharNum; }
		DWORD GetLastUseTime() { return LastUseTime; }
	private:
		StringTextureCacheManager *pStringTextureCacheManagerObj;
		ho::ComPtr<LPDIRECT3DTEXTURE9> cpTexture;
		tstring str;
		ho::PT<int> ptTextureSize; //������e�N�X�`���̃T�C�Y
		int CharNum; //������
		DWORD LastUseTime; //�Ō�Ɏg�p���ꂽ����

		void CreateCacheTexture(int Height, int LimitWidth, LPDIRECT3DDEVICE9 pD3DDevice, StringTextureCacheManager *pStringTextureCacheManagerObj, LOGFONT *pLF); //�L���b�V���ƂȂ�e�N�X�`�����쐬����
		void CalcPtStringSize(ho::PT<int> *pptSize, int *pCharNum, TCHAR *pStr, int Height, int LimitWidth, LOGFONT *pLF); //������̃T�C�Y���v�Z����
	};
}

/*DrawString �N���X�̃R���X�g���N�^�� StringTextureCacheManager ��1�����쐬����B
������`�掞�ɁAStringTextureCacheManager::GetpStringTextureCache() ���Ăяo���A
�L���b�V�����ꂽ������e�N�X�`���̃I�u�W�F�N�g���擾����B���̎��A�܂��L���b�V������Ă��Ȃ�
�ꍇ�͎����ŃL���b�V�����쐬�����B
�����āAStringTextureCache::GetptTextureSize() �ŕ�����e�N�X�`���̃T�C�Y���擾���Ē��_����ݒ肵����A
StiringTextureCache::GetcpTexture() �Ńe�N�X�`���𓾂ăf�o�C�X�ɐݒ肵�ĕ`�悷��B

���L���b�V���̍폜�ɂ���
�L���b�V�����ꂽ�e�N�X�`�����폜���邽�߂ɂ́AStringTextureCacheManager::Optimize() ��
����I�Ɏ��s���Ďg���Ă��Ȃ��Ǝv����e�N�X�`�����폜����K�v������B
���ɂ���������ŁA�e�N�X�`���̗e�ʂ��v�Z���Ĉ��ȏ�ɂȂ����疢�g�p�Ȃ��̂��폜�Ȃǂ̕��@������B
*/





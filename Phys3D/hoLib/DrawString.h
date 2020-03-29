//������`��N���X

#pragma once
#include "Direct3D.h"
#include "D3DVertex.h"

namespace ho {
	class GetCharTexture;
	class StringTextureCacheManager;

	class DrawString
	{
	public:
		DrawString(Direct3D *pDirect3DObj, LPDIRECT3DDEVICE9 pD3DDevice, LOGFONT *pLF, BOOL Cache); //�R���X�g���N�^
		~DrawString(); //�f�X�g���N�^

		void FrameProcess(); //1�t���[�����̏���
		void OnLostDevice(); //�f�o�C�X���X�g���̃��\�[�X�������

		void Draw(int x, int y, TCHAR *lpStr); //������`��i�J�����g�ݒ�ŕ`��j
		void Draw(int x, int y, TCHAR *lpStr, int Height); //������`��i�����T�C�Y�̍������w��j
		void Draw(int x, int y, TCHAR *lpStr, DWORD dwColor); //������`��i�����̐F���w��j
		void Draw(int x, int y, TCHAR *lpStr, int Height, DWORD dwColor); //������`��i�����T�C�Y�̍����ƐF���w��j

		int DrawWidth(int x, int y, TCHAR *pStr, int Width, int Height); //�����w�肵�ĕ�����`��

		//�A�N�Z�b�T
		Direct3D *GetpDirect3DObj() { return pDirect3DObj; }
		GetCharTexture *GetpGetCharTextureObj() { return pGetCharTextureObj; }
		StringTextureCacheManager *GetpStringTextureCacheManagerObj() { return pStringTextureCacheManagerObj; }
	private:
		Direct3D *pDirect3DObj;
		LPDIRECT3DDEVICE9 pD3DDevice;
		GetCharTexture *pGetCharTextureObj; //�����e�N�X�`���擾�N���X�̃|�C���^
		StringTextureCacheManager *pStringTextureCacheManagerObj; //������e�N�X�`���L���b�V���I�u�W�F�N�g�ւ̃|�C���^
		D3DVERTEX_2D vt[4]; //�`��p���_���
		WORD Index[6]; //�`��p���_�C���f�b�N�X
		DWORD dwCurrentColor; //�J�����g�`��F
		int CurrentHeight; //�J�����g����

		//�`��̓�������
		int DrawAlg(int x, int y, TCHAR *lpStr, DWORD dwColor, int Height, int LimitWidth);
	};
}

/*�g�p���@
�v���O��������1���� DrawString�I�u�W�F�N�g������Ă����A
�������`�悵�����ꏊ�� DrawString::Draw() ���\�b�h���Ăяo���B

�����̃e�N�X�`�����̊Ǘ��͎����ōs����B
�R���X�g���N�^�̑�O������ Cache �� TRUE �ɂ���ƁA������ɑ΂���L���b�V���������A
VRAM ������邪 DrawPrimitive �n���߂̔��s�񐔂�}���č������ł���B
*/

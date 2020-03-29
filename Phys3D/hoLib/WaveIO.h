//Windows�̕W��API���g���ĉ����̓��o�͂��s���N���X

#pragma once
#include <Windows.h>
#include <mmsystem.h>
#pragma comment (lib, "winmm.lib")

namespace ho
{
	class WaveIn //�������̓N���X
	{
	public:
		WaveIn(WAVEFORMATEX *pWFE, DWORD dwCallback, DWORD fdwOpen, DWORD dwBufferLength); //�R���X�g���N�^
		~WaveIn(); //�f�X�g���N�^
		void Start(); //�^���J�n
		void Stop(); //�^����~

		virtual void mm_wim_open(); //MM_WIN_OPEN���b�Z�[�W�i�f�o�C�X���J�����j���̏����i�I�[�o�[���C�h�p�j
		virtual void mm_wim_data(LPARAM lParam); //MM_WIM_DATA���b�Z�[�W�i�o�b�t�@����t�ɂȂ����j���̏����i�I�[�o�[���C�h�p�j

		//�A�N�Z�b�T
		WAVEFORMATEX *GetpWFE() { return &wfe; }
	private:
		HWAVEIN hWaveIn; //Wave���̓n���h��
		WAVEFORMATEX wfe;
		WAVEHDR whdr[2];
		SHORT *pBuffer[2]; //�g�`���i�[����o�b�t�@
	};
}


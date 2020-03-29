#include "WaveIO.h"
#include "Common.h"

namespace ho
{
	//�R���X�g���N�^
	WaveIn::WaveIn(WAVEFORMATEX *pWFE, DWORD dwCallback, DWORD fdwOpen, DWORD dwBufferLength)
	{
		wfe = *pWFE; //WAVEFORMATEX�\���̂̓��e���R�s�[
	
		MMRESULT mmr = waveInOpen(&hWaveIn, WAVE_MAPPER, pWFE, dwCallback, 0, fdwOpen); //WaveIn�f�o�C�X���J��

		for (int i = 0; i < 2; i++)
		{
			pBuffer[i] = (SHORT *)malloc(dwBufferLength); //�o�b�t�@���m��

			ZeroMemory(&whdr[i], sizeof(WAVEHDR)); //WAVEHDR�\���̂�������
			whdr[i].lpData = (LPSTR)pBuffer[i];			//�o�b�t�@�̃|�C���^
			whdr[i].dwBufferLength = dwBufferLength;	//�o�b�t�@�̑傫��
			whdr[i].dwLoops = 1;

			waveInPrepareHeader(hWaveIn, &whdr[i], sizeof(WAVEHDR)); //�f�o�C�X�̃o�b�t�@������
		}
	}

	//�f�X�g���N�^
	WaveIn::~WaveIn()
	{
		waveInStop(hWaveIn);

		for (int i = 0; i < 2; i++)
			waveInUnprepareHeader(hWaveIn, &whdr[i], sizeof(WAVEHDR)); //�o�b�t�@���

		waveInClose(hWaveIn);

		for (int i = 0; i < 2; i++)
			FREE(pBuffer[i]); //�o�b�t�@���
	}

	//�^���J�n
	void WaveIn::Start()
	{
		return;
	}

	//�^����~
	void WaveIn::Stop()
	{
		return;
	}

	//MM_WIN_OPEN���b�Z�[�W�i�f�o�C�X���J�����j���̏����i�I�[�o�[���C�h�p�j
	void WaveIn::mm_wim_open()
	{
		for (int i = 0; i < 2; i++)
			waveInAddBuffer(hWaveIn, &whdr[i], sizeof(WAVEHDR)); //�o�b�t�@��ǉ�
		waveInStart(hWaveIn); //�������͊J�n

		return;
	}

	//MM_WIM_DATA���b�Z�[�W�i�o�b�t�@����t�ɂȂ����j���̏����i�I�[�o�[���C�h�p�j
	void WaveIn::mm_wim_data(LPARAM lParam)
	{
		//(PWAVEHDR)lParam ��WAVEHDR�\���̂ւ̃|�C���^�Ƃ��Ď��R�Ɏg��
		waveInAddBuffer(hWaveIn, (PWAVEHDR)lParam, sizeof(WAVEHDR));

		return;
	}
}

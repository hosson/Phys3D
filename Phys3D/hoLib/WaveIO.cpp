#include "WaveIO.h"
#include "Common.h"

namespace ho
{
	//コンストラクタ
	WaveIn::WaveIn(WAVEFORMATEX *pWFE, DWORD dwCallback, DWORD fdwOpen, DWORD dwBufferLength)
	{
		wfe = *pWFE; //WAVEFORMATEX構造体の内容をコピー
	
		MMRESULT mmr = waveInOpen(&hWaveIn, WAVE_MAPPER, pWFE, dwCallback, 0, fdwOpen); //WaveInデバイスを開く

		for (int i = 0; i < 2; i++)
		{
			pBuffer[i] = (SHORT *)malloc(dwBufferLength); //バッファを確保

			ZeroMemory(&whdr[i], sizeof(WAVEHDR)); //WAVEHDR構造体を初期化
			whdr[i].lpData = (LPSTR)pBuffer[i];			//バッファのポインタ
			whdr[i].dwBufferLength = dwBufferLength;	//バッファの大きさ
			whdr[i].dwLoops = 1;

			waveInPrepareHeader(hWaveIn, &whdr[i], sizeof(WAVEHDR)); //デバイスのバッファを準備
		}
	}

	//デストラクタ
	WaveIn::~WaveIn()
	{
		waveInStop(hWaveIn);

		for (int i = 0; i < 2; i++)
			waveInUnprepareHeader(hWaveIn, &whdr[i], sizeof(WAVEHDR)); //バッファ解放

		waveInClose(hWaveIn);

		for (int i = 0; i < 2; i++)
			FREE(pBuffer[i]); //バッファ解放
	}

	//録音開始
	void WaveIn::Start()
	{
		return;
	}

	//録音停止
	void WaveIn::Stop()
	{
		return;
	}

	//MM_WIN_OPENメッセージ（デバイスが開いた）時の処理（オーバーライド用）
	void WaveIn::mm_wim_open()
	{
		for (int i = 0; i < 2; i++)
			waveInAddBuffer(hWaveIn, &whdr[i], sizeof(WAVEHDR)); //バッファを追加
		waveInStart(hWaveIn); //音声入力開始

		return;
	}

	//MM_WIM_DATAメッセージ（バッファが一杯になった）時の処理（オーバーライド用）
	void WaveIn::mm_wim_data(LPARAM lParam)
	{
		//(PWAVEHDR)lParam とWAVEHDR構造体へのポインタとして自由に使う
		waveInAddBuffer(hWaveIn, (PWAVEHDR)lParam, sizeof(WAVEHDR));

		return;
	}
}

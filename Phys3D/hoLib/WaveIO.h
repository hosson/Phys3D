//Windowsの標準APIを使って音声の入出力を行うクラス

#pragma once
#include <Windows.h>
#include <mmsystem.h>
#pragma comment (lib, "winmm.lib")

namespace ho
{
	class WaveIn //音声入力クラス
	{
	public:
		WaveIn(WAVEFORMATEX *pWFE, DWORD dwCallback, DWORD fdwOpen, DWORD dwBufferLength); //コンストラクタ
		~WaveIn(); //デストラクタ
		void Start(); //録音開始
		void Stop(); //録音停止

		virtual void mm_wim_open(); //MM_WIN_OPENメッセージ（デバイスが開いた）時の処理（オーバーライド用）
		virtual void mm_wim_data(LPARAM lParam); //MM_WIM_DATAメッセージ（バッファが一杯になった）時の処理（オーバーライド用）

		//アクセッサ
		WAVEFORMATEX *GetpWFE() { return &wfe; }
	private:
		HWAVEIN hWaveIn; //Wave入力ハンドル
		WAVEFORMATEX wfe;
		WAVEHDR whdr[2];
		SHORT *pBuffer[2]; //波形を格納するバッファ
	};
}


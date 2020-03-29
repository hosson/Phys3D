#include "OggVorbisPlayer.h"
#include <process.h>
#include "Common.h"
#include "Direct3D.h"
#include "Math.h"
#include <math.h>
#include "LibError.h"

namespace ho {
	//コンストラクタ
	OggVorbisPlayer::OggVorbisPlayer(LPDIRECTSOUND8 pDS8, DWORD BufferSizeSample)
	{
		this->pDS8 = pDS8;
		this->BufferSizeSample = BufferSizeSample;
		lpDSBuf = NULL;
		Playing = FALSE;
		this->Volume = 1.0;
		this->FadeOutVel = 0;
		this->FadeOutVolume = 1.0;
		this->Pause = false;

		InitializeCriticalSection(&csDSBuf);

		ThreadContinue = TRUE;
		hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, this, 0, NULL); //スレッド関数開始
	}

	//デストラクタ
	OggVorbisPlayer::~OggVorbisPlayer()
	{
		if (hThread) //波形生成スレッド関数が存在する場合
		{
			ThreadContinue = FALSE;
			WaitForSingleObject(hThread, INFINITE); //スレッドが終了するまで待機
		}

		Stop(); //再生停止
		/*
		EnterCriticalSection(&csDSBuf);
		if (lpDSBuf)
			lpDSBuf->Stop(); //再生停止
		RELEASE(lpDSBuf); //セカンダリバッファ解放
		LeaveCriticalSection(&csDSBuf);

		DeleteCriticalSection(&csDSBuf);

		//★再生していない状態で呼び出されるとエラーが出る（用改造）
		//ov_clear(&ovf);
		*/
	}

	//ファイル名を指定して再生開始
	bool OggVorbisPlayer::Play(char *pFileName, int LoopBeginSample, int LoopEndSample) 
	{
		this->LoopBeginSample = LoopBeginSample;
		this->LoopEndSample = LoopEndSample;

		if (Playing) //既に再生中の場合
			Stop(0); //即座に終了

		if (ov_fopen(pFileName, &ovf)) //ファイルを開き、エラーだった場合
		{
			tstring str;
			tPrint(str, TEXT("%s が開けませんでした。"), pFileName);
			ER(str.c_str(), __WFILE__, __LINE__, true);
			return false;
		}

		vorbis_info *vi = ov_info(&ovf, -1); //OggVorbisファイルの情報を取得
		if (!vi) //情報取得に失敗していた場合
		{
			tstring str;
			tPrint(str, TEXT("%s の情報取得に失敗しました。"), pFileName);
			ER(str.c_str(), __WFILE__, __LINE__, true);
			return false;
		}

		RELEASE(lpDSBuf);


		//WAVEFORMATEX構造体の設定
		WAVEFORMATEX wfe;
		ZeroMemory(&wfe, sizeof(WAVEFORMATEX));
		wfe.wFormatTag = WAVE_FORMAT_PCM;							//フォーマット
		wfe.nChannels = vi->channels;								//チャンネル数
		wfe.nSamplesPerSec = vi->rate;								//サンプリング周波数
		wfe.wBitsPerSample = 16;									//量子化ビット数
		wfe.nBlockAlign = wfe.nChannels * wfe.wBitsPerSample / 8;	//1サンプルあたりのバイト数
		wfe.nAvgBytesPerSec = wfe.nSamplesPerSec * wfe.nBlockAlign;	//1秒あたりのバイト数
		wfe.cbSize = 0;

		SamplesPerSec = wfe.nSamplesPerSec; //サンプリング周波数を取得しておく

		BytesPerSample = wfe.nBlockAlign; //1サンプルあたりのバイト数を取得しておく

		BufferSizeByte = DWORD(BufferSizeSample * BytesPerSample); //波形データサイズ（サンプル数 * 量子化バイト数 * チャンネル数）

		//DSBUFFERDESC構造体の設定
		DSBUFFERDESC DSBufferDesc;
		ZeroMemory(&DSBufferDesc, sizeof(DSBUFFERDESC));
		DSBufferDesc.dwSize = sizeof(DSBUFFERDESC);
		DSBufferDesc.dwFlags = DSBCAPS_LOCDEFER |
			DSBCAPS_CTRLPAN |					//パンコントロールを使う
			DSBCAPS_CTRLFREQUENCY |				//周波数コントロールを使う
			DSBCAPS_CTRLVOLUME;					//ボリュームコントロールを使う
		DSBufferDesc.dwBufferBytes = BufferSizeByte;
		DSBufferDesc.dwReserved = 0;
		DSBufferDesc.lpwfxFormat = &wfe;
		DSBufferDesc.guid3DAlgorithm = GUID_NULL;

		PlayingPoint = 0; //再生位置を初期化
		FadeOutVel = 0;
		FadeOutVolume = 1.0; //フェードアウトヴォリュームを初期化

		EnterCriticalSection(&csDSBuf);
		//セカンダリバッファの作成
		HRESULT hr;
		hr = pDS8->CreateSoundBuffer(&DSBufferDesc, &lpDSBuf, NULL);
		if (FAILED(hr))
		{
			tstring str;
			Direct3D::GetErrorStr(str, hr);
			MessageBox(NULL, str.c_str(), NULL, MB_OK);
			return FALSE;
		}

		lpDSBuf->Play(0, 0, DSBPLAY_LOOPING); //ループフラグを指定して再生

		this->Pause = false;

		LeaveCriticalSection(&csDSBuf);

		Playing = TRUE;

		return true;
	}

	//再生停止
	void OggVorbisPlayer::Stop(double FadeOutVel) 
	{
		EnterCriticalSection(&csDSBuf);

		if (!this->Playing) //再生中ではない場合
		{
			LeaveCriticalSection(&csDSBuf);
			return; //何もしない
		}

		/*引数はフェードアウト速度。 0.0 を最小、1.0を最大とした音量に対して、一秒間にどの程度減衰するか。*/

		if (FadeOutVel == 0.0) //フェードアウトなしの場合
		{
			//EnterCriticalSection(&csDSBuf);
			if (lpDSBuf)
				lpDSBuf->Stop(); //再生停止
			RELEASE(lpDSBuf); //セカンダリバッファ解放
			Playing = FALSE;
			//LeaveCriticalSection(&csDSBuf);

			ov_clear(&ovf);
		} else {
			this->FadeOutVel = FadeOutVel;
		}

		LeaveCriticalSection(&csDSBuf);

		return;
	}

	//一時停止やその解除
	void OggVorbisPlayer::PauseSwitch() 
	{
		EnterCriticalSection(&csDSBuf);

		this->Pause = this->Pause != true;

		LeaveCriticalSection(&csDSBuf);

		return;
	}

	//スレッド関数
	unsigned __stdcall OggVorbisPlayer::ThreadFunc(void *Arg) 
	{
		((OggVorbisPlayer *)Arg)->LocalThreadFunc(); //オブジェクト内のスレッド関数を呼び出す

		return 0;
	}

	//オブジェクト内のスレッド関数
	void OggVorbisPlayer::LocalThreadFunc() 
	{
		double Theta = 0; //位相
		DWORD dwCurrentPlayPosition; //現在の再生位置取得用

		while (ThreadContinue)
		{
			EnterCriticalSection(&csDSBuf);
			if (lpDSBuf && Playing) //セカンダリバッファが存在し、再生中の場合
			{
				lpDSBuf->GetCurrentPosition(&dwCurrentPlayPosition, NULL); //現在の再生位置を取得

				//今回と前回の再生位置の差を計算
				DWORD PlayPositionProgress; //再生位置の差（サンプル数）を計算
				if (PlayingPoint > dwCurrentPlayPosition)
					PlayPositionProgress = (BufferSizeByte - PlayingPoint + dwCurrentPlayPosition) / BytesPerSample;
				else
					PlayPositionProgress = (dwCurrentPlayPosition - PlayingPoint) / BytesPerSample;

				//フェードアウト用のヴォリュームを計算
				FadeOutVolume -= ((double)PlayPositionProgress / (double)SamplesPerSec) * FadeOutVel; //フェードアウトヴォリュームを設定
				ho::Clamp(FadeOutVolume, 0.0, 1.0);

				lpDSBuf->SetVolume(ho::GetValueFromRatio<LONG>(pow(Volume * FadeOutVolume, 0.1), DSBVOLUME_MIN, DSBVOLUME_MAX)); //ヴォリュームを設定

				if (PlayingPoint < BufferSizeByte / BytesPerSample && dwCurrentPlayPosition >= BufferSizeByte / BytesPerSample) //再生位置が前半から後半に移った場合
					LockAndWrite(0, BufferSizeByte / BytesPerSample); //バッファ前半をロックして波形書き込み
				else if (PlayingPoint >= BufferSizeByte / BytesPerSample && dwCurrentPlayPosition < BufferSizeByte / BytesPerSample) //再生位置が後半から前半に移った場合
					LockAndWrite(BufferSizeByte / BytesPerSample, BufferSizeByte - (BufferSizeByte / BytesPerSample)); //バッファ後半をロックして波形書き込み

				PlayingPoint = dwCurrentPlayPosition; //取得した再生位置を保存

				if (FadeOutVolume == 0.0) //フェードアウトが終了した場合
					Stop(0); //再生停止
			}
			LeaveCriticalSection(&csDSBuf);

			Sleep(1); //CPUを100%使い切らないための処置
		}

		return;
	}

	//バッファをロックして波形を書き込む
	void OggVorbisPlayer::LockAndWrite(DWORD Begin, DWORD Size)
	{
		LPVOID lpBufLock; //メモリロックして得られるポインタ
		DWORD Length; //バッファの長さ

		//メモリロックして音声データを書き込む
		HRESULT hr = lpDSBuf->Lock(Begin, Size, &lpBufLock, &Length, NULL, NULL, NULL);
		if (DS_OK == hr) //Beginを開始アドレスとしSize分だけロックする
		{
			//メモリへ波形データ作成
			if (this->Pause) //ポーズ中の場合
				GetBufferFromEmpty((SHORT *)lpBufLock, Length); //指定されたポインタ上に指定したサイズの無音の音声を読み込む
			else
				GetBufferFromOgg((SHORT *)lpBufLock, Length); //指定されたポインタ上に指定したサイズのOggVorbisの音声を読み込む

			lpDSBuf->Unlock(lpBufLock, Length, NULL, 0); //ロック解除
		} else {
			tstring str;
			Direct3D::GetErrorStr(str, hr);
			str = TEXT("DirectSound バッファのロックに失敗しました。") + str;
			ER(str.c_str(), __WFILE__, __LINE__, true);
		}

		return;
	}

	//指定されたポインタ上に指定したサイズのOggVorbisの音声を読み込む
	void OggVorbisPlayer::GetBufferFromOgg(SHORT *pBuffer, DWORD DataSize) 
	{
		long ReadSize; //関数で一度に読み込んだサイズ
		int bitstream = 0;
		__int64 RemainSize = DataSize; //バッファに必要な残りサイズ
		__int64 RemainSize2 = 0; //ループ終了位置をまたいだ後に必要な残りサイズ

		if (LoopEndSample != -1) //ループ終了位置が指定されている場合
		{
			if (RemainSize > (LoopEndSample - ovf.pcm_offset) * BytesPerSample) //今回の読み込みでループ終了位置をまたぐ場合
			{
				RemainSize2 = RemainSize - (LoopEndSample - ovf.pcm_offset) * BytesPerSample;
				RemainSize = (LoopEndSample - ovf.pcm_offset) * BytesPerSample;
			}
		}

		//バッファを満たせるまで続ける
		while (TRUE)
		{
			while (RemainSize> 0)
			{
				ReadSize = ov_read(&ovf, (char *)pBuffer, (int)RemainSize, 0, 2, 1, &bitstream); //RemainSize以内のサイズをデコードして読み込むが、何バイト読み込むかは不定。

				if (!ReadSize) //0バイトしか読めなかった（ファイルの終了に到達した）場合
				{
					if (LoopBeginSample == -1) //ループ開始位置が指定されていない場合
						ov_pcm_seek(&ovf, 0); //先頭に戻る
					else
						ov_pcm_seek(&ovf, LoopBeginSample); //ループ開始位置まで戻る
				}

				RemainSize -= ReadSize; //バッファに必要なサイズを読み込めた分減らす
				pBuffer = (SHORT *)((char *)pBuffer + ReadSize); //バッファの書き込み位置を読み込めた分増やす
			}

			if (RemainSize2) //ループ終了位置をまたいだ後に必要な残りサイズがある場合
			{
				RemainSize = RemainSize2;
				RemainSize2 = 0;

				if (LoopBeginSample == -1) //ループ開始位置が指定されていない場合
					ov_pcm_seek(&ovf, 0); //先頭に戻る
				else
					ov_pcm_seek(&ovf, LoopBeginSample); //ループ開始位置まで戻る
			} else {
				break;
			}
		}

		return;
	}

	//指定されたポインタ上に指定したサイズの空白の音声データを書き込む
	void OggVorbisPlayer::GetBufferFromEmpty(SHORT *pBuffer, DWORD DataSize) 
	{
		for (DWORD i = 0; i < DWORD(DataSize / sizeof(SHORT)); i++)
			pBuffer[i] = SHORT(0);

		return;
	}

}


//DirectSound8 で OggVorbisファイルを BGM的にストリーミング再生するクラス

/*
Release ビルドする場合は、プロジェクトのプロパティの C/C++ → コード生成 → ランタイムライブラリ の欄を
（マルチスレッド）/MT にする必要がある。これは、OggVorbis のライブラリもその条件でビルドしたため。
また、/MT にしておくと配布時も DLL が必要無いので、通常はこの /MT の設定で大丈夫。
*/

#pragma once
#include "DirectSound.h"
#include "vorbis/vorbisfile.h"

#pragma comment (lib, "libogg_static.lib" )
#pragma comment (lib, "libvorbis_static.lib" )
#pragma comment (lib, "libvorbisfile_static.lib" )

namespace ho {
	class OggVorbisPlayer
	{
	public:
		OggVorbisPlayer(LPDIRECTSOUND8 pDS8, DWORD BufferSizeSample); //コンストラクタ
		~OggVorbisPlayer(); //デストラクタ

		bool Play(char *pFileName, int LoopBeginSample = -1, int LoopEndSample = -1); //ファイル名を指定して再生開始
		void Stop(double FadeOutVel = 0.0); //再生停止
		void PauseSwitch(); //一時停止やその解除
		static unsigned __stdcall ThreadFunc(void *Arg); //スレッド関数

		//アクセッサ
		void SetVolume(double Volume) { this->Volume = Volume; }
	private:
		LPDIRECTSOUND8 pDS8;
		IDirectSoundBuffer *lpDSBuf; //セカンダリバッファ
		CRITICAL_SECTION csDSBuf; //DirectSoundバッファ用のクリティカルセクション
		OggVorbis_File ovf; //OggVorbisファイルのハンドル
		DWORD BufferSizeSample; //セカンダリバッファのサイズ（サンプル単位）
		DWORD BufferSizeByte; //セカンダリバッファのサイズ（バイト単位）
		DWORD BytesPerSample; //1サンプルあたりのバイト数
		DWORD SamplesPerSec; //サンプリング周波数
		DWORD PlayingPoint; //再生位置
		HANDLE hThread; //波形生成スレッドのハンドル
		BOOL Playing; //再生中かどうか
		double Volume; //ヴォリューム（0.0～1.0）
		double FadeOutVel; //フェードアウト速度
		double FadeOutVolume; //フェードアウト中のヴォリューム（1.0～0.0）
		int LoopBeginSample, LoopEndSample; //再生中のループ制御のサンプル数
		bool Pause; //一時停止中かどうか

		void LocalThreadFunc(); //波形生成スレッド関数のオブジェクト関数
		void LockAndWrite(DWORD dwBegin, DWORD dwSize); //バッファをロックして波形を書き込む
		void GetBufferFromOgg(SHORT *pBuffer, DWORD DataSize); //指定されたポインタ上に指定したサイズのOggVorbisの音声を読み込む
		void GetBufferFromEmpty(SHORT *pBuffer, DWORD DataSize); //指定されたポインタ上に指定したサイズの空白の音声データを書き込む

		BOOL ThreadContinue; //スレッド関数のループ制御
	};
}

/*プログラム内では一つのオブジェクトを作成しておき、BGMを再生するときには随時Playメソッドを
呼び出す。*/
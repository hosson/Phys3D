//DirectSoundをラップしたクラス
#pragma once
#include <dsound.h>
#include <vector>
#include "Math.h"

#pragma comment (lib, "dsound.lib")

namespace ho {

	class DSBuffer;

	class DirectSound
	{
	public:
		DirectSound(HWND hWnd); //コンストラクタ
		~DirectSound(); //デストラクタ

		//アクセッサ
		LPDIRECTSOUND8 GetpDS8() { return pDS8; }
	private:
		LPDIRECTSOUND8 pDS8;
	};

	//DirectSound用のバッファを管理するクラス
	class DSBufferManager
	{
	public:
		DSBufferManager(); //コンストラクタ
		~DSBufferManager(); //デストラクタ

		void RegistDSBuffer(DSBuffer *pDSBufferObj, int Index); //サウンドバッファを登録
		DSBuffer *GetpDSBuffer(int Index) { return vectorpDSBuffer.at(Index); } //サウンドバッファを取得
		double GetVolume() { return Volume; }
		void SetVolume(double d) { this->Volume = d; ho::Clamp(Volume, 0.0, 1.0); }
		void SetPause(bool Pause); //ポーズ開始・ポーズ解除
		void ResetPause(); //ポーズ状態の場合に音を鳴らさずにポーズ解除する
	private:
		std::vector<DSBuffer *> vectorpDSBuffer; //DirectSoundBufferへのポインタの配列
		double Volume; //全体の音量（0.0～1.0）
	};

	//DirectSoundBufferをラップしたクラス
	class DSBuffer
	{
	public:
		DSBuffer(TCHAR *pFilename, LPDIRECTSOUND8 pDS8, int Duplicates, const double &Volume = 1.0, DWORD TimeInterval = 0, bool UsePuase = true); //コンストラクタ（ファイルから開く）
		DSBuffer(void *p, LPDIRECTSOUND8 pDS8, int Duplicates, const double &Volume = 1.0, DWORD TimeInterval = 0, bool UsePuase = true); //コンストラクタ（メモリマップドファイルのポインタから開く）
		~DSBuffer(); //デストラクタ

		bool Play(const double &PlayVolume = 1.0, bool Loop = false); //再生
		void Stop(); //停止
		void SetSubVolume(double v); //時間軸音量を変更する
		void SetPause(bool Pause); //ポーズ開始・ポーズ解除
		void ResetPause(); //ポーズ状態の場合に音を鳴らさずに解除する

		//アクセッサ
		void SetpDSBufferManagerObj(DSBufferManager *p) { this->pDSBufferManagerObj = p; }
	private:
		bool Enable; //バッファが有効かどうか
		double Volume; //バッファ固有の音量（0.0～1.0）
		double SubVolume; //時間軸上で動的に変化する音量（0.0～1.0）
		DWORD LastPlayTime; //最後の再生開始された時間(timeGetTime()の値）
		DWORD TimeInterval; //連続再生時に必要な間隔（LastPlayTimeの値が参照される）
		DSBufferManager *pDSBufferManagerObj;
		LPDIRECTSOUNDBUFFER pDSBuffer;
		int Duplicates; //同時再生可能数
		std::vector<LPDIRECTSOUNDBUFFER> vectorpDuplicateBuffer; //同時再生用仮想バッファへのポインタ配列
		int NextPlayBufferIndex; //次に再生するバッファの配列のインデックス
		bool UsePause; //ポーズ（再生一時停止機能）を適用させるかどうか
		bool PauseState; //Duplicateではないバッファに対するポーズ状態
		std::vector<bool> vectorDuplicatesPauseState; //Duplicatesなバッファ用のポーズ状態
		bool PlayWithLoop; //再生時にループを伴っていたかどうか

		void Constract(TCHAR *pFilename, void *p, LPDIRECTSOUND8 pDS8, int Duplicates, const double &Volume, DWORD TimeInterval); //コンストラクタの内部関数
		bool ReadDataFromFile(TCHAR *pFilename, void *p, WAVEFORMATEX *pWFE, std::vector<BYTE> *pvectorWaveData); //ファイルのデータを読む
		void ReadBytes(HANDLE hFile, void **pp, void *pOut, DWORD Bytes); //ファイルハンドルまたはポインタからデータを読む
		void MoveFilePointer(HANDLE hFile, void **pp, int MoveBytes); //ファイルポインタを移動する
		bool JudgeTimeInterval(); //時間間隔を判定する
	};
}

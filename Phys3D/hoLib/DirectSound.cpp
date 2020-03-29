#include "DirectSound.h"
#include "Debug.h"
#include "hoLib.h"
#include "Direct3D.h"
#include "Common.h"
#include "Math.h"
#include "LibError.h"

namespace ho {

	//コンストラクタ
	DirectSound::DirectSound(HWND hWnd) 
	{
		pDS8 = NULL;
		HRESULT hr = DirectSoundCreate8(NULL, &pDS8, NULL); //ダイレクトサウンドデバイス作成

		if (FAILED(hr)) //デバイス作成に失敗していた場合
		{
			pDS8 = NULL;
			tstring str;
			Direct3D::GetErrorStr(str, hr);
			str = TEXT("DirectSound デバイスの作成に失敗しました。") + str;
			ER(str.c_str(), __WFILE__, __LINE__, true);
		} else {
			pDS8->SetCooperativeLevel(hWnd, DSSCL_PRIORITY); //デバイス優先度の設定
		}
	}

	//デストラクタ
	DirectSound::~DirectSound() 
	{
		RELEASE(pDS8);
	}









	//コンストラクタ
	DSBufferManager::DSBufferManager()
	{
		this->Volume = 1.0;
	}

	//デストラクタ
	DSBufferManager::~DSBufferManager() 
	{
		for (DWORD i = 0; i < vectorpDSBuffer.size(); i++)
			SDELETE(vectorpDSBuffer.at(i));
	}

	//サウンドバッファを登録
	void DSBufferManager::RegistDSBuffer(DSBuffer *pDSBufferObj, int Index) 
	{
		pDSBufferObj->SetpDSBufferManagerObj(this);

		if ((signed)vectorpDSBuffer.size() > Index) //登録したい要素数が配列内の場合
		{
			vectorpDSBuffer.at(Index) = pDSBufferObj;
		} else {
			vectorpDSBuffer.resize(Index + 1);
			vectorpDSBuffer.at(Index) = pDSBufferObj;
		}

		return;
	}

	//ポーズ開始・ポーズ解除
	void DSBufferManager::SetPause(bool Pause) 
	{
		for (std::vector<DSBuffer *>::iterator itr = this->vectorpDSBuffer.begin(); itr != this->vectorpDSBuffer.end(); itr++)
			(*itr)->SetPause(Pause);

		return;
	}

	//ポーズ状態の場合に音を鳴らさずにポーズ解除する
	void DSBufferManager::ResetPause() 
	{
		for (std::vector<DSBuffer *>::iterator itr = this->vectorpDSBuffer.begin(); itr != this->vectorpDSBuffer.end(); itr++)
			(*itr)->ResetPause();

		return;
	}








	//コンストラクタ（ファイルから開く）
	DSBuffer::DSBuffer(TCHAR *pFilename, LPDIRECTSOUND8 pDS8, int Duplicates, const double &Volume, DWORD TimeInterval, bool UsePause)
		: UsePause(UsePause)
	{
		Constract(pFilename, NULL, pDS8, Duplicates, Volume, TimeInterval);
	}

	//コンストラクタ（メモリマップドファイルのポインタから開く）
	DSBuffer::DSBuffer(void *p, LPDIRECTSOUND8 pDS8, int Duplicates, const double &Volume, DWORD TimeInterval, bool UsePause)
		: UsePause(UsePause)
	{
		Constract(NULL, p, pDS8, Duplicates, Volume, TimeInterval);
	}

	//コンストラクタの内部関数
	void DSBuffer::Constract(TCHAR *pFilename, void *p, LPDIRECTSOUND8 pDS8, int Duplicates, const double &Volume, DWORD TimeInterval)
	{
		this->pDSBufferManagerObj = NULL;
		this->Volume = Volume;
		this->Duplicates = Duplicates;
		this->pDSBuffer = NULL;
		this->SubVolume = 1.0;
		this->LastPlayTime = 0;
		this->TimeInterval = TimeInterval;

		LPVOID lpBufLock;
		DWORD dwLength;

		DSBUFFERDESC DSBufferDesc;
		ZeroMemory(&DSBufferDesc, sizeof(DSBUFFERDESC));

		WAVEFORMATEX wfe;
		ZeroMemory(&wfe, sizeof(WAVEFORMATEX));

		std::vector<BYTE> vectorWaveBuffer;

		Enable = false;

		try
		{
			if (!pDS8) //DirectSound デバイスが存在しない場合
				throw Exception(TEXT("DirectSound デバイスが存在しませんでした。"), __WFILE__, __LINE__, true);

			if (!ReadDataFromFile(pFilename, p, &wfe, &vectorWaveBuffer)) //WAVEファイルからデータを読み、読めなかった場合
			{
				tstring str; //■例外がスルーされて抜けるとき文字列もなくなってhしまうかも
				tPrint(str, TEXT("%s が開けませんでした。"), pFilename);
				throw Exception(str, __WFILE__, __LINE__, true);
			}

			//DSBUFFERDESC構造体の設定
			DSBufferDesc.dwSize = sizeof(DSBUFFERDESC);
			DSBufferDesc.dwFlags = DSBCAPS_LOCDEFER |
				DSBCAPS_CTRLPAN |					//パンコントロールを使う
				DSBCAPS_CTRLFREQUENCY |				//周波数コントロールを使う
				DSBCAPS_CTRLVOLUME;					//ボリュームコントロールを使う
			DSBufferDesc.dwBufferBytes = vectorWaveBuffer.size();
			DSBufferDesc.dwReserved = 0;
			DSBufferDesc.lpwfxFormat = &wfe;
			DSBufferDesc.guid3DAlgorithm = GUID_NULL;

			//セカンダリバッファの作成
			HRESULT hr;
			if (FAILED(hr = pDS8->CreateSoundBuffer(&DSBufferDesc, &pDSBuffer, NULL)))
			{
				tstring str;
				Direct3D::GetErrorStr(str, hr);
				str = TEXT("DirectSound のセカンダリバッファ作成に失敗しました。") + str;
				throw Exception(str, __WFILE__, __LINE__, true);
			}

			//メモリロックして音声データを書き込む
			if (FAILED(hr = pDSBuffer->Lock(0, 0, &lpBufLock, &dwLength, NULL, NULL, DSBLOCK_ENTIREBUFFER)))
			{
				tstring str;
				Direct3D::GetErrorStr(str, hr);
				str = TEXT("DirectSound のバッファロックに失敗しました。") + str;
				throw Exception(str, __WFILE__, __LINE__, true);
			}
			//メモリコピー
			memcpy(lpBufLock, &vectorWaveBuffer.at(0), vectorWaveBuffer.size());
			if (FAILED(hr = pDSBuffer->Unlock(lpBufLock, dwLength, NULL, 0))) //バッファロック解除
			{
				tstring str;
				Direct3D::GetErrorStr(str, hr);
				str = TEXT("DirectSound のバッファロック解除に失敗しました。") + str;
				throw Exception(str, __WFILE__, __LINE__, true);
			}

			//ポーズ状態
			this->PauseState = false;

			//同時再生用バッファ設定
			vectorpDuplicateBuffer.resize(Duplicates);
			for (int i = 0; i < Duplicates; i++)
			{
				hr = pDS8->DuplicateSoundBuffer(pDSBuffer, &vectorpDuplicateBuffer.at(i));
				if (FAILED(hr))
				{
					tstring str;
					Direct3D::GetErrorStr(str, hr);
					str = TEXT("DirectSound のセカンダリバッファ複製に失敗しました。") + str;
					throw Exception(str, __WFILE__, __LINE__, true);
				}

				this->vectorDuplicatesPauseState.push_back(false); //ポーズ状態
			}

			NextPlayBufferIndex = 0;
			Enable = true;
		}
		catch(const Exception &e)
		{
			ER(e.strMsg.c_str(), e.pFile, e.Line, e.Continue);
		}

		return;
	}


	//ファイルのデータを読む
	bool DSBuffer::ReadDataFromFile(TCHAR *pFilename, void *p, WAVEFORMATEX *pWFE, std::vector<BYTE> *pvectorWaveData) 
	{
		try
		{
			HANDLE hFile = NULL;
			if (pFilename) //ファイル名が渡されている場合
			{
				hFile = CreateFile(pFilename, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hFile == INVALID_HANDLE_VALUE)
				{
					tstring str;
					ho::tPrint(str, TEXT("%s が開けませんでした。"), pFilename);
					throw Exception(str, __WFILE__, __LINE__, true);
				}
				SetFilePointer(hFile, 0, 0, FILE_BEGIN); //ポインタをファイル先頭にセット
			}

			WORD str[5];
			//DWORD dwBytes;
			DWORD dwDataLen;
			DWORD dwWFESize;
			DWORD dwDataSize;

			//RIFFヘッダの確認
			ReadBytes(hFile, &p, str, 4);
			//ReadFile(hFile, str, 4, &dwBytes, NULL);
			if (*(LPDWORD)str != *(LPDWORD)"RIFF")
			{
				tstring str;
				ho::tPrint(str, TEXT("%s はWaveファイルではありません。"), pFilename);
				throw Exception(str, __WFILE__, __LINE__, true);
			}

			//これ以降のファイルサイズの取得
			ReadBytes(hFile, &p, &dwDataLen, 4);
			//ReadFile(hFile, &dwDataLen, 4, &dwBytes, NULL);

			//WAVEヘッダの確認
			ReadBytes(hFile, &p, str, 4);
			//ReadFile(hFile, str, 4, &dwBytes, NULL);
			if (*(LPDWORD)str != *(LPDWORD)"WAVE")
			{
				tstring str;
				ho::tPrint(str, TEXT("%s はWaveファイルではありません。"), pFilename);
				throw Exception(str, __WFILE__, __LINE__, true);
			}

			//fmtチャンクの確認
			ReadBytes(hFile, &p, str, 4);
			//ReadFile(hFile, str, 4, &dwBytes, NULL);
			if (*(LPDWORD)str != *(LPDWORD)"fmt ")
			{
				tstring str;
				ho::tPrint(str, TEXT("%s はWaveファイルではありません。"), pFilename);
				throw Exception(str, __WFILE__, __LINE__, true);
			}

			//fmtチャンクサイズの取得
			ReadBytes(hFile, &p, &dwWFESize, 4);
			//ReadFile(hFile, &dwWFESize, 4, &dwBytes, NULL);

			ReadBytes(hFile, &p, pWFE, dwWFESize);
			//ReadFile(hFile, pWFE, dwWFESize, &dwBytes, NULL); //WAVEFORMATEX構造体を読む



			//次のチャンクの判定
			ReadBytes(hFile, &p, str, 4);
			if (*(LPDWORD)str == *(LPDWORD)"fact") //factチャンクがあった場合
			{
				DWORD Size;
				ReadBytes(hFile, &p, &Size, 4); //factチャンクサイズを取得
				MoveFilePointer(hFile, &p, Size); //チャンクサイズ分を飛ばす

				ReadBytes(hFile, &p, str, 4);
				if (*(LPDWORD)str == *(LPDWORD)"data") //その後dataチャンクがあった場合
				{
					//波形データのバイト数の取得
					ReadBytes(hFile, &p, &dwDataSize, 4);

					pvectorWaveData->resize(dwDataSize);
					ReadBytes(hFile, &p, &pvectorWaveData->at(0), dwDataSize);
				} else {
					tstring str;
					ho::tPrint(str, TEXT("%s はWaveファイルではありません。"), pFilename);
					throw Exception(str, __WFILE__, __LINE__, true);
				}
			} else if (*(LPDWORD)str == *(LPDWORD)"data") //dataチャンクがあった場合
			{
				//波形データのバイト数の取得
				ReadBytes(hFile, &p, &dwDataSize, 4);

				pvectorWaveData->resize(dwDataSize);
				ReadBytes(hFile, &p, &pvectorWaveData->at(0), dwDataSize);
			} else {
				tstring str;
				ho::tPrint(str, TEXT("%s はWaveファイルではありません。"), pFilename);
				throw Exception(str, __WFILE__, __LINE__, true);
			}

			if (hFile) //ファイルから開いていた場合
				CloseHandle(hFile); //ファイルを閉じる
		}
		catch (Exception &e)
		{
			ER(e.strMsg.c_str(), e.pFile, e.Line, e.Continue);
			return false;
		}

		return true;
	}

	//ファイルハンドルまたはポインタからデータを読む
	void DSBuffer::ReadBytes(HANDLE hFile, void **pp, void *pOut, DWORD Bytes)
	{
		if (hFile) //ファイルハンドルが渡されていた場合
		{
			DWORD dmy;
			ReadFile(hFile, pOut, Bytes, &dmy, NULL);
		} else if (pp) //ポインタが渡されていた場合
		{
			memcpy(pOut, *pp, Bytes);
			*pp = (BYTE *)*pp + Bytes;
		}

		return;
	}

	//ファイルポインタを移動する
	void DSBuffer::MoveFilePointer(HANDLE hFile, void **pp, int MoveBytes)
	{
		if (hFile) //ファイルハンドルが渡されていた場合
		{
			SetFilePointer(hFile, MoveBytes, NULL, FILE_CURRENT);
		} else if (pp) //ポインタが渡されていた場合
		{
			*pp = (BYTE *)*pp + MoveBytes;
		}

		return;
	}
	
	//デストラクタ
	DSBuffer::~DSBuffer()
	{
		for (DWORD i = 0; i < vectorpDuplicateBuffer.size(); i++)
			RELEASE(vectorpDuplicateBuffer.at(i));

		RELEASE(pDSBuffer);
	}

	//再生
	bool DSBuffer::Play(const double &PlayVolume, bool Loop) 
	{
		if (!Enable) //デバイスが存在しない、ファイル読み込みに失敗したなどで、有効ではない場合
			return false;

		this->PlayWithLoop = false;

		if (Duplicates) //同時再生用バッファが存在する場合
		{
			if (Loop) //ループ再生する場合
			{
				ER(TEXT("Duplicate Buffer を持つ効果音がループ再生されようとしました。"), __WFILE__, __LINE__, true);
			} else {
				if (JudgeTimeInterval()) //時間間隔を判定し、再生可能な時間が経過していた場合
				{
					vectorpDuplicateBuffer.at(NextPlayBufferIndex)->SetVolume(GetValueFromRatio<LONG>(pow(pDSBufferManagerObj->GetVolume() * this->Volume * this->SubVolume * PlayVolume, 0.1), DSBVOLUME_MIN, DSBVOLUME_MAX)); //ヴォリュームを設定
					vectorpDuplicateBuffer.at(NextPlayBufferIndex)->Play(0, 0, 0);
					NextPlayBufferIndex = (NextPlayBufferIndex + 1) % Duplicates;
				}
			}
		} else {
			if (pDSBuffer)
			{
				if (JudgeTimeInterval()) //時間間隔を判定し、再生可能な時間が経過していた場合
				{
					pDSBuffer->SetVolume(GetValueFromRatio<LONG>(pow(pDSBufferManagerObj->GetVolume() * this->Volume * this->SubVolume * PlayVolume, 0.1), DSBVOLUME_MIN, DSBVOLUME_MAX)); //ヴォリュームを設定
					if (Loop) //ループ再生する場合
					{
						pDSBuffer->Play(0, 0, DSBPLAY_LOOPING); //ループフラグを指定
						this->PlayWithLoop = true;
					} else {
						pDSBuffer->Play(0, 0, 0);
					}
				}
			}
		}

		return true;
	}

	//時間間隔を判定する
	bool DSBuffer::JudgeTimeInterval()
	{
		DWORD NowTime = timeGetTime(); //現在時刻を取得

		if (LastPlayTime + TimeInterval <= NowTime) //再生可能な時間になっていた場合
		{
			LastPlayTime = NowTime;
			return true;
		}

		return false;
	}
	
	//停止
	void DSBuffer::Stop()
	{
		if (!Enable) //デバイスが存在しない、ファイル読み込みに失敗したなどで、有効ではない場合
			return;

		if (Duplicates) //同時再生用バッファが存在する場合
		{
			for (int i = 0; i < Duplicates; i++)
				vectorpDuplicateBuffer.at(i)->Stop(); //再生停止
		} else {
			if (pDSBuffer)
				pDSBuffer->Stop(); //再生停止
		}

		return;
	}

	//時間軸音量を変更する
	void DSBuffer::SetSubVolume(double v)
	{
		this->SubVolume = v;

		//DirectSoundBuffer に音量を設定する
		if (Duplicates) //同時再生用バッファが存在する場合
		{
			for (int i = 0; i < Duplicates; i++)
				vectorpDuplicateBuffer.at(i)->SetVolume(GetValueFromRatio<LONG>(pow(pDSBufferManagerObj->GetVolume() * this->Volume * this->SubVolume, 0.1), DSBVOLUME_MIN, DSBVOLUME_MAX)); //ヴォリュームを設定
		} else {
			if (pDSBuffer)
				pDSBuffer->SetVolume(GetValueFromRatio<LONG>(pow(pDSBufferManagerObj->GetVolume() * this->Volume * this->SubVolume, 0.1), DSBVOLUME_MIN, DSBVOLUME_MAX)); //ヴォリュームを設定
		}

		return;
	}

	//ポーズ開始・ポーズ解除
	void DSBuffer::SetPause(bool Pause) 
	{
		if (!this->UsePause) //ポーズ機能を使わない設定の場合
			return;

		try
		{
			HRESULT hr;

			if (Pause) //ポーズにする場合
			{
				DWORD Status; //再生状態受け取り用

				if (!this->Duplicates) //Duplicatesバッファを使わない場合
				{
					//バッファの再生状態を取得
					hr = this->pDSBuffer->GetStatus(&Status);
					if (FAILED(hr))
					{
						tstring str;
						Direct3D::GetErrorStr(str, hr);
						throw Exception(str, __WFILE__, __LINE__, true);
					}
				
					if (Status & DSBSTATUS_PLAYING) //バッファが再生中の場合
					{
						//再生一時停止
						hr = this->pDSBuffer->Stop(); //現在位置で再生停止
						if (FAILED(hr))
						{
							tstring str;
							Direct3D::GetErrorStr(str, hr);
							throw Exception(str, __WFILE__, __LINE__, true);
						}
						this->PauseState = true; //ポーズ中にする
					}
				} else {
					for (int i = 0; i < this->Duplicates; i++) //Duplicates分を走査
					{
						//バッファの再生状態を取得
						hr = this->vectorpDuplicateBuffer.at(i)->GetStatus(&Status);
						if (FAILED(hr))
						{
							tstring str;
							Direct3D::GetErrorStr(str, hr);
							throw Exception(str, __WFILE__, __LINE__, true);
						}
				
						if (Status & DSBSTATUS_PLAYING) //バッファが再生中の場合
						{
							//再生一時停止
							hr = this->vectorpDuplicateBuffer.at(i)->Stop(); //現在位置で再生停止
							if (FAILED(hr))
							{
								tstring str;
								Direct3D::GetErrorStr(str, hr);
								throw Exception(str, __WFILE__, __LINE__, true);
							}
							this->vectorDuplicatesPauseState.at(i) = true; //ポーズ中にする
						}
					}
				}
			} else { //ポーズを解除する場合
				if (!this->Duplicates) //Duplicatesバッファを使わない場合
				{
					if (this->PauseState) //一時停止中の場合
					{
						DWORD Flags = 0;
						if (this->PlayWithLoop) //ループを伴って再生されていた場合
						Flags = DSBPLAY_LOOPING;

						hr = this->pDSBuffer->Play(0, 0, Flags); //再生再開
						if (FAILED(hr))
						{
							tstring str;
							Direct3D::GetErrorStr(str, hr);
							throw Exception(str, __WFILE__, __LINE__, true);
						}

						this->PauseState = false;
					}
				} else {
					for (int i = 0; i < this->Duplicates; i++) //Duplicates分を走査
					{
						if (this->vectorDuplicatesPauseState.at(i)) //一時停止中の場合
						{
							hr = this->vectorpDuplicateBuffer.at(i)->Play(0, 0, 0); //再生再開
							if (FAILED(hr))
							{
								tstring str;
								Direct3D::GetErrorStr(str, hr);
								throw Exception(str, __WFILE__, __LINE__, true);
							}

							this->vectorDuplicatesPauseState.at(i) = false;
						}
					}
				}
			}
		}
		catch (Exception &e)
		{
			ER(e.strMsg.c_str(), e.pFile, e.Line, e.Continue);
		}

		return;
	}

	//ポーズ状態の場合に音を鳴らさずに解除する
	void DSBuffer::ResetPause() 
	{
		if (!this->UsePause) //ポーズ機能を使わない設定の場合
			return;
		try
		{
			HRESULT hr;

			if (!this->Duplicates) //Duplicatesバッファを使わない場合
			{
				if (this->PauseState) //一時停止中の場合
				{
					hr = this->pDSBuffer->SetCurrentPosition(0); //再生位置を最初に戻す
					if (FAILED(hr))
					{
						tstring str;
						Direct3D::GetErrorStr(str, hr);
						throw Exception(str, __WFILE__, __LINE__, true);
					}

					this->PauseState = false;
				}
			} else {
				for (int i = 0; i < this->Duplicates; i++) //Duplicates分を走査
				{
					if (this->vectorDuplicatesPauseState.at(i)) //一時停止中の場合
					{
						hr = this->vectorpDuplicateBuffer.at(i)->SetCurrentPosition(0); //再生位置を最初に戻す
						if (FAILED(hr))
						{
							tstring str;
							Direct3D::GetErrorStr(str, hr);
							throw Exception(str, __WFILE__, __LINE__, true);
						}

						this->vectorDuplicatesPauseState.at(i) = false;
					}
				}
			}
		}
		catch (Exception &e)
		{
			ER(e.strMsg.c_str(), e.pFile, e.Line, e.Continue);
		}

		return;
	}
}



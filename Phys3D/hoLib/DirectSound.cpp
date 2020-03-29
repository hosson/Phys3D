#include "DirectSound.h"
#include "Debug.h"
#include "hoLib.h"
#include "Direct3D.h"
#include "Common.h"
#include "Math.h"
#include "LibError.h"

namespace ho {

	//�R���X�g���N�^
	DirectSound::DirectSound(HWND hWnd) 
	{
		pDS8 = NULL;
		HRESULT hr = DirectSoundCreate8(NULL, &pDS8, NULL); //�_�C���N�g�T�E���h�f�o�C�X�쐬

		if (FAILED(hr)) //�f�o�C�X�쐬�Ɏ��s���Ă����ꍇ
		{
			pDS8 = NULL;
			tstring str;
			Direct3D::GetErrorStr(str, hr);
			str = TEXT("DirectSound �f�o�C�X�̍쐬�Ɏ��s���܂����B") + str;
			ER(str.c_str(), __WFILE__, __LINE__, true);
		} else {
			pDS8->SetCooperativeLevel(hWnd, DSSCL_PRIORITY); //�f�o�C�X�D��x�̐ݒ�
		}
	}

	//�f�X�g���N�^
	DirectSound::~DirectSound() 
	{
		RELEASE(pDS8);
	}









	//�R���X�g���N�^
	DSBufferManager::DSBufferManager()
	{
		this->Volume = 1.0;
	}

	//�f�X�g���N�^
	DSBufferManager::~DSBufferManager() 
	{
		for (DWORD i = 0; i < vectorpDSBuffer.size(); i++)
			SDELETE(vectorpDSBuffer.at(i));
	}

	//�T�E���h�o�b�t�@��o�^
	void DSBufferManager::RegistDSBuffer(DSBuffer *pDSBufferObj, int Index) 
	{
		pDSBufferObj->SetpDSBufferManagerObj(this);

		if ((signed)vectorpDSBuffer.size() > Index) //�o�^�������v�f�����z����̏ꍇ
		{
			vectorpDSBuffer.at(Index) = pDSBufferObj;
		} else {
			vectorpDSBuffer.resize(Index + 1);
			vectorpDSBuffer.at(Index) = pDSBufferObj;
		}

		return;
	}

	//�|�[�Y�J�n�E�|�[�Y����
	void DSBufferManager::SetPause(bool Pause) 
	{
		for (std::vector<DSBuffer *>::iterator itr = this->vectorpDSBuffer.begin(); itr != this->vectorpDSBuffer.end(); itr++)
			(*itr)->SetPause(Pause);

		return;
	}

	//�|�[�Y��Ԃ̏ꍇ�ɉ���炳���Ƀ|�[�Y��������
	void DSBufferManager::ResetPause() 
	{
		for (std::vector<DSBuffer *>::iterator itr = this->vectorpDSBuffer.begin(); itr != this->vectorpDSBuffer.end(); itr++)
			(*itr)->ResetPause();

		return;
	}








	//�R���X�g���N�^�i�t�@�C������J���j
	DSBuffer::DSBuffer(TCHAR *pFilename, LPDIRECTSOUND8 pDS8, int Duplicates, const double &Volume, DWORD TimeInterval, bool UsePause)
		: UsePause(UsePause)
	{
		Constract(pFilename, NULL, pDS8, Duplicates, Volume, TimeInterval);
	}

	//�R���X�g���N�^�i�������}�b�v�h�t�@�C���̃|�C���^����J���j
	DSBuffer::DSBuffer(void *p, LPDIRECTSOUND8 pDS8, int Duplicates, const double &Volume, DWORD TimeInterval, bool UsePause)
		: UsePause(UsePause)
	{
		Constract(NULL, p, pDS8, Duplicates, Volume, TimeInterval);
	}

	//�R���X�g���N�^�̓����֐�
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
			if (!pDS8) //DirectSound �f�o�C�X�����݂��Ȃ��ꍇ
				throw Exception(TEXT("DirectSound �f�o�C�X�����݂��܂���ł����B"), __WFILE__, __LINE__, true);

			if (!ReadDataFromFile(pFilename, p, &wfe, &vectorWaveBuffer)) //WAVE�t�@�C������f�[�^��ǂ݁A�ǂ߂Ȃ������ꍇ
			{
				tstring str; //����O���X���[����Ĕ�����Ƃ���������Ȃ��Ȃ���h���܂�����
				tPrint(str, TEXT("%s ���J���܂���ł����B"), pFilename);
				throw Exception(str, __WFILE__, __LINE__, true);
			}

			//DSBUFFERDESC�\���̂̐ݒ�
			DSBufferDesc.dwSize = sizeof(DSBUFFERDESC);
			DSBufferDesc.dwFlags = DSBCAPS_LOCDEFER |
				DSBCAPS_CTRLPAN |					//�p���R���g���[�����g��
				DSBCAPS_CTRLFREQUENCY |				//���g���R���g���[�����g��
				DSBCAPS_CTRLVOLUME;					//�{�����[���R���g���[�����g��
			DSBufferDesc.dwBufferBytes = vectorWaveBuffer.size();
			DSBufferDesc.dwReserved = 0;
			DSBufferDesc.lpwfxFormat = &wfe;
			DSBufferDesc.guid3DAlgorithm = GUID_NULL;

			//�Z�J���_���o�b�t�@�̍쐬
			HRESULT hr;
			if (FAILED(hr = pDS8->CreateSoundBuffer(&DSBufferDesc, &pDSBuffer, NULL)))
			{
				tstring str;
				Direct3D::GetErrorStr(str, hr);
				str = TEXT("DirectSound �̃Z�J���_���o�b�t�@�쐬�Ɏ��s���܂����B") + str;
				throw Exception(str, __WFILE__, __LINE__, true);
			}

			//���������b�N���ĉ����f�[�^����������
			if (FAILED(hr = pDSBuffer->Lock(0, 0, &lpBufLock, &dwLength, NULL, NULL, DSBLOCK_ENTIREBUFFER)))
			{
				tstring str;
				Direct3D::GetErrorStr(str, hr);
				str = TEXT("DirectSound �̃o�b�t�@���b�N�Ɏ��s���܂����B") + str;
				throw Exception(str, __WFILE__, __LINE__, true);
			}
			//�������R�s�[
			memcpy(lpBufLock, &vectorWaveBuffer.at(0), vectorWaveBuffer.size());
			if (FAILED(hr = pDSBuffer->Unlock(lpBufLock, dwLength, NULL, 0))) //�o�b�t�@���b�N����
			{
				tstring str;
				Direct3D::GetErrorStr(str, hr);
				str = TEXT("DirectSound �̃o�b�t�@���b�N�����Ɏ��s���܂����B") + str;
				throw Exception(str, __WFILE__, __LINE__, true);
			}

			//�|�[�Y���
			this->PauseState = false;

			//�����Đ��p�o�b�t�@�ݒ�
			vectorpDuplicateBuffer.resize(Duplicates);
			for (int i = 0; i < Duplicates; i++)
			{
				hr = pDS8->DuplicateSoundBuffer(pDSBuffer, &vectorpDuplicateBuffer.at(i));
				if (FAILED(hr))
				{
					tstring str;
					Direct3D::GetErrorStr(str, hr);
					str = TEXT("DirectSound �̃Z�J���_���o�b�t�@�����Ɏ��s���܂����B") + str;
					throw Exception(str, __WFILE__, __LINE__, true);
				}

				this->vectorDuplicatesPauseState.push_back(false); //�|�[�Y���
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


	//�t�@�C���̃f�[�^��ǂ�
	bool DSBuffer::ReadDataFromFile(TCHAR *pFilename, void *p, WAVEFORMATEX *pWFE, std::vector<BYTE> *pvectorWaveData) 
	{
		try
		{
			HANDLE hFile = NULL;
			if (pFilename) //�t�@�C�������n����Ă���ꍇ
			{
				hFile = CreateFile(pFilename, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hFile == INVALID_HANDLE_VALUE)
				{
					tstring str;
					ho::tPrint(str, TEXT("%s ���J���܂���ł����B"), pFilename);
					throw Exception(str, __WFILE__, __LINE__, true);
				}
				SetFilePointer(hFile, 0, 0, FILE_BEGIN); //�|�C���^���t�@�C���擪�ɃZ�b�g
			}

			WORD str[5];
			//DWORD dwBytes;
			DWORD dwDataLen;
			DWORD dwWFESize;
			DWORD dwDataSize;

			//RIFF�w�b�_�̊m�F
			ReadBytes(hFile, &p, str, 4);
			//ReadFile(hFile, str, 4, &dwBytes, NULL);
			if (*(LPDWORD)str != *(LPDWORD)"RIFF")
			{
				tstring str;
				ho::tPrint(str, TEXT("%s ��Wave�t�@�C���ł͂���܂���B"), pFilename);
				throw Exception(str, __WFILE__, __LINE__, true);
			}

			//����ȍ~�̃t�@�C���T�C�Y�̎擾
			ReadBytes(hFile, &p, &dwDataLen, 4);
			//ReadFile(hFile, &dwDataLen, 4, &dwBytes, NULL);

			//WAVE�w�b�_�̊m�F
			ReadBytes(hFile, &p, str, 4);
			//ReadFile(hFile, str, 4, &dwBytes, NULL);
			if (*(LPDWORD)str != *(LPDWORD)"WAVE")
			{
				tstring str;
				ho::tPrint(str, TEXT("%s ��Wave�t�@�C���ł͂���܂���B"), pFilename);
				throw Exception(str, __WFILE__, __LINE__, true);
			}

			//fmt�`�����N�̊m�F
			ReadBytes(hFile, &p, str, 4);
			//ReadFile(hFile, str, 4, &dwBytes, NULL);
			if (*(LPDWORD)str != *(LPDWORD)"fmt ")
			{
				tstring str;
				ho::tPrint(str, TEXT("%s ��Wave�t�@�C���ł͂���܂���B"), pFilename);
				throw Exception(str, __WFILE__, __LINE__, true);
			}

			//fmt�`�����N�T�C�Y�̎擾
			ReadBytes(hFile, &p, &dwWFESize, 4);
			//ReadFile(hFile, &dwWFESize, 4, &dwBytes, NULL);

			ReadBytes(hFile, &p, pWFE, dwWFESize);
			//ReadFile(hFile, pWFE, dwWFESize, &dwBytes, NULL); //WAVEFORMATEX�\���̂�ǂ�



			//���̃`�����N�̔���
			ReadBytes(hFile, &p, str, 4);
			if (*(LPDWORD)str == *(LPDWORD)"fact") //fact�`�����N���������ꍇ
			{
				DWORD Size;
				ReadBytes(hFile, &p, &Size, 4); //fact�`�����N�T�C�Y���擾
				MoveFilePointer(hFile, &p, Size); //�`�����N�T�C�Y�����΂�

				ReadBytes(hFile, &p, str, 4);
				if (*(LPDWORD)str == *(LPDWORD)"data") //���̌�data�`�����N���������ꍇ
				{
					//�g�`�f�[�^�̃o�C�g���̎擾
					ReadBytes(hFile, &p, &dwDataSize, 4);

					pvectorWaveData->resize(dwDataSize);
					ReadBytes(hFile, &p, &pvectorWaveData->at(0), dwDataSize);
				} else {
					tstring str;
					ho::tPrint(str, TEXT("%s ��Wave�t�@�C���ł͂���܂���B"), pFilename);
					throw Exception(str, __WFILE__, __LINE__, true);
				}
			} else if (*(LPDWORD)str == *(LPDWORD)"data") //data�`�����N���������ꍇ
			{
				//�g�`�f�[�^�̃o�C�g���̎擾
				ReadBytes(hFile, &p, &dwDataSize, 4);

				pvectorWaveData->resize(dwDataSize);
				ReadBytes(hFile, &p, &pvectorWaveData->at(0), dwDataSize);
			} else {
				tstring str;
				ho::tPrint(str, TEXT("%s ��Wave�t�@�C���ł͂���܂���B"), pFilename);
				throw Exception(str, __WFILE__, __LINE__, true);
			}

			if (hFile) //�t�@�C������J���Ă����ꍇ
				CloseHandle(hFile); //�t�@�C�������
		}
		catch (Exception &e)
		{
			ER(e.strMsg.c_str(), e.pFile, e.Line, e.Continue);
			return false;
		}

		return true;
	}

	//�t�@�C���n���h���܂��̓|�C���^����f�[�^��ǂ�
	void DSBuffer::ReadBytes(HANDLE hFile, void **pp, void *pOut, DWORD Bytes)
	{
		if (hFile) //�t�@�C���n���h�����n����Ă����ꍇ
		{
			DWORD dmy;
			ReadFile(hFile, pOut, Bytes, &dmy, NULL);
		} else if (pp) //�|�C���^���n����Ă����ꍇ
		{
			memcpy(pOut, *pp, Bytes);
			*pp = (BYTE *)*pp + Bytes;
		}

		return;
	}

	//�t�@�C���|�C���^���ړ�����
	void DSBuffer::MoveFilePointer(HANDLE hFile, void **pp, int MoveBytes)
	{
		if (hFile) //�t�@�C���n���h�����n����Ă����ꍇ
		{
			SetFilePointer(hFile, MoveBytes, NULL, FILE_CURRENT);
		} else if (pp) //�|�C���^���n����Ă����ꍇ
		{
			*pp = (BYTE *)*pp + MoveBytes;
		}

		return;
	}
	
	//�f�X�g���N�^
	DSBuffer::~DSBuffer()
	{
		for (DWORD i = 0; i < vectorpDuplicateBuffer.size(); i++)
			RELEASE(vectorpDuplicateBuffer.at(i));

		RELEASE(pDSBuffer);
	}

	//�Đ�
	bool DSBuffer::Play(const double &PlayVolume, bool Loop) 
	{
		if (!Enable) //�f�o�C�X�����݂��Ȃ��A�t�@�C���ǂݍ��݂Ɏ��s�����ȂǂŁA�L���ł͂Ȃ��ꍇ
			return false;

		this->PlayWithLoop = false;

		if (Duplicates) //�����Đ��p�o�b�t�@�����݂���ꍇ
		{
			if (Loop) //���[�v�Đ�����ꍇ
			{
				ER(TEXT("Duplicate Buffer �������ʉ������[�v�Đ�����悤�Ƃ��܂����B"), __WFILE__, __LINE__, true);
			} else {
				if (JudgeTimeInterval()) //���ԊԊu�𔻒肵�A�Đ��\�Ȏ��Ԃ��o�߂��Ă����ꍇ
				{
					vectorpDuplicateBuffer.at(NextPlayBufferIndex)->SetVolume(GetValueFromRatio<LONG>(pow(pDSBufferManagerObj->GetVolume() * this->Volume * this->SubVolume * PlayVolume, 0.1), DSBVOLUME_MIN, DSBVOLUME_MAX)); //���H�����[����ݒ�
					vectorpDuplicateBuffer.at(NextPlayBufferIndex)->Play(0, 0, 0);
					NextPlayBufferIndex = (NextPlayBufferIndex + 1) % Duplicates;
				}
			}
		} else {
			if (pDSBuffer)
			{
				if (JudgeTimeInterval()) //���ԊԊu�𔻒肵�A�Đ��\�Ȏ��Ԃ��o�߂��Ă����ꍇ
				{
					pDSBuffer->SetVolume(GetValueFromRatio<LONG>(pow(pDSBufferManagerObj->GetVolume() * this->Volume * this->SubVolume * PlayVolume, 0.1), DSBVOLUME_MIN, DSBVOLUME_MAX)); //���H�����[����ݒ�
					if (Loop) //���[�v�Đ�����ꍇ
					{
						pDSBuffer->Play(0, 0, DSBPLAY_LOOPING); //���[�v�t���O���w��
						this->PlayWithLoop = true;
					} else {
						pDSBuffer->Play(0, 0, 0);
					}
				}
			}
		}

		return true;
	}

	//���ԊԊu�𔻒肷��
	bool DSBuffer::JudgeTimeInterval()
	{
		DWORD NowTime = timeGetTime(); //���ݎ������擾

		if (LastPlayTime + TimeInterval <= NowTime) //�Đ��\�Ȏ��ԂɂȂ��Ă����ꍇ
		{
			LastPlayTime = NowTime;
			return true;
		}

		return false;
	}
	
	//��~
	void DSBuffer::Stop()
	{
		if (!Enable) //�f�o�C�X�����݂��Ȃ��A�t�@�C���ǂݍ��݂Ɏ��s�����ȂǂŁA�L���ł͂Ȃ��ꍇ
			return;

		if (Duplicates) //�����Đ��p�o�b�t�@�����݂���ꍇ
		{
			for (int i = 0; i < Duplicates; i++)
				vectorpDuplicateBuffer.at(i)->Stop(); //�Đ���~
		} else {
			if (pDSBuffer)
				pDSBuffer->Stop(); //�Đ���~
		}

		return;
	}

	//���Ԏ����ʂ�ύX����
	void DSBuffer::SetSubVolume(double v)
	{
		this->SubVolume = v;

		//DirectSoundBuffer �ɉ��ʂ�ݒ肷��
		if (Duplicates) //�����Đ��p�o�b�t�@�����݂���ꍇ
		{
			for (int i = 0; i < Duplicates; i++)
				vectorpDuplicateBuffer.at(i)->SetVolume(GetValueFromRatio<LONG>(pow(pDSBufferManagerObj->GetVolume() * this->Volume * this->SubVolume, 0.1), DSBVOLUME_MIN, DSBVOLUME_MAX)); //���H�����[����ݒ�
		} else {
			if (pDSBuffer)
				pDSBuffer->SetVolume(GetValueFromRatio<LONG>(pow(pDSBufferManagerObj->GetVolume() * this->Volume * this->SubVolume, 0.1), DSBVOLUME_MIN, DSBVOLUME_MAX)); //���H�����[����ݒ�
		}

		return;
	}

	//�|�[�Y�J�n�E�|�[�Y����
	void DSBuffer::SetPause(bool Pause) 
	{
		if (!this->UsePause) //�|�[�Y�@�\���g��Ȃ��ݒ�̏ꍇ
			return;

		try
		{
			HRESULT hr;

			if (Pause) //�|�[�Y�ɂ���ꍇ
			{
				DWORD Status; //�Đ���Ԏ󂯎��p

				if (!this->Duplicates) //Duplicates�o�b�t�@���g��Ȃ��ꍇ
				{
					//�o�b�t�@�̍Đ���Ԃ��擾
					hr = this->pDSBuffer->GetStatus(&Status);
					if (FAILED(hr))
					{
						tstring str;
						Direct3D::GetErrorStr(str, hr);
						throw Exception(str, __WFILE__, __LINE__, true);
					}
				
					if (Status & DSBSTATUS_PLAYING) //�o�b�t�@���Đ����̏ꍇ
					{
						//�Đ��ꎞ��~
						hr = this->pDSBuffer->Stop(); //���݈ʒu�ōĐ���~
						if (FAILED(hr))
						{
							tstring str;
							Direct3D::GetErrorStr(str, hr);
							throw Exception(str, __WFILE__, __LINE__, true);
						}
						this->PauseState = true; //�|�[�Y���ɂ���
					}
				} else {
					for (int i = 0; i < this->Duplicates; i++) //Duplicates���𑖍�
					{
						//�o�b�t�@�̍Đ���Ԃ��擾
						hr = this->vectorpDuplicateBuffer.at(i)->GetStatus(&Status);
						if (FAILED(hr))
						{
							tstring str;
							Direct3D::GetErrorStr(str, hr);
							throw Exception(str, __WFILE__, __LINE__, true);
						}
				
						if (Status & DSBSTATUS_PLAYING) //�o�b�t�@���Đ����̏ꍇ
						{
							//�Đ��ꎞ��~
							hr = this->vectorpDuplicateBuffer.at(i)->Stop(); //���݈ʒu�ōĐ���~
							if (FAILED(hr))
							{
								tstring str;
								Direct3D::GetErrorStr(str, hr);
								throw Exception(str, __WFILE__, __LINE__, true);
							}
							this->vectorDuplicatesPauseState.at(i) = true; //�|�[�Y���ɂ���
						}
					}
				}
			} else { //�|�[�Y����������ꍇ
				if (!this->Duplicates) //Duplicates�o�b�t�@���g��Ȃ��ꍇ
				{
					if (this->PauseState) //�ꎞ��~���̏ꍇ
					{
						DWORD Flags = 0;
						if (this->PlayWithLoop) //���[�v�𔺂��čĐ�����Ă����ꍇ
						Flags = DSBPLAY_LOOPING;

						hr = this->pDSBuffer->Play(0, 0, Flags); //�Đ��ĊJ
						if (FAILED(hr))
						{
							tstring str;
							Direct3D::GetErrorStr(str, hr);
							throw Exception(str, __WFILE__, __LINE__, true);
						}

						this->PauseState = false;
					}
				} else {
					for (int i = 0; i < this->Duplicates; i++) //Duplicates���𑖍�
					{
						if (this->vectorDuplicatesPauseState.at(i)) //�ꎞ��~���̏ꍇ
						{
							hr = this->vectorpDuplicateBuffer.at(i)->Play(0, 0, 0); //�Đ��ĊJ
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

	//�|�[�Y��Ԃ̏ꍇ�ɉ���炳���ɉ�������
	void DSBuffer::ResetPause() 
	{
		if (!this->UsePause) //�|�[�Y�@�\���g��Ȃ��ݒ�̏ꍇ
			return;
		try
		{
			HRESULT hr;

			if (!this->Duplicates) //Duplicates�o�b�t�@���g��Ȃ��ꍇ
			{
				if (this->PauseState) //�ꎞ��~���̏ꍇ
				{
					hr = this->pDSBuffer->SetCurrentPosition(0); //�Đ��ʒu���ŏ��ɖ߂�
					if (FAILED(hr))
					{
						tstring str;
						Direct3D::GetErrorStr(str, hr);
						throw Exception(str, __WFILE__, __LINE__, true);
					}

					this->PauseState = false;
				}
			} else {
				for (int i = 0; i < this->Duplicates; i++) //Duplicates���𑖍�
				{
					if (this->vectorDuplicatesPauseState.at(i)) //�ꎞ��~���̏ꍇ
					{
						hr = this->vectorpDuplicateBuffer.at(i)->SetCurrentPosition(0); //�Đ��ʒu���ŏ��ɖ߂�
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



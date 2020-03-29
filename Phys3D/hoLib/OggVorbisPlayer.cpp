#include "OggVorbisPlayer.h"
#include <process.h>
#include "Common.h"
#include "Direct3D.h"
#include "Math.h"
#include <math.h>
#include "LibError.h"

namespace ho {
	//�R���X�g���N�^
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
		hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, this, 0, NULL); //�X���b�h�֐��J�n
	}

	//�f�X�g���N�^
	OggVorbisPlayer::~OggVorbisPlayer()
	{
		if (hThread) //�g�`�����X���b�h�֐������݂���ꍇ
		{
			ThreadContinue = FALSE;
			WaitForSingleObject(hThread, INFINITE); //�X���b�h���I������܂őҋ@
		}

		Stop(); //�Đ���~
		/*
		EnterCriticalSection(&csDSBuf);
		if (lpDSBuf)
			lpDSBuf->Stop(); //�Đ���~
		RELEASE(lpDSBuf); //�Z�J���_���o�b�t�@���
		LeaveCriticalSection(&csDSBuf);

		DeleteCriticalSection(&csDSBuf);

		//���Đ����Ă��Ȃ���ԂŌĂяo�����ƃG���[���o��i�p�����j
		//ov_clear(&ovf);
		*/
	}

	//�t�@�C�������w�肵�čĐ��J�n
	bool OggVorbisPlayer::Play(char *pFileName, int LoopBeginSample, int LoopEndSample) 
	{
		this->LoopBeginSample = LoopBeginSample;
		this->LoopEndSample = LoopEndSample;

		if (Playing) //���ɍĐ����̏ꍇ
			Stop(0); //�����ɏI��

		if (ov_fopen(pFileName, &ovf)) //�t�@�C�����J���A�G���[�������ꍇ
		{
			tstring str;
			tPrint(str, TEXT("%s ���J���܂���ł����B"), pFileName);
			ER(str.c_str(), __WFILE__, __LINE__, true);
			return false;
		}

		vorbis_info *vi = ov_info(&ovf, -1); //OggVorbis�t�@�C���̏����擾
		if (!vi) //���擾�Ɏ��s���Ă����ꍇ
		{
			tstring str;
			tPrint(str, TEXT("%s �̏��擾�Ɏ��s���܂����B"), pFileName);
			ER(str.c_str(), __WFILE__, __LINE__, true);
			return false;
		}

		RELEASE(lpDSBuf);


		//WAVEFORMATEX�\���̂̐ݒ�
		WAVEFORMATEX wfe;
		ZeroMemory(&wfe, sizeof(WAVEFORMATEX));
		wfe.wFormatTag = WAVE_FORMAT_PCM;							//�t�H�[�}�b�g
		wfe.nChannels = vi->channels;								//�`�����l����
		wfe.nSamplesPerSec = vi->rate;								//�T���v�����O���g��
		wfe.wBitsPerSample = 16;									//�ʎq���r�b�g��
		wfe.nBlockAlign = wfe.nChannels * wfe.wBitsPerSample / 8;	//1�T���v��������̃o�C�g��
		wfe.nAvgBytesPerSec = wfe.nSamplesPerSec * wfe.nBlockAlign;	//1�b������̃o�C�g��
		wfe.cbSize = 0;

		SamplesPerSec = wfe.nSamplesPerSec; //�T���v�����O���g�����擾���Ă���

		BytesPerSample = wfe.nBlockAlign; //1�T���v��������̃o�C�g�����擾���Ă���

		BufferSizeByte = DWORD(BufferSizeSample * BytesPerSample); //�g�`�f�[�^�T�C�Y�i�T���v���� * �ʎq���o�C�g�� * �`�����l�����j

		//DSBUFFERDESC�\���̂̐ݒ�
		DSBUFFERDESC DSBufferDesc;
		ZeroMemory(&DSBufferDesc, sizeof(DSBUFFERDESC));
		DSBufferDesc.dwSize = sizeof(DSBUFFERDESC);
		DSBufferDesc.dwFlags = DSBCAPS_LOCDEFER |
			DSBCAPS_CTRLPAN |					//�p���R���g���[�����g��
			DSBCAPS_CTRLFREQUENCY |				//���g���R���g���[�����g��
			DSBCAPS_CTRLVOLUME;					//�{�����[���R���g���[�����g��
		DSBufferDesc.dwBufferBytes = BufferSizeByte;
		DSBufferDesc.dwReserved = 0;
		DSBufferDesc.lpwfxFormat = &wfe;
		DSBufferDesc.guid3DAlgorithm = GUID_NULL;

		PlayingPoint = 0; //�Đ��ʒu��������
		FadeOutVel = 0;
		FadeOutVolume = 1.0; //�t�F�[�h�A�E�g���H�����[����������

		EnterCriticalSection(&csDSBuf);
		//�Z�J���_���o�b�t�@�̍쐬
		HRESULT hr;
		hr = pDS8->CreateSoundBuffer(&DSBufferDesc, &lpDSBuf, NULL);
		if (FAILED(hr))
		{
			tstring str;
			Direct3D::GetErrorStr(str, hr);
			MessageBox(NULL, str.c_str(), NULL, MB_OK);
			return FALSE;
		}

		lpDSBuf->Play(0, 0, DSBPLAY_LOOPING); //���[�v�t���O���w�肵�čĐ�

		this->Pause = false;

		LeaveCriticalSection(&csDSBuf);

		Playing = TRUE;

		return true;
	}

	//�Đ���~
	void OggVorbisPlayer::Stop(double FadeOutVel) 
	{
		EnterCriticalSection(&csDSBuf);

		if (!this->Playing) //�Đ����ł͂Ȃ��ꍇ
		{
			LeaveCriticalSection(&csDSBuf);
			return; //�������Ȃ�
		}

		/*�����̓t�F�[�h�A�E�g���x�B 0.0 ���ŏ��A1.0���ő�Ƃ������ʂɑ΂��āA��b�Ԃɂǂ̒��x�������邩�B*/

		if (FadeOutVel == 0.0) //�t�F�[�h�A�E�g�Ȃ��̏ꍇ
		{
			//EnterCriticalSection(&csDSBuf);
			if (lpDSBuf)
				lpDSBuf->Stop(); //�Đ���~
			RELEASE(lpDSBuf); //�Z�J���_���o�b�t�@���
			Playing = FALSE;
			//LeaveCriticalSection(&csDSBuf);

			ov_clear(&ovf);
		} else {
			this->FadeOutVel = FadeOutVel;
		}

		LeaveCriticalSection(&csDSBuf);

		return;
	}

	//�ꎞ��~�₻�̉���
	void OggVorbisPlayer::PauseSwitch() 
	{
		EnterCriticalSection(&csDSBuf);

		this->Pause = this->Pause != true;

		LeaveCriticalSection(&csDSBuf);

		return;
	}

	//�X���b�h�֐�
	unsigned __stdcall OggVorbisPlayer::ThreadFunc(void *Arg) 
	{
		((OggVorbisPlayer *)Arg)->LocalThreadFunc(); //�I�u�W�F�N�g���̃X���b�h�֐����Ăяo��

		return 0;
	}

	//�I�u�W�F�N�g���̃X���b�h�֐�
	void OggVorbisPlayer::LocalThreadFunc() 
	{
		double Theta = 0; //�ʑ�
		DWORD dwCurrentPlayPosition; //���݂̍Đ��ʒu�擾�p

		while (ThreadContinue)
		{
			EnterCriticalSection(&csDSBuf);
			if (lpDSBuf && Playing) //�Z�J���_���o�b�t�@�����݂��A�Đ����̏ꍇ
			{
				lpDSBuf->GetCurrentPosition(&dwCurrentPlayPosition, NULL); //���݂̍Đ��ʒu���擾

				//����ƑO��̍Đ��ʒu�̍����v�Z
				DWORD PlayPositionProgress; //�Đ��ʒu�̍��i�T���v�����j���v�Z
				if (PlayingPoint > dwCurrentPlayPosition)
					PlayPositionProgress = (BufferSizeByte - PlayingPoint + dwCurrentPlayPosition) / BytesPerSample;
				else
					PlayPositionProgress = (dwCurrentPlayPosition - PlayingPoint) / BytesPerSample;

				//�t�F�[�h�A�E�g�p�̃��H�����[�����v�Z
				FadeOutVolume -= ((double)PlayPositionProgress / (double)SamplesPerSec) * FadeOutVel; //�t�F�[�h�A�E�g���H�����[����ݒ�
				ho::Clamp(FadeOutVolume, 0.0, 1.0);

				lpDSBuf->SetVolume(ho::GetValueFromRatio<LONG>(pow(Volume * FadeOutVolume, 0.1), DSBVOLUME_MIN, DSBVOLUME_MAX)); //���H�����[����ݒ�

				if (PlayingPoint < BufferSizeByte / BytesPerSample && dwCurrentPlayPosition >= BufferSizeByte / BytesPerSample) //�Đ��ʒu���O������㔼�Ɉڂ����ꍇ
					LockAndWrite(0, BufferSizeByte / BytesPerSample); //�o�b�t�@�O�������b�N���Ĕg�`��������
				else if (PlayingPoint >= BufferSizeByte / BytesPerSample && dwCurrentPlayPosition < BufferSizeByte / BytesPerSample) //�Đ��ʒu���㔼����O���Ɉڂ����ꍇ
					LockAndWrite(BufferSizeByte / BytesPerSample, BufferSizeByte - (BufferSizeByte / BytesPerSample)); //�o�b�t�@�㔼�����b�N���Ĕg�`��������

				PlayingPoint = dwCurrentPlayPosition; //�擾�����Đ��ʒu��ۑ�

				if (FadeOutVolume == 0.0) //�t�F�[�h�A�E�g���I�������ꍇ
					Stop(0); //�Đ���~
			}
			LeaveCriticalSection(&csDSBuf);

			Sleep(1); //CPU��100%�g���؂�Ȃ����߂̏��u
		}

		return;
	}

	//�o�b�t�@�����b�N���Ĕg�`����������
	void OggVorbisPlayer::LockAndWrite(DWORD Begin, DWORD Size)
	{
		LPVOID lpBufLock; //���������b�N���ē�����|�C���^
		DWORD Length; //�o�b�t�@�̒���

		//���������b�N���ĉ����f�[�^����������
		HRESULT hr = lpDSBuf->Lock(Begin, Size, &lpBufLock, &Length, NULL, NULL, NULL);
		if (DS_OK == hr) //Begin���J�n�A�h���X�Ƃ�Size���������b�N����
		{
			//�������֔g�`�f�[�^�쐬
			if (this->Pause) //�|�[�Y���̏ꍇ
				GetBufferFromEmpty((SHORT *)lpBufLock, Length); //�w�肳�ꂽ�|�C���^��Ɏw�肵���T�C�Y�̖����̉�����ǂݍ���
			else
				GetBufferFromOgg((SHORT *)lpBufLock, Length); //�w�肳�ꂽ�|�C���^��Ɏw�肵���T�C�Y��OggVorbis�̉�����ǂݍ���

			lpDSBuf->Unlock(lpBufLock, Length, NULL, 0); //���b�N����
		} else {
			tstring str;
			Direct3D::GetErrorStr(str, hr);
			str = TEXT("DirectSound �o�b�t�@�̃��b�N�Ɏ��s���܂����B") + str;
			ER(str.c_str(), __WFILE__, __LINE__, true);
		}

		return;
	}

	//�w�肳�ꂽ�|�C���^��Ɏw�肵���T�C�Y��OggVorbis�̉�����ǂݍ���
	void OggVorbisPlayer::GetBufferFromOgg(SHORT *pBuffer, DWORD DataSize) 
	{
		long ReadSize; //�֐��ň�x�ɓǂݍ��񂾃T�C�Y
		int bitstream = 0;
		__int64 RemainSize = DataSize; //�o�b�t�@�ɕK�v�Ȏc��T�C�Y
		__int64 RemainSize2 = 0; //���[�v�I���ʒu���܂�������ɕK�v�Ȏc��T�C�Y

		if (LoopEndSample != -1) //���[�v�I���ʒu���w�肳��Ă���ꍇ
		{
			if (RemainSize > (LoopEndSample - ovf.pcm_offset) * BytesPerSample) //����̓ǂݍ��݂Ń��[�v�I���ʒu���܂����ꍇ
			{
				RemainSize2 = RemainSize - (LoopEndSample - ovf.pcm_offset) * BytesPerSample;
				RemainSize = (LoopEndSample - ovf.pcm_offset) * BytesPerSample;
			}
		}

		//�o�b�t�@�𖞂�����܂ő�����
		while (TRUE)
		{
			while (RemainSize> 0)
			{
				ReadSize = ov_read(&ovf, (char *)pBuffer, (int)RemainSize, 0, 2, 1, &bitstream); //RemainSize�ȓ��̃T�C�Y���f�R�[�h���ēǂݍ��ނ��A���o�C�g�ǂݍ��ނ��͕s��B

				if (!ReadSize) //0�o�C�g�����ǂ߂Ȃ������i�t�@�C���̏I���ɓ��B�����j�ꍇ
				{
					if (LoopBeginSample == -1) //���[�v�J�n�ʒu���w�肳��Ă��Ȃ��ꍇ
						ov_pcm_seek(&ovf, 0); //�擪�ɖ߂�
					else
						ov_pcm_seek(&ovf, LoopBeginSample); //���[�v�J�n�ʒu�܂Ŗ߂�
				}

				RemainSize -= ReadSize; //�o�b�t�@�ɕK�v�ȃT�C�Y��ǂݍ��߂������炷
				pBuffer = (SHORT *)((char *)pBuffer + ReadSize); //�o�b�t�@�̏������݈ʒu��ǂݍ��߂������₷
			}

			if (RemainSize2) //���[�v�I���ʒu���܂�������ɕK�v�Ȏc��T�C�Y������ꍇ
			{
				RemainSize = RemainSize2;
				RemainSize2 = 0;

				if (LoopBeginSample == -1) //���[�v�J�n�ʒu���w�肳��Ă��Ȃ��ꍇ
					ov_pcm_seek(&ovf, 0); //�擪�ɖ߂�
				else
					ov_pcm_seek(&ovf, LoopBeginSample); //���[�v�J�n�ʒu�܂Ŗ߂�
			} else {
				break;
			}
		}

		return;
	}

	//�w�肳�ꂽ�|�C���^��Ɏw�肵���T�C�Y�̋󔒂̉����f�[�^����������
	void OggVorbisPlayer::GetBufferFromEmpty(SHORT *pBuffer, DWORD DataSize) 
	{
		for (DWORD i = 0; i < DWORD(DataSize / sizeof(SHORT)); i++)
			pBuffer[i] = SHORT(0);

		return;
	}

}


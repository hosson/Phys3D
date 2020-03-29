//DirectSound�����b�v�����N���X
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
		DirectSound(HWND hWnd); //�R���X�g���N�^
		~DirectSound(); //�f�X�g���N�^

		//�A�N�Z�b�T
		LPDIRECTSOUND8 GetpDS8() { return pDS8; }
	private:
		LPDIRECTSOUND8 pDS8;
	};

	//DirectSound�p�̃o�b�t�@���Ǘ�����N���X
	class DSBufferManager
	{
	public:
		DSBufferManager(); //�R���X�g���N�^
		~DSBufferManager(); //�f�X�g���N�^

		void RegistDSBuffer(DSBuffer *pDSBufferObj, int Index); //�T�E���h�o�b�t�@��o�^
		DSBuffer *GetpDSBuffer(int Index) { return vectorpDSBuffer.at(Index); } //�T�E���h�o�b�t�@���擾
		double GetVolume() { return Volume; }
		void SetVolume(double d) { this->Volume = d; ho::Clamp(Volume, 0.0, 1.0); }
		void SetPause(bool Pause); //�|�[�Y�J�n�E�|�[�Y����
		void ResetPause(); //�|�[�Y��Ԃ̏ꍇ�ɉ���炳���Ƀ|�[�Y��������
	private:
		std::vector<DSBuffer *> vectorpDSBuffer; //DirectSoundBuffer�ւ̃|�C���^�̔z��
		double Volume; //�S�̂̉��ʁi0.0�`1.0�j
	};

	//DirectSoundBuffer�����b�v�����N���X
	class DSBuffer
	{
	public:
		DSBuffer(TCHAR *pFilename, LPDIRECTSOUND8 pDS8, int Duplicates, const double &Volume = 1.0, DWORD TimeInterval = 0, bool UsePuase = true); //�R���X�g���N�^�i�t�@�C������J���j
		DSBuffer(void *p, LPDIRECTSOUND8 pDS8, int Duplicates, const double &Volume = 1.0, DWORD TimeInterval = 0, bool UsePuase = true); //�R���X�g���N�^�i�������}�b�v�h�t�@�C���̃|�C���^����J���j
		~DSBuffer(); //�f�X�g���N�^

		bool Play(const double &PlayVolume = 1.0, bool Loop = false); //�Đ�
		void Stop(); //��~
		void SetSubVolume(double v); //���Ԏ����ʂ�ύX����
		void SetPause(bool Pause); //�|�[�Y�J�n�E�|�[�Y����
		void ResetPause(); //�|�[�Y��Ԃ̏ꍇ�ɉ���炳���ɉ�������

		//�A�N�Z�b�T
		void SetpDSBufferManagerObj(DSBufferManager *p) { this->pDSBufferManagerObj = p; }
	private:
		bool Enable; //�o�b�t�@���L�����ǂ���
		double Volume; //�o�b�t�@�ŗL�̉��ʁi0.0�`1.0�j
		double SubVolume; //���Ԏ���œ��I�ɕω����鉹�ʁi0.0�`1.0�j
		DWORD LastPlayTime; //�Ō�̍Đ��J�n���ꂽ����(timeGetTime()�̒l�j
		DWORD TimeInterval; //�A���Đ����ɕK�v�ȊԊu�iLastPlayTime�̒l���Q�Ƃ����j
		DSBufferManager *pDSBufferManagerObj;
		LPDIRECTSOUNDBUFFER pDSBuffer;
		int Duplicates; //�����Đ��\��
		std::vector<LPDIRECTSOUNDBUFFER> vectorpDuplicateBuffer; //�����Đ��p���z�o�b�t�@�ւ̃|�C���^�z��
		int NextPlayBufferIndex; //���ɍĐ�����o�b�t�@�̔z��̃C���f�b�N�X
		bool UsePause; //�|�[�Y�i�Đ��ꎞ��~�@�\�j��K�p�����邩�ǂ���
		bool PauseState; //Duplicate�ł͂Ȃ��o�b�t�@�ɑ΂���|�[�Y���
		std::vector<bool> vectorDuplicatesPauseState; //Duplicates�ȃo�b�t�@�p�̃|�[�Y���
		bool PlayWithLoop; //�Đ����Ƀ��[�v�𔺂��Ă������ǂ���

		void Constract(TCHAR *pFilename, void *p, LPDIRECTSOUND8 pDS8, int Duplicates, const double &Volume, DWORD TimeInterval); //�R���X�g���N�^�̓����֐�
		bool ReadDataFromFile(TCHAR *pFilename, void *p, WAVEFORMATEX *pWFE, std::vector<BYTE> *pvectorWaveData); //�t�@�C���̃f�[�^��ǂ�
		void ReadBytes(HANDLE hFile, void **pp, void *pOut, DWORD Bytes); //�t�@�C���n���h���܂��̓|�C���^����f�[�^��ǂ�
		void MoveFilePointer(HANDLE hFile, void **pp, int MoveBytes); //�t�@�C���|�C���^���ړ�����
		bool JudgeTimeInterval(); //���ԊԊu�𔻒肷��
	};
}

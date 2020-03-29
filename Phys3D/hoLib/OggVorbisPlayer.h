//DirectSound8 �� OggVorbis�t�@�C���� BGM�I�ɃX�g���[�~���O�Đ�����N���X

/*
Release �r���h����ꍇ�́A�v���W�F�N�g�̃v���p�e�B�� C/C++ �� �R�[�h���� �� �����^�C�����C�u���� �̗���
�i�}���`�X���b�h�j/MT �ɂ���K�v������B����́AOggVorbis �̃��C�u���������̏����Ńr���h�������߁B
�܂��A/MT �ɂ��Ă����Ɣz�z���� DLL ���K�v�����̂ŁA�ʏ�͂��� /MT �̐ݒ�ő��v�B
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
		OggVorbisPlayer(LPDIRECTSOUND8 pDS8, DWORD BufferSizeSample); //�R���X�g���N�^
		~OggVorbisPlayer(); //�f�X�g���N�^

		bool Play(char *pFileName, int LoopBeginSample = -1, int LoopEndSample = -1); //�t�@�C�������w�肵�čĐ��J�n
		void Stop(double FadeOutVel = 0.0); //�Đ���~
		void PauseSwitch(); //�ꎞ��~�₻�̉���
		static unsigned __stdcall ThreadFunc(void *Arg); //�X���b�h�֐�

		//�A�N�Z�b�T
		void SetVolume(double Volume) { this->Volume = Volume; }
	private:
		LPDIRECTSOUND8 pDS8;
		IDirectSoundBuffer *lpDSBuf; //�Z�J���_���o�b�t�@
		CRITICAL_SECTION csDSBuf; //DirectSound�o�b�t�@�p�̃N���e�B�J���Z�N�V����
		OggVorbis_File ovf; //OggVorbis�t�@�C���̃n���h��
		DWORD BufferSizeSample; //�Z�J���_���o�b�t�@�̃T�C�Y�i�T���v���P�ʁj
		DWORD BufferSizeByte; //�Z�J���_���o�b�t�@�̃T�C�Y�i�o�C�g�P�ʁj
		DWORD BytesPerSample; //1�T���v��������̃o�C�g��
		DWORD SamplesPerSec; //�T���v�����O���g��
		DWORD PlayingPoint; //�Đ��ʒu
		HANDLE hThread; //�g�`�����X���b�h�̃n���h��
		BOOL Playing; //�Đ������ǂ���
		double Volume; //���H�����[���i0.0�`1.0�j
		double FadeOutVel; //�t�F�[�h�A�E�g���x
		double FadeOutVolume; //�t�F�[�h�A�E�g���̃��H�����[���i1.0�`0.0�j
		int LoopBeginSample, LoopEndSample; //�Đ����̃��[�v����̃T���v����
		bool Pause; //�ꎞ��~�����ǂ���

		void LocalThreadFunc(); //�g�`�����X���b�h�֐��̃I�u�W�F�N�g�֐�
		void LockAndWrite(DWORD dwBegin, DWORD dwSize); //�o�b�t�@�����b�N���Ĕg�`����������
		void GetBufferFromOgg(SHORT *pBuffer, DWORD DataSize); //�w�肳�ꂽ�|�C���^��Ɏw�肵���T�C�Y��OggVorbis�̉�����ǂݍ���
		void GetBufferFromEmpty(SHORT *pBuffer, DWORD DataSize); //�w�肳�ꂽ�|�C���^��Ɏw�肵���T�C�Y�̋󔒂̉����f�[�^����������

		BOOL ThreadContinue; //�X���b�h�֐��̃��[�v����
	};
}

/*�v���O�������ł͈�̃I�u�W�F�N�g���쐬���Ă����ABGM���Đ�����Ƃ��ɂ͐���Play���\�b�h��
�Ăяo���B*/
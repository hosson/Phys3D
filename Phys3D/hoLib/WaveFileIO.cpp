#include "WaveFileIO.h"

namespace ho {

BOOL WaveFileWrite::Write(TCHAR *pFilename, WAVEFORMATEX *pWFE, void *pData, DWORD DataSize)
{

	HANDLE hFile;
	DWORD dwBytes;

	hFile = CreateFile(pFilename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) //�t�@�C�����J���Ȃ������ꍇ
		MessageBox(NULL, TEXT("�o�͗p�t�@�C�����J���܂���ł����B"), TEXT("WaveFileWrite::WaveFileWrite"), MB_OK);

	SetFilePointer(hFile, 0, 0, FILE_BEGIN);

	DWORD dwDummy;

	WriteFile(hFile, "RIFF", 4, &dwBytes, NULL); //RIFF�w�b�_
	dwDummy = 12 + sizeof(WAVEFORMATEX) + 8 + DataSize;
	WriteFile(hFile, &dwDummy, 4, &dwBytes, NULL); //�t�@�C���T�C�Y-8Byte

	WriteFile(hFile, "WAVE", 4, &dwBytes, NULL); //WAVE�w�b�_
	
	WriteFile(hFile, "fmt ", 4, &dwBytes, NULL); //fmt�w�b�_
	dwDummy = sizeof(WAVEFORMATEX);
	WriteFile(hFile, &dwDummy, 4, &dwBytes, NULL); //fmt�w�b�_
	WriteFile(hFile, pWFE, sizeof(WAVEFORMATEX), &dwBytes, NULL); //fmt�w�b�_

	WriteFile(hFile, "data", 4, &dwBytes, NULL); //data�`�����N
	WriteFile(hFile, &DataSize, 4, &dwBytes, NULL); //�f�[�^�{�̂̃T�C�Y

	//16Bit�̉����̏������݂ɂ̂ݑΉ�
	WriteFile(hFile, pData, DataSize, &dwBytes, NULL);
	/*
	signed short Dummy16; //16Bit�ϊ��p
	for (DWORD i = 0; i < DataSize; i += pWFE->wBitsPerSample / 8)
	{
		Dummy16 = *(signed short *)pData; //16Bit�ɕϊ�
		WriteFile(hFile, &Dummy16, sizeof(signed short), &dwBytes, NULL); //��������
		pData = (signed short *)pData + 1;
	}*/

	/*
	//�f�[�^�{�̂�1�T���v������signed short(16Bit)�ɕϊ����Ȃ��珑������
	//unsigned char Dummy8; //8Bit�ϊ��p
	signed short Dummy16; //16Bit�ϊ��p
	//char Dummy24[3]; //24Bit�p
	//signed int Dummy32; //32Bit�p
	DWORD Samples = DataSize / pWFE->nBlockAlign; //�T���v�������v�Z
	for (DWORD i = 0; i < Samples; i++)
	{
		switch (pWFE->wBitsPerSample) //�o�͂���r�b�g�ɂ���ĕ���
		{
		case 8: //8Bit
			break;
		case 16: //16Bit
			Dummy16 = *(signed short *)pData; //16Bit�ɕϊ�
			WriteFile(hFile, &Dummy16, sizeof(signed short), &dwBytes, NULL); //L��������
			Dummy16 = *((signed short *)pData + 1); //16Bit�ɕϊ�
			WriteFile(hFile, &Dummy16, sizeof(signed short), &dwBytes, NULL); //R��������
			break;
		case 24: //24Bit
			break;
		}

		pData = (signed short *)pData + 2;
	}
	*/

	SetEndOfFile(hFile); //�t�@�C����EOF��ݒ�
	CloseHandle(hFile);

	return TRUE;
}

}

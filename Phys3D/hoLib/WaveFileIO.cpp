#include "WaveFileIO.h"

namespace ho {

BOOL WaveFileWrite::Write(TCHAR *pFilename, WAVEFORMATEX *pWFE, void *pData, DWORD DataSize)
{

	HANDLE hFile;
	DWORD dwBytes;

	hFile = CreateFile(pFilename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) //ファイルが開けなかった場合
		MessageBox(NULL, TEXT("出力用ファイルを開けませんでした。"), TEXT("WaveFileWrite::WaveFileWrite"), MB_OK);

	SetFilePointer(hFile, 0, 0, FILE_BEGIN);

	DWORD dwDummy;

	WriteFile(hFile, "RIFF", 4, &dwBytes, NULL); //RIFFヘッダ
	dwDummy = 12 + sizeof(WAVEFORMATEX) + 8 + DataSize;
	WriteFile(hFile, &dwDummy, 4, &dwBytes, NULL); //ファイルサイズ-8Byte

	WriteFile(hFile, "WAVE", 4, &dwBytes, NULL); //WAVEヘッダ
	
	WriteFile(hFile, "fmt ", 4, &dwBytes, NULL); //fmtヘッダ
	dwDummy = sizeof(WAVEFORMATEX);
	WriteFile(hFile, &dwDummy, 4, &dwBytes, NULL); //fmtヘッダ
	WriteFile(hFile, pWFE, sizeof(WAVEFORMATEX), &dwBytes, NULL); //fmtヘッダ

	WriteFile(hFile, "data", 4, &dwBytes, NULL); //dataチャンク
	WriteFile(hFile, &DataSize, 4, &dwBytes, NULL); //データ本体のサイズ

	//16Bitの音声の書き込みにのみ対応
	WriteFile(hFile, pData, DataSize, &dwBytes, NULL);
	/*
	signed short Dummy16; //16Bit変換用
	for (DWORD i = 0; i < DataSize; i += pWFE->wBitsPerSample / 8)
	{
		Dummy16 = *(signed short *)pData; //16Bitに変換
		WriteFile(hFile, &Dummy16, sizeof(signed short), &dwBytes, NULL); //書き込み
		pData = (signed short *)pData + 1;
	}*/

	/*
	//データ本体を1サンプルずつsigned short(16Bit)に変換しながら書き込み
	//unsigned char Dummy8; //8Bit変換用
	signed short Dummy16; //16Bit変換用
	//char Dummy24[3]; //24Bit用
	//signed int Dummy32; //32Bit用
	DWORD Samples = DataSize / pWFE->nBlockAlign; //サンプル数を計算
	for (DWORD i = 0; i < Samples; i++)
	{
		switch (pWFE->wBitsPerSample) //出力するビットによって分岐
		{
		case 8: //8Bit
			break;
		case 16: //16Bit
			Dummy16 = *(signed short *)pData; //16Bitに変換
			WriteFile(hFile, &Dummy16, sizeof(signed short), &dwBytes, NULL); //L書き込み
			Dummy16 = *((signed short *)pData + 1); //16Bitに変換
			WriteFile(hFile, &Dummy16, sizeof(signed short), &dwBytes, NULL); //R書き込み
			break;
		case 24: //24Bit
			break;
		}

		pData = (signed short *)pData + 2;
	}
	*/

	SetEndOfFile(hFile); //ファイルのEOFを設定
	CloseHandle(hFile);

	return TRUE;
}

}

#include "ConfigIO.h"
#include "Debug.h"
#include <tchar.h>
#include "LibError.h"

namespace ho
{
	//�R���X�g���N�^
	ConfigIO::ConfigIO(TCHAR *pFilename)
	{
		this->strFilename = pFilename;
		this->hFile = NULL;
	}

	//�f�X�g���N�^
	ConfigIO::~ConfigIO()
	{
	}

	//�w�肳�ꂽ�l��ǂށiint�^�j
	BOOL ConfigIO::Read(TCHAR *pName, int *pValue)
	{
		/*
		if (!FileOpen(GENERIC_READ, OPEN_EXISTING)) //�t�@�C����ǂݍ��݃��[�h�ŊJ���A�t�@�C�������݂��Ȃ��ꍇ�͎��s
			return FALSE;

		if (!FindName(pName)) //���O�������i���O��=�̉E����hFile�̃|�C���^�𑗂�j
		{
			FileClose(); //�t�@�C�������
			return FALSE;
		}

		tstring str;

		GetAt(NULL, 0, str); //���s�܂ł̕�������擾
		*/

		tstring str;
		if (ReadGetAt(pName, str))
		{
			*pValue = _ttoi(str.c_str()); //������𐮐��֕ϊ�

			FileClose(); //�t�@�C�������

			return TRUE;
		}

		return FALSE;
	}

	//�w�肳�ꂽ�l��ǂށidouble�^�j
	BOOL ConfigIO::Read(TCHAR *pName, double *pValue)
	{
		tstring str;
		if (ReadGetAt(pName, str))
		{
			*pValue = _ttof(str.c_str()); //������� double �^�֕ϊ�

			FileClose(); //�t�@�C�������

			return TRUE;
		}

		return FALSE;
	}
	
	//�w�肳�ꂽ�l��ǂށiTCHAR�^�j
	BOOL ConfigIO::Read(TCHAR *pName, tstring &Value)
	{
		if (ReadGetAt(pName, Value))
		{
			FileClose(); //�t�@�C�������
			return TRUE;
		}

		return FALSE;
	}

	//�t�@�C�����J���� pName �� = �̉E���̕�������擾
	BOOL ConfigIO::ReadGetAt(TCHAR *pName, tstring &str)
	{
		if (!FileOpen(GENERIC_READ, OPEN_EXISTING)) //�t�@�C����ǂݍ��݃��[�h�ŊJ���A�t�@�C�������݂��Ȃ��ꍇ�͎��s
			return FALSE;

		if (!FindName(pName)) //���O�������i���O��=�̉E����hFile�̃|�C���^�𑗂�j
		{
			FileClose(); //�t�@�C�������
			return FALSE;
		}

		return GetAt(NULL, 0, str); //���s�܂ł̕�������擾
	}

	//�w�肳�ꂽ�l���������ށiint�^�j
	BOOL ConfigIO::Write(TCHAR *pName, int Value)
	{
		if (!FileOpen(GENERIC_READ | GENERIC_WRITE, OPEN_ALWAYS)) //�t�@�C����ǂݏ������[�h�ŊJ���A�t�@�C�������݂��Ȃ��ꍇ�͐V�K�쐬
			return FALSE;

		tstring strLine;
	
		if (!FindName(pName)) //���O�������i���O��=�̉E����hFile�̃|�C���^�𑗂�j
		{ //���O��������Ȃ������ꍇ
			ho::tPrint(strLine, TEXT("%s=%d\r\n"), pName, Value);
			InsertStr((TCHAR *)strLine.c_str()); //���O=�l ��}��
		} else { //���O�����������ꍇ
			if (DeleteAtReturn()) //���݈ʒu���炻�̍s�̉��s��O�܂ł��폜�i���s�͎c��j
			{
				ho::tPrint(strLine, TEXT("%d"), Value);
				InsertStr((TCHAR *)strLine.c_str()); //�l��}��
			}
		}

		FileClose(); //�t�@�C�������
		
		return TRUE;
	}

	//�w�肳�ꂽ�l���������ށidouble�^�j
	BOOL ConfigIO::Write(TCHAR *pName, double Value)
	{
		if (!FileOpen(GENERIC_READ | GENERIC_WRITE, OPEN_ALWAYS)) //�t�@�C����ǂݏ������[�h�ŊJ���A�t�@�C�������݂��Ȃ��ꍇ�͐V�K�쐬
			return FALSE;

		tstring strLine;
	
		if (!FindName(pName)) //���O�������i���O��=�̉E����hFile�̃|�C���^�𑗂�j
		{ //���O��������Ȃ������ꍇ
			ho::tPrint(strLine, TEXT("%s=%f\r\n"), pName, Value);
			InsertStr((TCHAR *)strLine.c_str()); //���O=�l ��}��
		} else { //���O�����������ꍇ
			if (DeleteAtReturn()) //���݈ʒu���炻�̍s�̉��s��O�܂ł��폜�i���s�͎c��j
			{
				ho::tPrint(strLine, TEXT("%f"), Value);
				InsertStr((TCHAR *)strLine.c_str()); //�l��}��
			}
		}

		FileClose(); //�t�@�C�������
		return TRUE;
	}

	//�w�肳�ꂽ�l��ǂށiTCHAR�^�j
	BOOL ConfigIO::Write(TCHAR *pName, TCHAR *pValue)
	{
		//��������
		return TRUE;
	}

	//�t�@�C�����J��
	BOOL ConfigIO::FileOpen(DWORD dwDesiredAccess, DWORD dwCreationDisposition)
	{
		if (hFile) //�t�@�C���n���h�������ɑ��݂��Ă���ꍇ
		{
			ER(TEXT("�t�@�C���n���h�������ɑ��݂��Ă��܂��B"), __WFILE__, __LINE__, true);
			return FALSE;
		}

		hFile = CreateFile(strFilename.c_str(), dwDesiredAccess, 0, 0, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) //�t�@�C�����J���Ȃ������ꍇ
		{
			hFile = NULL;
			return FALSE;
		}

		SetFilePointer(hFile, NULL, NULL, FILE_BEGIN); //�t�@�C���|�C���^��擪�Ɏ����Ă���

		if (dwDesiredAccess & GENERIC_WRITE) //�������݂��܂ރ��[�h�̏ꍇ
		{
			WORD bom = 0xfeff; //UTF-16(���g���G���f�B�A���j��BOM Unicode�ł�����
			DWORD Bytes;
			WriteFile(hFile, &bom, sizeof(WORD), &Bytes, NULL); //�擪��Bom����������
		} else {
			SetFilePointer(hFile, 2, NULL, FILE_CURRENT); //Unicode �̍ŏ���16�r�b�g�̎��ʃR�[�h�iBOM�j���΂�
		}

		return TRUE;
	}

	//�t�@�C�������
	BOOL ConfigIO::FileClose()
	{
		if (!hFile) //�t�@�C���n���h�������݂��Ȃ��ꍇ
		{
			ER(TEXT("�t�@�C���n���h�������ɑ��݂��Ă��܂���B"), __WFILE__, __LINE__, true);
		}

		FlushFileBuffers(hFile); //�t�@�C���̃o�b�t�@���t���b�V��

		if (!CloseHandle(hFile)) //�t�@�C��������Ȃ������ꍇ
		{
			ER(TEXT("�t�@�C��������܂���ł����B"), __WFILE__, __LINE__, true);
			return FALSE;
		}

		hFile = NULL;

		return TRUE;
	}

	//���O�������i���O��=�̉E����hFile�̃|�C���^�𑗂�j
	BOOL ConfigIO::FindName(TCHAR *pName) 
	{
		TCHAR Hit; //������������
		tstring strRead; //�t�@�C������ǂ񂾕������i�[����

		while (TRUE) //EOF��������܂ŌJ��Ԃ�
		{
			Hit = GetAt(TEXT(" =["), 3, strRead); // =��[���X�y�[�X�܂ł̕�������擾
			if (!Hit) //EOF�����������ꍇ
				return FALSE;
	
			if (!_tcsicmp(pName, strRead.c_str())) //����������Ǝ擾����=�܂ł̕����񂪓������ꍇ
			{
				break;
			} else {
				SearchAtReturn(); //���̍s�փt�@�C���|�C���^���ڂ�
			}
		}

		if (Hit != TEXT('=')) //��������������=�ȊO�������ꍇ
		{
			SearchAt(TEXT('=')); //=��������܂Ńt�@�C����ǂ�
		}

		return TRUE;
	}

	//Key�̕����񂪌�����܂Ńt�@�C����ǂݑ�����
	BOOL ConfigIO::SearchAt(TCHAR Key) 
	{
		TCHAR ReadStr;
		DWORD dwBytes;

		do
		{
			if (ReadFile(hFile, &ReadStr, sizeof(TCHAR), &dwBytes, NULL)) //1�����擾
				if (!dwBytes) //EOF�ɒB���Ă����ꍇ
					return FALSE;
		} while (ReadStr != Key); //�ǂ񂾕����ƒT���Ă��镶�����Ⴄ�ꍇ�͑�����

		return TRUE;
	}

	//key�̕��������s�R�[�h��������܂ł̕������strOut�֊i�[����
	TCHAR ConfigIO::GetAt(TCHAR *pKeys, int Keys, tstring &strOut) 
	{
		strOut.clear(); //������
		strOut.resize(64); //����p�o�b�t�@�̏����T�C�Y
		DWORD dwBytes;
		TCHAR Hit; //������������
		TCHAR Buffer;

		DWORD i = 0;
		while (TRUE)
		{
			for (i = 0; i < strOut.size() - 1; i++)
			{
				if (ReadFile(hFile, &Buffer, sizeof(TCHAR), &dwBytes, NULL)) //1�����擾
				{
					if (!dwBytes) //EOF�ɒB���Ă����ꍇ
					{
						strOut.clear();
						return NULL;
					}
				}
				strOut.at(i) = Buffer;
				strOut.at(i + 1) = NULL;

				for (int j = 0; j < Keys; j++)
				{
					if (Buffer == pKeys[j]) //Key�ɒB�����ꍇ
					{
						strOut.at(i) = NULL;
						Hit = Buffer;
						goto LABEL1;
					}
				}

				if (i) //���ł�2�����ȏ�ǂݎ�����ꍇ
				{
					if (strOut.at(i - 1) == TEXT('\r') && strOut.at(i) == TEXT('\n')) //���s�R�[�h�������ꍇ
					{
						Hit = strOut.at(i);
						strOut.at(i) = NULL;
						goto LABEL1;
					}
				}

			}
			strOut.resize(strOut.size() * 2); //�o�b�t�@�T�C�Y��2�{�ɂ���
		}

	LABEL1:
		return Hit; //��������������Ԃ�
	}

	//���s��������܂Ńt�@�C����ǂݑ�����
	BOOL ConfigIO::SearchAtReturn() 
	{
		TCHAR ReadStr[3];
		DWORD dwBytes;
		int i = 0; //�ǂݎ��ꏊ

		while (TRUE)
		{
			if (ReadFile(hFile, &ReadStr[i], sizeof(TCHAR), &dwBytes, NULL)) //1�����擾
				if (!dwBytes) //EOF�ɒB���Ă����ꍇ
					return FALSE;

			if (i) //2�����ȏ�ǂ�ł����ꍇ
			{
				if (ReadStr[0] == TEXT('\r') && ReadStr[1] == TEXT('\n')) //���s�R�[�h�������ꍇ
					break;

				ReadStr[0] = ReadStr[1];
			} else {
				i = 1;
			}
		}

		return TRUE;
	}

	BOOL ConfigIO::InsertStr(TCHAR *pStr) //�t�@�C���̌��݈ʒu�ɕ������}������
	{
		TCHAR *pBuffer = new TCHAR[GetFileSize(hFile, NULL)]; //�t�@�C���T�C�Y���̃o�b�t�@���m��
		if (!pBuffer)
		{
			ER(TEXT("���������s�����Ă��܂��B"), __WFILE__, __LINE__, true);
			return FALSE;
		}

		DWORD dwFilePointer = SetFilePointer(hFile, 0, NULL, FILE_CURRENT); //�t�@�C���|�C���^�̌��݈ʒu���L�^

		BOOL Result;
		DWORD dwBytes;
		int i = 0; //�o�b�t�@���̌��݈ʒu�������J�E���g
		do
		{
			Result = ReadFile(hFile, &pBuffer[i], sizeof(TCHAR), &dwBytes, NULL); //1�����擾
			i++;
		} while (!Result || dwBytes); //EOF�ɒB����܂œǂݑ�����
		int BufferLen = i - 1; //�ǂݎ����������

		SetFilePointer(hFile, (LONG)dwFilePointer, NULL, FILE_BEGIN); //�t�@�C���|�C���^��}���n�_�ɖ߂�

		//�}�������̑}��
		for (i = 0; i < lstrlen(pStr); i++)
			WriteFile(hFile, &pStr[i], sizeof(TCHAR), &dwBytes, NULL);

		//�o�b�t�@�ɑޔ����Ă����㑱��������������
		for (i = 0; i < BufferLen; i++)
			WriteFile(hFile, &pBuffer[i], sizeof(TCHAR), &dwBytes, NULL);

		SetEndOfFile(hFile); //EOF��ݒ�

		SDELETE(pBuffer);

		return TRUE;
	}

	BOOL ConfigIO::DeleteAtReturn() //���݈ʒu���猻�݂̍s�̉��s��O�܂ł��폜�i���s�͎c��j
	{
		TCHAR *pBuffer = new TCHAR[GetFileSize(hFile, NULL)]; //�t�@�C���T�C�Y�iByte�j*TCHAR �̃o�b�t�@���m��
		if (!pBuffer)
		{
			ER(TEXT("���������s�����Ă��܂��B"), __WFILE__, __LINE__, true);
			return FALSE;
		}

		DWORD dwFilePointer = SetFilePointer(hFile, 0, NULL, FILE_CURRENT); //�t�@�C���|�C���^�̌��݈ʒu���L�^
		DWORD dwBytes;

		if (!SearchAtReturn()) //���s��������܂Ńt�@�C����ǂݑ�����
		{ //���s��������Ȃ������i�Ō�̍s�������j�ꍇ
			SetFilePointer(hFile, (LONG)dwFilePointer, NULL, FILE_BEGIN); //�t�@�C���|�C���^���L�^�n�_�ɖ߂�
			TCHAR str[3] = TEXT("\r\n");
			WriteFile(hFile, str, sizeof(TCHAR) * 2, &dwBytes, NULL); //���s����������
			SetEndOfFile(hFile); //EOF��ݒ�
			SetFilePointer(hFile, (LONG)dwFilePointer, NULL, FILE_BEGIN); //�t�@�C���|�C���^���L�^�n�_�ɖ߂�
			return TRUE;
		}

		BOOL Result;
		int i = 0; //�o�b�t�@���̌��݈ʒu�������J�E���g
		do
		{
			Result = ReadFile(hFile, &pBuffer[i], sizeof(TCHAR), &dwBytes, NULL); //1�����擾
			i++;
		} while (!Result || dwBytes); //EOF�ɒB����܂œǂݑ�����
		int BufferLen = i - 1; //�ǂݎ����������

		SetFilePointer(hFile, (LONG)dwFilePointer, NULL, FILE_BEGIN); //�t�@�C���|�C���^��}���n�_�ɖ߂�

		//���s����������
		TCHAR str[3] = TEXT("\r\n");
		WriteFile(hFile, str, sizeof(TCHAR) * 2, &dwBytes, NULL); //���s����������

		//�o�b�t�@�ɑޔ����Ă����㑱��������������
		for (i = 0; i < BufferLen; i++)
		{
			WriteFile(hFile, &pBuffer[i], sizeof(TCHAR), &dwBytes, NULL);
		}

		SetEndOfFile(hFile); //EOF��ݒ�
		SetFilePointer(hFile, (LONG)dwFilePointer, NULL, FILE_BEGIN); //�t�@�C���|�C���^��}���n�_�ɖ߂�

		SDELETE(pBuffer);

		return TRUE;
	}
}

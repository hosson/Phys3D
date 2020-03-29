//CSV�`���̃t�@�C����ǂރN���X
#include <string>
#include <tchar.h>
#include "tstring.h"
#include "CSVReader.h"

/*���\�[�X�̕����R�[�h�ɂ���
�ۑ��� VC++�̃G�f�B�^�� Unicode �R�[�h�y�[�W1200 �ŕۑ�����
*/

namespace ho {
	//�R���X�g���N�^
	//���\�[�X�t�@�C������ǂޏꍇ
	CSVReader::CSVReader(HMODULE hModule, TCHAR *lpResName, TCHAR *lpResType) 
	{
		this->hModule = hModule;
		strResName = lpResName;
		strResType = lpResType;

		Mode = Resource;
		Pos = 0;

		bool Error = FALSE; //�G���[������

		hGM = LoadResource(hModule, FindResource(hModule, strResName.c_str(), strResType.c_str()));
		if (!hGM)
			Error = TRUE;

		if (!Error)
			p = (LPTSTR)LockResource(hGM); //���\�[�X�f�[�^�̊i�[���ꂽ�������ւ̃|�C���^���擾
		if (p)
		{
			p++; //Unicode�擪��BOM�i16�r�b�g�j�����΂��ă|�C���^�𕶎���擪�֎����Ă���

			// //�Ŏn�܂�s�̏ꍇ�͎��̍s�ֈړ�
			while (*p == TEXT('/'))
			{
				FindStr(&p, TEXT('\n'));
				p++;
			}
		}
	}

	//�R���X�g���N�^
	//�t�@�C������ǂޏꍇ
	CSVReader::CSVReader(TCHAR *lpFilename)
	{
		this->hModule = NULL;
		strFilename = lpFilename;

		Mode = File;
		Pos = 0;
	}

	//��������̃|�C���^����ǂޏꍇ
	CSVReader::CSVReader(void *p)
	{
		this->p = (TCHAR *)p;

		if (this->p)
		{
			this->p++; //Unicode�擪��BOM�i16�r�b�g�j�����΂��ă|�C���^�𕶎���擪�֎����Ă���

			// //�Ŏn�܂�s�̏ꍇ�͎��̍s�ֈړ�
			while (*this->p == TEXT('/'))
			{
				FindStr(&this->p, TEXT('\n'));
				this->p++;
			}
		}

		Mode = Memory;
		Pos = 0;
	}


	//���݂̃f�[�^�ʒu�̃f�[�^��int�^�Ƃ��ēǂ�
	bool CSVReader::Read(int *pBuffer)
	{
		*pBuffer = _ttoi(p);
		NextPos(); //�|�C���^�����̃f�[�^�ʒu�ɐi�߂�

		return TRUE;
	}

	//���݂̃f�[�^�ʒu�̃f�[�^��DWORD�^�iunsigned long�^�j�Ƃ��ēǂ�
	bool CSVReader::Read(DWORD *pBuffer)
	{
		TCHAR *pEnd;
		*pBuffer = _tcstoul(p, &pEnd, 0);
		NextPos(); //�|�C���^�����̃f�[�^�ʒu�ɐi�߂�

		return TRUE;
	}

	//���݂̃f�[�^�ʒu�̃f�[�^��float�^�Ƃ��ēǂ�
	bool CSVReader::Read(float *pBuffer)
	{
		*pBuffer = (float)_ttof(p);
		NextPos(); //�|�C���^�����̃f�[�^�ʒu�ɐi�߂�

		return TRUE;
	}

	//���݂̃f�[�^�ʒu�̃f�[�^��double�^�Ƃ��ēǂ�
	bool CSVReader::Read(double *pBuffer) 
	{
		*pBuffer = _ttof(p);
		NextPos(); //�|�C���^�����̃f�[�^�ʒu�ɐi�߂�

		return TRUE;
	}

	//���݂̃f�[�^�ʒu�̃f�[�^��tstring�^�Ƃ��ēǂ�
	bool CSVReader::Read(tstring *pBuffer) 
	{
		tstring str;
		GetStr(&str); //���݂̃f�[�^�ʒu�̕�����𓾂�

		pBuffer->clear();

		int dq = 0; // " �̏o����
		for (tstring::iterator itr = str.begin(); itr != str.end(); itr++)
		{
			if (*itr == TEXT('"'))
			{
				dq++;
				if (dq == 1)
					continue;
				else if (dq == 2)
					break;
			}
			if (dq == 1)
				pBuffer->push_back(*itr);
		}

		NextPos(); //�|�C���^�����̃f�[�^�ʒu�ɐi�߂�

		return TRUE;
	}

	//���݂̃f�[�^�ʒu�̕�����𓾂�
	void CSVReader::GetStr(tstring *pStr) 
	{
		TCHAR *pToken = p; //��؂蕶���̈ʒu
		FindStr(&pToken, TEXT(','), true); //���̋�؂蕶���̈ʒu������

		pStr->clear();
		for (TCHAR *i = p; i <= pToken; i++)
			pStr->push_back(*i);

		return;
	}
	//���݂̃f�[�^�ʒu�̃f�[�^�^��Ԃ�
	CSVReader::eDataType CSVReader::GetDataType() 
	{
		tstring str;
		GetStr(&str); //���݂̃f�[�^�ʒu�̕�����𓾂�

		//�����񒆂̋󔒂ł͂Ȃ��Ȃ�ʒu�𓾂�
		int Begin = 0;
		for (tstring::iterator itr = str.begin(); itr != str.end(); itr++)
			if (*itr != TEXT(' ')) //�󔒂ł͂Ȃ��ꍇ
				break;
			else
				Begin++;

		if (str.at(Begin) == TEXT('"')) //"�Ŏn�܂�ꍇ
			return String; //������^

		//�������ǂ����̔���
		const TCHAR num[] = TEXT("0123456789.-"); //�����p������
		bool Number = FALSE; //�������ǂ���
		for (int i = 0; i < 12; i++)
			if (str.at(Begin) == num[i])
				Number = TRUE;
		if (!Number) //�����ł��Ȃ��ꍇ
			return null; //�s��

		//Int�AFloat�ADouble�̔���
		bool Period = FALSE, f = FALSE; //�����_��f�̗L��
		for (unsigned int i = 0; i < str.size(); i++)
		{
			if (str.at(i) == TEXT('.'))
				Period = TRUE;
			if (str.at(i) == TEXT('f'))
				f = TRUE;
		}
		if (Period == FALSE && f == FALSE) //�����_��f���Ȃ��ꍇ
			return Int; //����
		if (f)
			return Float; //�P���x���������_
		if (Period && !f)
			return Double; //�{���x���������_

		return null; //����ȊO�͕s��
	}

	//�f�[�^�ʒu�����ֈړ�
	void CSVReader::NextPos() 
	{
		TCHAR *pToken = p; //��؂蕶���̈ʒu
		FindStr(&pToken, TEXT(','), true); //���̋�؂蕶���̈ʒu������
		pToken++; //��؂蕶���̎��ֈړ�

		while (*pToken == TEXT('/')) // /�Ŏn�܂�ꍇ
		{
			FindStr(&pToken, TEXT('\n')); //���̍s��
			pToken++;
		}

		if (*pToken == TEXT('!')) //EOF�������ꍇ
			return; //�������Ȃ��ŏI��

		p = pToken;
		Pos++;

		return;
	}



	//���\�[�X��ǒ��Ɏ��̋�؂蕶����T��
	int CSVReader::FindStr(TCHAR **lpRes, TCHAR Token, bool SkipDoubleQuotes)
	{
		/* SkipDoubleQuotes �� true �̏ꍇ�́A " �� " �ň͂܂ꂽ�͈͓�����؂蕶�������Ώۂ���O���B*/

		TCHAR *lpToken = NULL; //��؂蕶���̈ʒu
		TCHAR *lpEnter = NULL; //���s�R�[�h�̈ʒu
		//TCHAR *lpToken = _tcschr(*lpRes, Token); //��؂蕶���̈ʒu
		//TCHAR *lpEnter = _tcschr(*lpRes, TEXT('\n')); //���s�R�[�h�̈ʒu

		TCHAR *p = *lpRes;
		bool DQ = false; //�_�u���N�H�[�e�[�V�����������ǂ���
		while (*p) //NULL�ɂȂ�܂ő����i���ӁF�t�@�C���I�[�� NULL ������ۏ؂͖����̂ŁANULL�����Ăɂ��Ă͂����Ȃ��j
		{
			if (SkipDoubleQuotes) //�_�u���N�H�[�e�[�V���������X�L�b�v����ꍇ
			{
				if (*p == TEXT('"')) //�_�u���N�H�[�e�[�V���������������ꍇ
				{
					DQ = (true - DQ) != 0; //�_�u���N�H�[�e�[�V������Ԃ𔽓]
				}
			}

			if (!DQ) //�_�u���N�H�[�e�[�V�����O�̏ꍇ
			{
				if (*p == Token && !lpToken)
					lpToken = p;
				if (*p == TEXT('\n') && !lpEnter)
					lpEnter = p;
			}

			if (lpToken || lpEnter) //��؂蕶���Ɖ��s�̂ǂ��炩�����������ꍇ
				break;

			p++;
		}


		if (lpToken) //��؂蕶�������������ꍇ
			*lpRes = lpToken;
		else //���s�R�[�h�����������ꍇ
			*lpRes = lpEnter;

		return 0; //���̖߂�l�͎g���Ă��Ȃ�

		/*

		if (!lpToken) //��؂蕶����������Ȃ������ꍇ
		{
			if (lpEnter) //���s�R�[�h�͌��������ꍇ
			{
				*lpRes = lpEnter;
				return 2; //���s�R�[�h����Ɍ��������ꍇ��2��Ԃ�
			}
			return 0;
		}

		if (lpEnter < lpToken) //�g�[�N�����������s�R�[�h�̕�����Ɍ��������ꍇ
		{
			*lpRes = lpEnter;
			return 2; //���s�R�[�h����Ɍ��������ꍇ��2��Ԃ�
		}

		*lpRes = lpToken;

		return 1; //��؂蕶���̎������݂�����1��Ԃ�
		*/
	}
}

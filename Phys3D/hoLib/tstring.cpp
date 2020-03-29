#include "tstring.h"
#include <stdarg.h>
#include "Common.h"

namespace ho
{
	//�����t�H�[�}�b�g����tstring�^��������쐬���A���̃|�C���^��Ԃ�
	void tPrint(tstring &str, const TCHAR *format, ...)
	{
		va_list args; //�ϒ��������X�g
		va_start(args, format); //�����擾�J�n

		int Length = _vsctprintf(format, args); //�����̕��������擾

		TCHAR *pTChar = new TCHAR[Length + 1]; //�����t�H�[�}�b�g�ɂ��o�͗p�̈�쐬
		_vstprintf_s(pTChar, Length + 1, format, args); //�����t�H�[�}�b�g�ɂ��o��

		str = pTChar;

		SDELETE(pTChar);

		return;
	}

	//�����t�H�[�}�b�g����tstring�^��������쐬���A���̎��̂�Ԃ�
	tstring tPrint(const TCHAR *format, ...)
	{
		va_list args; //�ϒ��������X�g
		va_start(args, format); //�����擾�J�n

		int Length = _vsctprintf(format, args); //�����̕��������擾

		TCHAR *pTChar = new TCHAR[Length + 1]; //�����t�H�[�}�b�g�ɂ��o�͗p�̈�쐬
		_vstprintf_s(pTChar, Length + 1, format, args); //�����t�H�[�}�b�g�ɂ��o��

		tstring str = pTChar;

		SDELETE(pTChar);

		return str;
	}

	//�}���`�o�C�g����(char)�����C�h����(wchar)�ɕϊ����� tstring �^�ɋl�߂ĕԂ�
	tstring CharToWChar(const char *pChar)
	{
		tstring strWChar; //return �ŕԋp���镶����

#if _UNICODE //Unicode�̏ꍇ
		setlocale(LC_ALL, "japanese"); //���P�[�����w��

		size_t BufferSize = strlen(pChar) + 1; //�o�b�t�@�T�C�Y
		TCHAR *pBuffer = new TCHAR[BufferSize]; //����p�̃o�b�t�@

		errno_t err = 0;
		size_t TransformedNumber = 0; //�ϊ����ꂽ������
		err = mbstowcs_s(&TransformedNumber, pBuffer, BufferSize, pChar, _TRUNCATE); //�ϊ�����

		strWChar = pBuffer; //�ϊ��������������
		delete [] pBuffer;
#else //�}���`�o�C�g�R�[�h�̏ꍇ
		strWChar = pChar; //�ϊ������ɂ��̂܂ܑ��
#endif
		return strWChar;
	}

	//���C�h������(WCHAR*)���}���`�o�C�g������(char*)�ɕϊ�
	std::string WCharToChar(const TCHAR *pTChar)
	{
		std::string strChar; //return�ŕԋp���镶����

#if _UNICODE //Unicode�̏ꍇ
		setlocale(LC_ALL, "japanese"); //���P�[�����w��

		size_t BufferSize = _tcslen(pTChar) + 1; //�o�b�t�@�T�C�Y
		char *pBuffer = new char[BufferSize]; //�󂯎��p�̃o�b�t�@

		errno_t err = 0;
		size_t TransformedNumber = 0; //�ϊ����ꂽ������
		err = wcstombs_s(&TransformedNumber, pBuffer, BufferSize, pTChar, _TRUNCATE); //�ϊ�����

		strChar = pBuffer; //�ϊ��������������
		delete [] pBuffer;
#else //�}���`�o�C�g�R�[�h�̏ꍇ
		strWChar = pChar; //�ϊ������ɂ��̂܂ܑ��
#endif
		return strChar;
	}

	//�C�ӂ̕����ŋ�؂�
	void Split(std::vector<tstring> &vectorStr, const tstring &str, const TCHAR &Key)
	{
		tstring spStr;

		for (tstring::const_iterator itr = str.begin(); itr != str.end(); itr++) //1����������
		{
			if (*itr == Key) //��؂蕶���ɓ��B�����ꍇ
			{
				vectorStr.push_back(spStr); //���������܂ł̕������z��ɒǉ�
				spStr.clear(); //�����������
			} else {
				spStr.push_back(*itr); //1�����ǉ�
			}
		}

		vectorStr.push_back(spStr); //�Ō�̕�������ǉ�

		return;
	}

	//�������16�i���Ƃ��ēǂ� int �^�ɕϊ�����
	__int64 atoi16(const tstring &str)
	{
		auto fTrans = [](const TCHAR &c)->__int64 //1�����𐔎��ɕϊ�����
		{
			switch (c)
			{
			case _T('0'):
				return 0;
				break;
			case _T('1'):
				return 1;
				break;
			case _T('2'):
				return 2;
				break;
			case _T('3'):
				return 3;
				break;
			case _T('4'):
				return 4;
				break;
			case _T('5'):
				return 5;
				break;
			case _T('6'):
				return 6;
				break;
			case _T('7'):
				return 7;
				break;
			case _T('8'):
				return 8;
				break;
			case _T('9'):
				return 9;
				break;
			case _T('a'):
			case _T('A'):
				return 10;
				break;
			case _T('b'):
			case _T('B'):
				return 11;
				break;
			case _T('c'):
			case _T('C'):
				return 12;
				break;
			case _T('d'):
			case _T('D'):
				return 13;
				break;
			case _T('e'):
			case _T('E'):
				return 14;
				break;
			case _T('f'):
			case _T('F'):
				return 15;
				break;
			}

			return -1; //0-9 a-f �ȊO�̕����̏ꍇ
		};

		__int64 Number = 0;
		int Digit = 0; //����
		tstring::const_iterator itr = str.end();
		do
		{
			itr--;
			int Trans = (int)fTrans(*itr); //1�����𐔎��ɕϊ�����
			if (Trans >= 0) //����ɕϊ��ł����ꍇ
			{
				Number += Trans * (__int64)pow(16.0, Digit); //�����𐔎��ɕϊ����Č��̐������|���ĉ��Z
				Digit++; //�����グ��
			}
		} while(itr != str.begin()); //�擪�̕����ł͂Ȃ��ꍇ�͑�����

		return Number;
	}

}

#include "MultiLingual.h"
#include "LibError.h"
#include <Windows.h>

namespace ho
{
	const TCHAR MultiLingual::strNull = NULL;

	//�R���X�g���N�^
	MultiLingual::MultiLingual()
	{
		vectorValidLocale.resize(_END, false);
		vectorStr.resize(_END);
	}

	//�R���X�g���N�^
	MultiLingual::MultiLingual(const TCHAR *pStr, ...)
	{
		vectorValidLocale.resize(_END, true);
		vectorStr.resize(_END);

		va_list list;
		va_start(list, pStr);

		vectorStr.at(0) = pStr;
		for (DWORD i = 1; i < _END; i++)
			vectorStr.at(i) = va_arg(list, TCHAR *);

		va_end(list);

	}

	//���郍�P�[���̕������ݒ�
	void MultiLingual::SetStr(const TCHAR *pStr, eLocale Locale) 
	{
		if (Locale < 0 || Locale >= _END) //�͈͊O�̏ꍇ
		{
			ER(TEXT("�͈͊O�̃��P�[�����w�肳��܂����B"), __WFILE__, __LINE__, true);
			return;
		}

		vectorStr.at(Locale) = pStr;
		vectorValidLocale.at(Locale) = true;

		return;
	}

	//�S�Ẵ��P�[���̕������ݒ�
	void MultiLingual::SetAllStr(const TCHAR *pStr, ...)
	{
		va_list list;
		va_start(list, pStr);

		vectorStr.at(0) = pStr;
		vectorValidLocale.at(0) = true;
		for (DWORD i = 1; i < _END; i++)
		{
			vectorStr.at(i) = va_arg(list, TCHAR);
			vectorValidLocale.at(i) = true;
		}

		va_end(list);

		return;
	}

	//���P�[�����w�肵�ĕ�����ւ̃|�C���^���擾
	const TCHAR *MultiLingual::c_str(eLocale Locale)
	{
		if (Locale < 0 || Locale >= _END) //�͈͊O�̏ꍇ
		{
			ER(TEXT("�͈͊O�̃��P�[�����w�肳��܂����B"), __WFILE__, __LINE__, true);
			return &strNull;
		}

		if (vectorValidLocale.at(Locale)) //���P�[���ɑΉ����������񂪐ݒ肳��Ă��Ȃ��ꍇ
			return &strNull;

		return vectorStr.at(Locale).c_str();
	}
}
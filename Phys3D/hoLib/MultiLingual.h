//������ɑΉ����������񃊃\�[�X�N���X
#pragma once
#include "tstring.h"
#include <vector>
#include <stdarg.h>

namespace ho
{
	class MultiLingual
	{
	public:
		enum eLocale { Japanese = 0, English, _END };
	public:
		MultiLingual(); //�R���X�g���N�^
		MultiLingual(const TCHAR *pStr, ...); //�R���X�g���N�^
		~MultiLingual() {} //�f�X�g���N�^

		void SetStr(const TCHAR *pStr, eLocale Locale); //���郍�P�[���̕������ݒ�
		void SetAllStr(const TCHAR *pStr, ...); //�S�Ẵ��P�[���̕������ݒ�
		const TCHAR *c_str(eLocale Locale); //���P�[�����w�肵�ĕ�����ւ̃|�C���^���擾

		const static TCHAR strNull;
	private:
		std::vector<bool> vectorValidLocale; //�����񂪗L���ȃ��P�[���������z��
		std::vector<tstring> vectorStr; //�e���P�[���̕�����z��
	};
}
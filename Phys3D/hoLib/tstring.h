//������֌W
#pragma once
#include <string>
#include <tchar.h>
#include <vector>

namespace ho
{
	typedef std::basic_string<TCHAR> tstring; //TCHAR�^�̉ϕ�����

	//�����t�H�[�}�b�g����tstring�^��������쐬���A���̃|�C���^��Ԃ�
	void tPrint(tstring &str, const TCHAR *format, ...);

	//�����t�H�[�}�b�g����tstring�^��������쐬���A���̎��̂�Ԃ�
	tstring tPrint(const TCHAR *format, ...);

	//�}���`�o�C�g����(char)�����C�h����(wchar)�ɕϊ����� tstring �^�ɋl�߂ĕԂ�
	tstring CharToWChar(const char *pChar);

	//���C�h������(wchar)���}���`�o�C�g������(char)�ɕϊ�
	std::string WCharToChar(const TCHAR *pTChar);

	//�C�ӂ̕����ŋ�؂�
	void Split(std::vector<tstring> &vectorStr, const tstring &str, const TCHAR &Key);

	//�������16�i���Ƃ��ēǂ� int �^�ɕϊ�����
	__int64 atoi16(const tstring &str);
}

/*
STL �� ������N���X�� TCHAR �^�ɂ������́B
�ϒ��̕����񂪈�����̂ŁA��{�I�ɕ�����͂�����g���B
*/
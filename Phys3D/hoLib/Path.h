//�p�X�Ɋւ���֐��Q

#pragma once
#include <Windows.h>
#include "hoLib.h"
#include "tstring.h"

namespace ho
{
	tstring GetDirectory(TCHAR *pIn); //���΃p�X����t�@�C�����̕����������������̂��擾����
	//void SetCurrentDirectoryFromExe(const tstring &strRelativePath); //�J�����g�f�B���N�g�����A���s�t�@�C�����猩�����΃p�X�Őݒ肷��
}
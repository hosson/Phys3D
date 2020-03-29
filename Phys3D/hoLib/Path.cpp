#include "Path.h"
#include "Debug.h"

namespace ho
{
	//���΃p�X����t�@�C�����̕����������������̂��擾����
	tstring GetDirectory(TCHAR *pIn) 
	{
		tstring strDir(pIn);

		int i2 = -1;
		for (int i = strDir.size() - 1; i >= 0; i--) //��������t���瑖��
		{
			if ((TCHAR)strDir.at(i) == TEXT('/')) //��؂蕶�������������ꍇ
			{
				i2 = i; //��؂蕶���̈ʒu���L��
				break;
			}
		}

		if (i2 != -1) //��؂蕶�����������Ă����ꍇ
		{
			strDir.resize(i2 + 1); //��؂蕶���܂ł͎c���āA���̌���폜
			//strDir.resize(i2); //��؂蕶���Ƃ��̌���폜
		}

		return strDir;
	}

	/*
	//�J�����g�f�B���N�g�����A���s�t�@�C�����猩�����΃p�X�Őݒ肷��
	void SetCurrentDirectoryFromExe(const tstring &strRelativePath) 
	{
		//�����ɋ�̕�������w�肷��ƁA�J�����g�f�B���N�g�������s�t�@�C���̏ꏊ�ɖ߂����B

		try
		{
			TCHAR FullPath[MAX_PATH + 1]; //�p�X�����i�[���镶����
			if (!GetModuleFileName(NULL, FullPath, MAX_PATH)) //���s�t�@�C���̃t���p�X�����擾���A���s�����ꍇ
			{
				tstring str;
				LibError::GetLastErrorStr(str); //�G���[������擾
				throw ho::Exception(str.c_str(), __WFILE__, __LINE__, false);
			}

			//�p�X����h���C�u���ƃf�B���N�g�����݂̂𒊏o
			TCHAR Drive[_MAX_DRIVE + 1], Dir[_MAX_DIR + 1];
			//_tsplitpath(FullPath, Drive, Dir, NULL, NULL); //�����̍s�Ōx�����o��̂ł��̕����͌��ݎg���Ă��Ȃ�

			//�h���C�u�ƃf�B���N�g���݂̂�����
			TCHAR CurrentPath[_MAX_DRIVE + _MAX_DIR + 1]; //���s�t�@�C�����u����Ă���f�B���N�g���܂ł̐�΃p�X
			errno_t error = _tmakepath_s(CurrentPath, sizeof(TCHAR) * (_MAX_DRIVE + _MAX_DIR + 1), Drive, Dir, NULL, NULL);
			if (error) //�G���[�����������ꍇ
			{
				tstring str;
				tPrint(str, TEXT("�h���C�u���ƃf�B���N�g�����̍������ɃG���[���������܂����B�G���[�R�[�h = %d"), error);
				throw ho::Exception(str.c_str(), __WFILE__, __LINE__, false);
			}

			tstring strSetPath = CurrentPath + strRelativePath; //���s�t�@�C�����u����Ă���f�B���N�g���܂ł̐�΃p�X�ƈ����̑��΃p�X����������
			if (!SetCurrentDirectory(strSetPath.c_str())) //�J�����g�f�B���N�g����ݒ肵�A�G���[���o���ꍇ
			{
				tstring str;
				LibError::GetLastErrorStr(str);
				throw ho::Exception(str.c_str(), __WFILE__, __LINE__, false);
			}
		}
		catch (ho::Exception &e)
		{
			ER(e.strMsg.c_str(), e.pFile, e.Line, e.Continue);
		}

		return;
	}*/
}

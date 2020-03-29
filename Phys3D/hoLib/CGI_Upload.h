//�f�[�^�� CGI ����� Web �T�[�o��ɃA�b�v���[�h����N���X
#pragma once
#include <string>
#include "tstring.h"

namespace ho
{
	class CGI_Upload
	{
	public:
		CGI_Upload(const std::string &strHost); //�R���X�g���N�^
		~CGI_Upload(); //�f�X�g���N�^

		bool Upload(const void *pData, const unsigned int Size); //�f�[�^�𑗐M����
		bool Upload(const ho::tstring &strFilename, const std::string &strVer); //�t�@�C���𑗐M����
	private:
		const std::string strHost; //�z�X�g���܂���IP�A�h���X
	};
}
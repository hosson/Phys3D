#include "Error.h"

namespace ho
{
	std::shared_ptr<ErrorManager> ErrorManager::spErrorManagerObj;

	//�R���X�g���N�^
	ErrorManager::ErrorManager()
	{
		QueryPerformanceFrequency(&this->qpf);
		QueryPerformanceCounter(&this->qpc);

		AllocConsole(); //�R���\�[������
	}

	//�f�X�g���N�^
	ErrorManager::~ErrorManager()
	{
		FreeConsole(); //�R���\�[�����
	}

	//�G���[����ǉ�����
	void ErrorManager::addErrorInfo(std::shared_ptr<ErrorInfo_Base> spErrorInfoObj)
	{
		_cputts(spErrorInfoObj->getErrorStr(this->spErrorManagerObj).c_str()); //�G���[��������R���\�[���֏o��

		
		if (spErrorInfoObj->getExit()) //���s�ł��Ȃ��G���[�̏ꍇ
		{
			MessageBox(NULL, spErrorInfoObj->getErrorStr(this->spErrorManagerObj).c_str(), TEXT("�A�v���P�[�V���������s�ł��Ȃ��G���["), MB_OK); //���b�Z�[�W�{�b�N�X�\��
			std::exit(-1); //���̏�ŏI��
		}
		

		return;
	}

	//�R���X�g���N�^
	ErrorInfo_Base::ErrorInfo_Base(const bool &Exit)
		: Exit(Exit)
	{
	}

	//�f�X�g���N�^
	ErrorInfo_Base::~ErrorInfo_Base()
	{
	}


	//�R���X�g���N�^
	ErrorInfo::ErrorInfo(const bool &Exit, const tstring &str)
		: ErrorInfo_Base(Exit), str(str)
	{}

	//�f�X�g���N�^
	ErrorInfo::~ErrorInfo()
	{}

	//�G���[��������擾
	tstring ErrorInfo::getErrorStr(std::shared_ptr<ErrorManager> spErrorManagerObj) const
	{
		return this->str;
	}


	//�R���X�g���N�^
	ErrorInfoDetail::ErrorInfoDetail(const bool &Exit, const tstring &str, const tstring &strFile, const int &Line)
		: ErrorInfo(Exit, str), strFile(strFile), Line(Line)
	{
		QueryPerformanceCounter(&this->qpc);
	}

	//�f�X�g���N�^
	ErrorInfoDetail::~ErrorInfoDetail()
	{}

	//�G���[��������擾
	tstring ErrorInfoDetail::getErrorStr(std::shared_ptr<ErrorManager> spErrorManagerObj) const
	{
		//���ԏ����A�G���[�Ǘ��I�u�W�F�N�g����������Ă���o�߂������ԁi�~���b�P�ʁj�ɕϊ�����
		LARGE_INTEGER t;
		t.QuadPart = this->qpc.QuadPart - spErrorManagerObj->getqpc().QuadPart;
		t.QuadPart = t.QuadPart / LONGLONG(spErrorManagerObj->getqpf().QuadPart * 0.001);

		return tPrint(TEXT("[%lld] %s (%d)\n%s"), t.QuadPart, this->strFile.c_str(), this->Line, this->str.c_str());
	}


	//�R���X�g���N�^
	Exception::Exception(std::shared_ptr<ErrorManager> spErrorManagerObj, const tstring &strMsg, const TCHAR *pFile, const int Line, const bool Exit)
	{
		std::shared_ptr<ErrorInfo> spErrorInfoObj(new ErrorInfo(Exit, strMsg));

		spErrorManagerObj->addErrorInfo(spErrorInfoObj);
	}

	//�R���X�g���N�^
	Exception::Exception(std::shared_ptr<ErrorManager> spErrorManagerObj, const TCHAR *pMsg, const TCHAR *pFile, const int Line, const bool Exit)
	{
		std::shared_ptr<ErrorInfo> spErrorInfoObj(new ErrorInfo(Exit, tstring(TEXT("ERROR!"))));

		spErrorManagerObj->addErrorInfo(spErrorInfoObj);
	}

}
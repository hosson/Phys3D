//�G���[���̏������s���N���X
#pragma once

#include "tstring.h"
#include <Windows.h>
#include <boost/shared_ptr.hpp>

//�G���[�o�̓}�N��
#define ERR(Exit, str, strFile, Line) ho::ErrorManager::spErrorManagerObj->addErrorInfo(std::shared_ptr<ho::ErrorInfo>(new ho::ErrorInfoDetail(Exit, str, strFile, Line)));
#define ERRc(Exit, str, strFile, Line) ho::ErrorManager::spErrorManagerObj->addErrorInfo(std::shared_ptr<ho::ErrorInfo>(new ho::ErrorInfoDetail(Exit, str, strFile, Line)));

// __FILE__ �� TCHAR �ɑΉ����邽�� __WFILE__ ���g���Ή�������悤�ɂ���}�N��
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#if _UNICODE //Unicode�̏ꍇ
#define __WFILE__ WIDEN(__FILE__) //Unicode�ɕύX
#else
#define __WFILE__ __FILE__ //�}���`�o�C�g�����̂܂�
#endif

namespace ho
{
	class ErrorInfo_Base;

	//�G���[���Ǘ��N���X
	class ErrorManager
	{
	public:
		ErrorManager(); //�R���X�g���N�^
		~ErrorManager(); //�f�X�g���N�^

		void addErrorInfo(std::shared_ptr<ErrorInfo_Base> spErrorInfoObj); //�G���[����ǉ�����

		static std::shared_ptr<ErrorManager> spErrorManagerObj;

		//�A�N�Z�b�T
		const LARGE_INTEGER &getqpf() const { return qpf; }
		const LARGE_INTEGER &getqpc() const { return qpc; }
	private:
		LARGE_INTEGER qpf; //QueryPerformanceFrequency �̒l
		LARGE_INTEGER qpc; //�I�u�W�F�N�g�������� QueryPerformanceCounter �̒l
	};

	//��̃G���[����\�����N���X
	class ErrorInfo_Base
	{
	public:
		ErrorInfo_Base(const bool &Exit); //�R���X�g���N�^
		virtual ~ErrorInfo_Base(); //�f�X�g���N�^

		virtual tstring getErrorStr(std::shared_ptr<ErrorManager> spErrorManagerObj) const { return tstring(TEXT("Require Override.")); } //�G���[��������擾

		//�A�N�Z�b�T
		const bool getExit() const { return Exit; }
	protected:
		const bool Exit; //�G���[�����Ɠ����ɃA�v���P�[�V�������I�����邩�ǂ���
	};

	//�ʏ�̃G���[����\���N���X
	class ErrorInfo : public ErrorInfo_Base
	{
	public:
		ErrorInfo(const bool &Exit, const tstring &str); //�R���X�g���N�^
		virtual ~ErrorInfo(); //�f�X�g���N�^

		virtual tstring getErrorStr(std::shared_ptr<ErrorManager> spErrorManagerObj) const override; //�G���[��������擾
	protected:
		const tstring str; //�G���[������
	};

	//�G���[���������A�\�[�X�R�[�h�t�@�C�����A�s�ԍ��A���܂ރG���[����\���N���X
	class ErrorInfoDetail : public ErrorInfo
	{
	public:
		ErrorInfoDetail(const bool &Exit, const tstring &str, const tstring &strFile, const int &Line); //�R���X�g���N�^
		virtual ~ErrorInfoDetail(); //�f�X�g���N�^

		virtual tstring getErrorStr(std::shared_ptr<ErrorManager> spErrorManagerObj) const override; //�G���[��������擾
	protected:
		LARGE_INTEGER qpc; //�G���[��������
		const tstring strFile; //�\�[�X�R�[�h�t�@�C����
		const int Line; //�s��
	};

	//��O�I�u�W�F�N�g
	class Exception
	{
	public:
		Exception(std::shared_ptr<ErrorManager> spErrorManagerObj, const tstring &strMsg, const TCHAR *pFile, const int Line, const bool Exit); //�R���X�g���N�^
		Exception(std::shared_ptr<ErrorManager> spErrorManagerObj, const TCHAR *pMsg, const TCHAR *pFile, const int Line, const bool Exit); //�R���X�g���N�^

		tstring strMsg;
		TCHAR *pFile;
		int Line;
		bool Exit;
	};

}
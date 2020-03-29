//���C�u���������̃G���[����������N���X
#pragma once
#include <tchar.h>
#include "tstring.h"

//���C�u�����p�G���[�o�̓}�N��
#define ER(Msg, File, Line, Continue) LibError::pLibErrorObj->Throw(Msg, File, Line, Continue); 

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
	//�O���錾
	class ErrorReceiver;

	class LibError
	{
	public:
		LibError(); //�R���X�g���N�^
		~LibError() {} //�f�X�g���N�^

		void Throw(const TCHAR *pMsg, const TCHAR *pFile, const int Line, const bool Continue = true); //�G���[����

		static void GetLastErrorStr(tstring &str); //�G���[�����擾
		static LibError *pLibErrorObj;

		//�A�N�Z�b�T
		void SetpErrorReceiverObj(ErrorReceiver *pObj) { this->pErrorReceiverObj = pObj; }
	private:
		ErrorReceiver *pErrorReceiverObj;
	};

	class ErrorReceiver
	{
	public:
		ErrorReceiver() {} //�R���X�g���N�^
		virtual ~ErrorReceiver() {} //�f�X�g���N�^
		virtual void Receive(const TCHAR *pMsg, const TCHAR *pFile, const int Line, const bool Continue) {} //�G���[���󂯎��
	};

	//��O�I�u�W�F�N�g
	class Exception
	{
	public:
		Exception(const tstring &strMsg, const TCHAR *pFile, const int Line, const bool Continue); //�R���X�g���N�^
		Exception(const TCHAR *pMsg, const TCHAR *pFile, const int Line, const bool Continue); //�R���X�g���N�^
		tstring strMsg;
		TCHAR *pFile;
		int Line;
		bool Continue;
	};
}

/*
ho::hoLib �N���X�̃I�u�W�F�N�g���쐬����Ǝ����� Error �I�u�W�F�N�g���쐬����A
���C�u���������Ŕ��������G���[���͈�U�����ɏW�񂳂��B

ErrorReceiver �N���X���p������ Receive() ���\�b�h���I�[�o�[���C�h�����N���X��
�v���O�����{�̑��ɍ��ƁA�v���O�����{�̂Ŏg���G���[�����@�\�փG���[���e�𑗐M���邱�Ƃ��ł���B
*/
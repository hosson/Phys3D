//�Ăяo���������`�F�b�N����N���X
#pragma once
#include "tstring.h"
#include <boost/shared_ptr.hpp>

namespace ho
{
	class OrderChecker
	{
	public:
		OrderChecker(const int &AllCheckerNum, const tstring &strName); //�R���X�g���N�^
		~OrderChecker(); //�f�X�g���N�^

		bool Check(const int &Num); //�`�F�b�N���s
	private:
		const int AllCheckerNum; //�S�̂̃`�F�b�J�[��
		const tstring strName; //�`�F�b�N�Ώۂ̖���
		int NowCheckerNum; //���݂̃`�F�b�J�[�ԍ�
	};
}

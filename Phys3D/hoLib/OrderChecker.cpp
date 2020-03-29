#include "OrderChecker.h"
#include "Error.h"

namespace ho
{
	//�R���X�g���N�^
	OrderChecker::OrderChecker(const int &AllCheckerNum, const tstring &strName)
		: AllCheckerNum(AllCheckerNum), strName(strName)
	{
		this->NowCheckerNum = 0;
	}

	//�f�X�g���N�^
	OrderChecker::~OrderChecker()
	{
	}

	//�`�F�b�N���s
	bool OrderChecker::Check(const int &Num)
	{
		if (this->NowCheckerNum == Num) //����ȏꍇ
		{
			this->NowCheckerNum = (this->NowCheckerNum + 1) % this->AllCheckerNum; //���݂̃`�F�b�J�[�ԍ������ֈړ�
			return true; //����I��
		}

		//�ُ�ȏꍇ
		ERR(true, tPrint(TEXT("OrderChecker �ɂ�� %s �̏����G���[�����o����܂����B\n%d �Ԃ̗\��ɑ΂��Ď��ۂɂ� %d �Ԃ��Ă΂�܂����B"), this->strName.c_str(), this->NowCheckerNum, Num), __WFILE__,__LINE__);

		return false; //�G���[
	}
}


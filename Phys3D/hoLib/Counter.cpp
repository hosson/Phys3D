#include "Counter.h"

namespace ho
{
	//�R���X�g���N�^
	Counter::Counter(const int MaxCountNum) : MaxCountNum(MaxCountNum)
	{
		this->CountNum = 0;
	}

	//�f�X�g���N�^
	Counter::~Counter()
	{
	}

	//�J�E���g����
	bool Counter::Count()
	{
		CountNum++;
		if (CountNum >= MaxCountNum) //�ő�l�ɒB�����ꍇ
		{
			CountNum = 0;
			return true;
		}

		return false;
	}

	//�J�E���^�����Z�b�g
	void Counter::Reset()
	{
		CountNum = 0;

		return;
	}

	//�J�E���^�̒l���ő�l�͈͓̔��̑O�����ǂ���
	bool Counter::IsFirstHalf()
	{
		return CountNum < MaxCountNum * 0.5;
	}

	//�J�E���^�̒l���ő�l�͈͓̔��̌㔼���ǂ���
	bool Counter::IsSecondHalf()
	{
		return CountNum >= int(MaxCountNum * 0.5);
	}



	//�R���X�g���N�^
	CounterF::CounterF(const double &MaxCountNum)
		: MaxCountNum(MaxCountNum)
	{
	}

	//�f�X�g���N�^
	CounterF::~CounterF()
	{
	}

	//�J�E���g����
	bool CounterF::Count(const double &AddNum)
	{
		this->CountNum += AddNum;

		if (this->CountNum >= this->MaxCountNum)
		{
			this->CountNum -= this->MaxCountNum;
			return true; //�ő�l�ɓ��B����
		}

		return false;
	}

	//�J�E���^�����Z�b�g
	void CounterF::Reset()
	{
		this->CountNum = 0.0;

		return;
	}
}


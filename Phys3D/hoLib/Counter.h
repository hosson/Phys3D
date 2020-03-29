//�J�E���^�N���X
#pragma once

namespace ho
{
	class Counter
	{
	public:
		Counter(const int MaxCountNum); //�R���X�g���N�^
		~Counter(); //�f�X�g���N�^

		bool Count(); //�J�E���g����
		void Reset(); //�J�E���^�����Z�b�g
		bool IsFirstHalf(); //�J�E���^�̒l���ő�l�͈͓̔��̑O�����ǂ���
		bool IsSecondHalf(); //�J�E���^�̒l���ő�l�͈͓̔��̌㔼���ǂ���

		//�A�N�Z�b�T
		int GetCountNum() const { return CountNum; }
	private:
		int CountNum; //�J�E���^�̐���
		const int MaxCountNum; //�J�E���^�̍ő�l
	};

	class CounterF
	{
	public:
		CounterF(const double &MaxCountNum); //�R���X�g���N�^
		~CounterF(); //�f�X�g���N�^

		bool Count(const double &AddNum); //�J�E���g����
		void Reset(); //�J�E���^�����Z�b�g
	private:
		double CountNum; //�J�E���^�̌��݂̐���
		const double MaxCountNum; //�J�E���^�̍ő�l
	};
}

//�g�ݍ��킹�i������)�N���X
#pragma once
#include <vector>
#include "LibError.h"
#include "Math.h"

namespace ho
{
	template <class T> class Combination
	{
	public:
		//�R���X�g���N�^�i�������Ȃ��j
		Combination(unsigned int Numbers)
		{
			this->Numbers = Numbers;
			vectorCell.resize(Numbers * Numbers);
		}

		//�R���X�g���N�^�i����������j
		Combination(unsigned int Numbers, const T &Init)
		{
			this->Numbers = Numbers;
			vectorCell.resize(Numbers * Numbers, Init);
		}

		//�f�X�g���N�^
		~Combination()
		{
		}

		//�C�ӂ̗v�f���̑g�ݍ��킹�̒l���擾
		T Get(unsigned int Number1, unsigned int Number2)
		{
#ifdef _DEBUG
			if (Number1 >= Numbers || Number2 >= Numbers)
				ER(TEXT("�����̗v�f�����͈͊O�ł��B"), __WFILE__, __LINE__, true);
#endif
			if (Number1 > Number2)
				ho::Swap(Number1, Number2);

			return vectorCell.at(Number1 * Numbers + Number2);
		}
		//�C�ӂ̗v�f���̑g�ݍ��킹�̒l��ݒ�
		void Set(unsigned int Number1, unsigned int Number2, const T &Value)
		{
#ifdef _DEBUG
			if (Number1 >= Numbers || Number2 >= Numbers)
				ER(TEXT("�����̗v�f�����͈͊O�ł��B"), __WFILE__, __LINE__, true);
#endif
			if (Number1 > Number2)
				ho::Swap(Number1, Number2);

			vectorCell.at(Number1 * Numbers + Number2) = Value;

			return;
		}

	private:
		unsigned int Numbers; //�g�ݍ��킹�̌��ƂȂ鐔���̐�
		std::vector<T> vectorCell; //���ꂼ��̑g�ݍ��킹���
	};
}
//�p�t�H�[�}���X��̓N���X
#pragma once
#include <vector>
#include <Windows.h>

namespace ho
{
	class Profiler
	{
	public:
		Profiler(unsigned int Num); //�R���X�g���N�^
		~Profiler(); //�f�X�g���N�^

		void Begin(unsigned int Index); //�v���J�n
		double End(unsigned int Index); //�v���I��
		double GetTime(unsigned int Index); //�v���������Ԃ��擾
	private:
		std::vector<double> vectorTime; //�^�C�}�[�Ōv����������
		std::vector<LARGE_INTEGER> vectorqpc; //�v���J�n����
		LARGE_INTEGER Freq; //�v���P��
	};
}

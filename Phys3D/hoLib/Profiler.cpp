#include "Profiler.h"
#include "Error.h"

namespace ho
{
	//�R���X�g���N�^
	Profiler::Profiler(unsigned int Num)
	{
		vectorTime.resize(Num);
		vectorqpc.resize(Num);

		if (!QueryPerformanceFrequency(&Freq)) //������\�p�t�H�[�}���X�J�E���^�̎��g�����擾���A�n�[�h�E�F�A���T�|�[�g���Ă��Ȃ��ꍇ
			Freq.QuadPart = 0;
	}

	//�f�X�g���N�^
	Profiler::~Profiler()
	{
	}

	//�v���J�n
	void Profiler::Begin(unsigned int Index)
	{
		if (vectorTime.size() <= Index)
			ERR(true, TEXT("�͈͊O�̃C���f�b�N�X���n����܂����B"), __WFILE__, __LINE__);

		QueryPerformanceCounter(&vectorqpc.at(Index)); //�v���J�n

		return;
	}

	//�v���I��
	double Profiler::End(unsigned int Index)
	{
		if (vectorTime.size() <= Index)
			ERR(true, TEXT("�͈͊O�̃C���f�b�N�X���n����܂����B"), __WFILE__, __LINE__);

		LARGE_INTEGER EndTime;
		QueryPerformanceCounter(&EndTime); //�v���I��

		vectorTime.at(Index) = (double(EndTime.QuadPart - vectorqpc.at(Index).QuadPart) / (double)Freq.QuadPart) * 1000.0; //ms�P�ʂŊi�[

		return vectorTime.at(Index);
	}

	//�v���������Ԃ��擾
	double Profiler::GetTime(unsigned int Index)
	{
		if (vectorTime.size() <= Index)
		{
			ERR(true, TEXT("�͈͊O�̃C���f�b�N�X���n����܂����B"), __WFILE__, __LINE__);
		} else {
			return vectorTime.at(Index);
		}

		return 0;
	}
}

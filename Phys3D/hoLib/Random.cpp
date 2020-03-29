#include "Random.h"
#include "Error.h"
#define PI 3.14159265358979

namespace ho
{
	//�W�������^�C�����C�u�����̃����_���֐���������
	void Randomize()
	{
		SYSTEMTIME st;
		unsigned int TotalTime;	GetLocalTime(&st);
		TotalTime = st.wDay + st.wHour + st.wMinute + st.wSecond + st.wMilliseconds;
		srand(TotalTime);

		return;
	}

	//�C�ӂ͈̔͂̃����_���̒l���擾����
	double GetRand(const double &Min, const double &Max)
	{
		return double((rand() * (double(Max - Min) / (double)RAND_MAX)) + Min);
	}

	//�C�ӂ͈̔͂̃����_���̒l���擾����
	int GetRand(const int Min, const int Max)
	{
		if (Min >= Max)
		{
			ERR(false, TEXT("�����_���֐��̈����̒l���s���ł��B"), __WFILE__, __LINE__);
		} else if (Max - Min < 0)
		{
			ERR(true, TEXT("�����_���֐��̈����̒l���s���ł��B"), __WFILE__, __LINE__); //���s�s�\
		}

		return (rand() % (Max - Min + 1)) + Min;
	}

	//���K���z�̃����_���l���擾�i�{�b�N�X�E�~���[���[�@�j
	double GetBMRand(const double &Center, const double &Width)
	{
		double r1 = rand() / 32767.1, r2 = rand() / 32767.1;

		return Width * sqrt(-2.0 * log(r1)) * cos(2.0 * PI * r2) + Center;
	}




	//�R���X�g���N�^
	xorShift::xorShift()
	{
		this->getNum = 0;
		this->initNum = 0;

		Init(); //�������������
	}

	//�f�X�g���N�^
	xorShift::~xorShift() 
	{
	}

	//�������������
	void xorShift::Init() 
	{
		this->initNum++;

		this->x = 123456789;
		this->y = 362436069;
		this->z = 521288629;
		this->w = 88675123;

		return;
	}

	//�����𓾂�
	unsigned long xorShift::get()
	{
		this->getNum++;

		unsigned long t; 
		t = (x ^ (x << 11));
		x = y;
		y = z;
		z = w;

		return (w = (w ^ (w >> 19)) ^ (t ^ (t >>8 ))); 
	}

	//�͈͂��w�肵�ė����l�𓾂�
	int xorShift::get(const int &Min, const int &Max)
	{
		if (Min >= Max)
		{
			ERR(false, TEXT("�����_���֐��̈����̒l���s���ł��B"), __WFILE__, __LINE__);
		} else if (Max - Min < 0)
		{
			ERR(true, TEXT("�����_���֐��̈����̒l���s���ł��B"), __WFILE__, __LINE__); //���s�s�\
		}

		return (this->get() % (Max - Min + 1)) + Min;
	}

	//�͈͂��w�肵�ė����l�𓾂�
	double xorShift::get(const double &Min, const double &Max)
	{
		return this->get() * (double(Max - Min) / (double)UINT_MAX) + Min;
	}

	//�{�b�N�X�E�~���[���[�@�ɂ�鐳�K���z�̗����l�𓾂�
	double xorShift::getBM(const double &Center, const double &Width)
	{
		double r1 = this->get() / ((double)UINT_MAX + 1.0), r2 = this->get() / ((double)UINT_MAX + 1.0);

		return Width * sqrt(-2.0 * log(r1)) * cos(2.0 * PI * r2) + Center;
	}

}

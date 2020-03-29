//�����_���l�Ɋւ��鏈��
#pragma once
#include <Windows.h>

namespace ho
{
	void Randomize(); //�W�������^�C�����C�u�����̃����_���֐���������

	double GetRand(const double &Min, const double &Max); //�C�ӂ͈̔͂̃����_���̒l���擾����
	int GetRand(const int Min, const int Max); //�C�ӂ͈̔͂̃����_���̒l���擾����

	double GetBMRand(const double &Center, const double &Width); //���K���z�̃����_���l���擾�i�{�b�N�X�E�~���[���[�@�j

	//xorShift �@�ɂ�闐������
	class xorShift
	{
	public:
		xorShift(); //�R���X�g���N�^
		~xorShift(); //�f�X�g���N�^

		void Init(); //�������������
		unsigned long get(); //unsigned long �^�̗����l�𒼐ړ���

		int get(const int &Min, const int &Max); //�͈͂��w�肵�ė����l�𓾂�
		double get(const double &Min, const double &Max); //�͈͂��w�肵�ė����l�𓾂�
		double getBM(const double &Center, const double &Width); //�{�b�N�X�E�~���[���[�@�ɂ�鐳�K���z�̗����l�𓾂�

		//�A�N�Z�b�T
		unsigned int getgetNum() const { return getNum; }
	private:
		unsigned long x, y, z, w;
		unsigned int getNum; //get()���Ăяo���ꂽ��
		unsigned int initNum; //Init()���Ăяo���ꂽ��
	};
}

//�{�b�N�X�E�~���[���[�@�̐��K���z�i�Q�l�j
//http://ja.wikipedia.org/wiki/%E3%83%95%E3%82%A1%E3%82%A4%E3%83%AB:Standard_deviation_diagram.svg
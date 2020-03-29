#include "Pitch.h"
#include <math.h>
#include "Math.h"

namespace ho {

	const double NoteNumber::APitch = 442.0; //���A�̎��g���iHz�j

	//�R���X�g���N�^�i�m�[�g�i���o�[�Ŏw��j
	NoteNumber::NoteNumber(int Num) 
	{
		this->Num = Num;
	}

	//�R���X�g���N�^�i�I�N�^�[�u�Ɖ��K�i0�`11�j�Ŏw��j
	NoteNumber::NoteNumber(int Octave, int Scale) 
	{
		this->Num = (Octave + 2) * 12 + Scale;
	}

	//���g���𓾂�
	double NoteNumber::GetPitch() 
	{
		return APitch * pow(2.0, (Num - 69.0) / 12.0);
	}







	const BOOL Tonal::KeyTable[12] = {1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1};
	const TCHAR Tonal::Name[12][8] = { TEXT("C dur"), TEXT("Des dur"), TEXT("D dur"), TEXT("Es dur"), TEXT("E dur"), TEXT("F dur"), TEXT("Fis dur"), TEXT("G dur"), TEXT("As dur"), TEXT("A dur"), TEXT("B dur"), TEXT("H dur") };

	//�R���X�g���N�^
	Tonal::Tonal(int Key)
	{
		this->Key = Key % 12;
	}

	BOOL Tonal::Check(int NoteNum) //�m�[�g�i���o�[�������̍\�������ǂ����𔻒�
	{
		return KeyTable[(NoteNum - Key) % 12];
	}

	void Tonal::ChangeRelative(int Value) //���Γ]��
	{
		Key = (Key + Value) % 12;

		return;
	}

	void Tonal::ChangeAbsolute(int Value) //��Γ]��
	{
		Key = Value % 12;

		return;
	}

	//�m�[�g�i���o�[�����K�ɉ����ď㏸�E���~������
	int Tonal::GetNoteAlongScale(int NoteNum, int Degree) 
	{
		if (Degree >= -1 && Degree <= 1) //-1�x�A0�x�A1�x�̏ꍇ
			return Degree; //�����Ƃ���i�ω����Ȃ��j

		int Sign = ho::GetSign(Degree); //�㏸�����~���𕄍��œ���

		for (int i = 0; i < abs(Degree) - 1; i++)
		{
			int j = 1;
			while (!Check(NoteNum + j * Sign)) //�m�[�g�i���o�[+j �������̍\�����ł͂Ȃ��ꍇ
				j++;
			NoteNum += j * Sign;
		}

		return NoteNum;
	}

	//���݂̒��������擾
	const TCHAR *Tonal::GetpName() 
	{
		return Name[Key];
	}

}
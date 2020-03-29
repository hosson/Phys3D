//�m�[�g�i�����j�⒲���̃N���X

#pragma once
#include <Windows.h>

namespace ho {

//�m�[�g
class NoteNumber
{
	public:
		NoteNumber(int Num); //�R���X�g���N�^�i�m�[�g�i���o�[�Ŏw��j
		NoteNumber(int Octave, int Scale); //�R���X�g���N�^�i�I�N�^�[�u�Ɖ��K�i0�`11�j�Ŏw��j
		~NoteNumber() {} //�f�X�g���N�^

		double GetPitch(); //���g���𓾂�

		//�A�N�Z�b�T
		int GetNum() { return Num; }
		void SetNum(int Num) { this->Num = Num; }
	private:
		int Num; //0�`127�܂ł̃m�[�g�i���o�[
		static const double APitch; //���A�̎��g���iHz�j
	};

	/*MIDI�K�i�Ɠ���0�`127�܂ł̃m�[�g�i���o�[��\���B*/

	//����
	class Tonal
	{
	public:
		Tonal(int Key); //�R���X�g���N�^
		BOOL Check(int NoteNum); //�m�[�g�i���o�[�������̍\�������ǂ����𔻒�
		void ChangeRelative(int Value); //���Γ]��
		void ChangeAbsolute(int Value); //��Γ]��
		int GetNoteAlongScale(int NoteNum, int Degree); //�m�[�g�i���o�[�����K�ɉ����ď㏸�E���~������
		const TCHAR *GetpName(); //���݂̒��������擾
	private:
		int Key; //�����i0=C dur, 1=Cis dur, 2=D dur�c11=H Dur�j
		static const BOOL KeyTable[12]; //�����̍\��������p�e�[�u��
		static const TCHAR Name[12][8]; //������
	};

	/*�I�u�W�F�N�g���Ɉ�̒����������A�m�[�g�i���o�[�Œ����̍\�����𔻒肵���肷��B*/

}
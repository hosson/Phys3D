#include "ScaleName.h"
#include <tchar.h>

namespace ho {
	//�������0�`11�ɕϊ����邽�߂̃e�[�u��
	const TCHAR SCALENAME::ScaleName[34][4] = { TEXT("C"), TEXT("c"),
		TEXT("Cis"), TEXT("cis"), TEXT("Des"), TEXT("des"),
		TEXT("D"), TEXT("d"),
		TEXT("Dis"), TEXT("dis"), TEXT("Es"), TEXT("es"),
		TEXT("E"), TEXT("e"),
		TEXT("F"), TEXT("f"),
		TEXT("Fis"), TEXT("fis"), TEXT("Ges"), TEXT("ges"),
		TEXT("G"), TEXT("g"),
		TEXT("Gis"), TEXT("gis"), TEXT("As"), TEXT("as"),
		TEXT("A"), TEXT("a"),
		TEXT("Ais"), TEXT("ais"), TEXT("B"), TEXT("b"),
		TEXT("H"), TEXT("h")
	};
	const int SCALENAME::ScaleNumber[34] = { 0, 0, 1, 1, 1, 1, 2, 2, 3, 3, 3, 3, 4, 4, 5, 5, 6, 6, 6, 6, 7, 7, 8, 8, 8, 8, 9, 9, 10, 10, 10, 10, 11, 11 };

	//�R���X�g���N�^�i0�`11�Ŏw��j
	SCALENAME::SCALENAME(int Scale)
	{
		this->Scale = Scale;
	}

	//Cis Des ���̃h�C�c�����Ŏw��j
	SCALENAME::SCALENAME(const TCHAR *pScale)
	{
		this->Scale = StringToNumber(pScale); //Cis Des ���̃h�C�c������0�`11�̐����ɕϊ�
	}

	//Cis Des ���̃h�C�c������0�`11�̐����ɕϊ�
	int SCALENAME::StringToNumber(const TCHAR *pStr) 
	{
		//��`����Ă���S�Ẳ�������v���邩�ǂ����𒲂ׂ�
		for (int i = 0; i < 34; i++)
		{
			if (!_tcscmp(pStr, ScaleName[i])) //��v����ꍇ
				return ScaleNumber[i]; //�e�[�u���ɉ�����������Ԃ�
		}

		return -1; //������Ȃ������i���s�����j�ꍇ��-1��Ԃ�
	}

}

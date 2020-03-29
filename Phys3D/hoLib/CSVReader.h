//CSV�`���̃t�@�C����ǂރN���X
#pragma once
#include <Windows.h>
#include <vector>
#include "Common.h"
#include "tstring.h"

namespace ho {
	class CSVReader
	{
	public:
		enum eMode {Resource, File, Memory}; //���\�[�X���t�@�C������\���񋓌^
		enum eDataType {null, Int, Float, Double, String}; //�f�[�^�^��\���񋓌^

		//�R���X�g���N�^
		CSVReader(HMODULE hModule, TCHAR *lpResName, TCHAR *lpResType); //���\�[�X�t�@�C������ǂޏꍇ
		CSVReader(TCHAR *lpFilename); //�t�@�C������ǂޏꍇ
		CSVReader(void *p); //��������̃|�C���^����ǂޏꍇ

		bool Read(int *pBuffer); //���݂̃f�[�^�ʒu�̃f�[�^��int�^�Ƃ��ēǂ�
		bool Read(DWORD *pBuffer); //���݂̃f�[�^�ʒu�̃f�[�^��DWORD�^�iunsigned long�^�j�Ƃ��ēǂ�
		bool Read(float *pBuffer); //���݂̃f�[�^�ʒu�̃f�[�^��float�^�Ƃ��ēǂ�
		bool Read(double *pBuffer); //���݂̃f�[�^�ʒu�̃f�[�^��double�^�Ƃ��ēǂ�
		bool Read(tstring *pBuffer); //���݂̃f�[�^�ʒu�̃f�[�^��tstring�^�Ƃ��ēǂ�

		eDataType GetDataType(); //���݂̃f�[�^�ʒu�̃f�[�^�^��Ԃ�

		//���Z�q
		void operator ++() { NextPos(); } 
	private:
		eMode Mode; //���\�[�X���t�@�C����
		HMODULE hModule; //���W���[����
		tstring strResName; //���\�[�X��
		tstring strResType; //���\�[�X�^�C�v��
		tstring strFilename; //�t�@�C����
		int Pos; //�f�[�^�ʒu
		HGLOBAL hGM;
		TCHAR *p; //���\�[�X�⃁������̃|�C���^
		void *pMemory; //��������̃|�C���^

		void GetStr(tstring *pStr); //���݂̃f�[�^�ʒu�̕�����𓾂�
		int FindStr(TCHAR **lpRes, TCHAR Token, bool SkipDoubleQuotes = false); //���\�[�X��ǒ��Ɏ��̋�؂蕶����T��
		void NextPos(); //�f�[�^�ʒu�����ֈړ�
	};
}

/*CSV�̂悤�ȁC�i�R���}�j�Ńf�[�^����؂�ꂽ�t�@�C������f�[�^�����o���āA
�v���O�������̕ϐ��փf�[�^������N���X�B

�R���X�g���N�^�Ńt�@�C�������w�肷��ƃt�@�C�����J����ARead�n�̖��߂œ��e��ǂ݁A
delete�ȂǂŃf�X�g���N�^���Ă΂��ƃt�@�C�������Ȃǂ̏I���������s����B
*/
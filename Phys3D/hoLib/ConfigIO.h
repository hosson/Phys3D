//�R���t�B�O�t�@�C��������IO��񋟂���N���X

#pragma once
#include <Windows.h>
#include "hoLib.h"
#include "tstring.h"

namespace ho
{
	class ConfigIO
	{
	public:
		ConfigIO(TCHAR *pFilename); //�R���X�g���N�^
		~ConfigIO(); //�f�X�g���N�^

		BOOL Read(TCHAR *pName, int *pValue); //�w�肳�ꂽ�l��ǂށiint�^�j
		BOOL Read(TCHAR *pName, double *pValue); //�w�肳�ꂽ�l��ǂށidouble�^�j
		BOOL Read(TCHAR *pName, tstring &Value); //�w�肳�ꂽ�l��ǂށiTCHAR�^�j
		BOOL Write(TCHAR *pName, int Value); //�w�肳�ꂽ�l���������ށiint�^�j
		BOOL Write(TCHAR *pName, double Value); //�w�肳�ꂽ�l���������ށidouble�^�j
		BOOL Write(TCHAR *pName, TCHAR *pValue); //�w�肳�ꂽ�l��ǂށiTCHAR�^�j
	private:
		tstring strFilename;		
		HANDLE hFile; //�t�@�C���n���h��

		BOOL ReadGetAt(TCHAR *pName, tstring &str); //�t�@�C�����J���� pName �� = �̉E���̕�������擾
		BOOL FileOpen(DWORD dwDesiredAccess, DWORD dwCreationDisposition); //�t�@�C�����J��
		BOOL FileClose(); //�t�@�C�������
		BOOL FindName(TCHAR *pName); //���O�������i���O��=�̉E����hFile�̃|�C���^�𑗂�j
		BOOL SearchAt(TCHAR Key); //Key�̕����񂪌�����܂Ńt�@�C����ǂݑ�����
		TCHAR GetAt(TCHAR *pKeys, int Keys, tstring &strOut); //key�̕��������s�R�[�h��������܂ł̕������lpBuf�֊i�[����
		BOOL SearchAtReturn(); //���s��������܂Ńt�@�C����ǂݑ�����
		BOOL InsertStr(TCHAR *pStr); //�t�@�C���̌��݈ʒu�ɕ������}������
		BOOL DeleteAtReturn(); //���݈ʒu���猻�݂̍s�̉��s��O�܂ł��폜�i���s�͎c��j
	};
}

/*
�e�L�X�g�`���̃R���t�B�O�t�@�C����ǂݏ�������N���X�B
Width=640
�Ȃǂ̂悤�ɁA�L�[�ƂȂ閼�O�ƒl���C�R�[���ŋ�؂���B�f�[�^�͉��s������Ŏ��X�ƋL�^�����B
*/

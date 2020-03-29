//�t�@�C���A�[�J�C�u
#pragma once
#include <Windows.h>
#include "tstring.h"
#include <list>

namespace ho
{
	//�o�[�W���������������邽�߂̊��N���X
	class Archive_Base
	{
	protected:
		static const TCHAR Tag[5], Ver[5];
	};

	//�A�[�J�C�u��ǂރN���X
	class Archive : public Archive_Base
	{
	public:
		Archive(const TCHAR *pArchiveName, bool Debug); //�R���X�g���N�^
		~Archive(); //�f�X�g���N�^

		void *GetFilePointer(const TCHAR *pFilename); //�A�[�J�C�u���̔C�ӂ̃t�@�C���̐擪�̃|�C���^���擾
		void EndFilePointer(); //�|�C���^����I��
		DWORD GetFileSize(); //�t�@�C���|�C���^���擾���Ă���t�@�C���̃T�C�Y���擾����
	private:
		const TCHAR *pArchiveName; //�A�[�J�C�u��
		bool Debug; //�f�o�b�O���[�h
		HANDLE hFile; //�t�@�C���n���h��
		HANDLE hMap; //�}�b�v�n���h��
		void *pFileBeginPointer; //�J�����t�@�C���̐擪�̃|�C���^
		unsigned int NoDebugFileSize; //��f�o�b�O���[�h���̃t�@�C���|�C���^���擾�����A�[�J�C�u�t�@�C�����̌ʃt�@�C���̃T�C�Y
		void *pNoDebugArcBeginPointer; //��f�o�b�O���[�h���� MapViewOfFile() �Ŏ擾�����A�[�J�C�u�t�@�C���擪���w���t�@�C���|�C���^�i�J�������ŕK�v�j

		unsigned int GetFilePos(const tstring& strArcFilename, const tstring &Filename); //�A�[�J�C�u������C�ӂ̃t�@�C���̈ʒu�i�A�[�J�C�u�t�@�C���擪����̃o�C�g���j���擾
	};

	//�A�[�J�C�u�t�@�C���𐶐�����N���X
	class ArchiveCreator : public Archive_Base
	{
	public:
		struct FILEINFO //�X�̃t�@�C�����
		{
			FILEINFO(tstring &strFilename, unsigned int FileSize) //�R���X�g���N�^
				: strFilename(strFilename), FileSize(FileSize)
			{
			}

			tstring strFilename; //�t�@�C����
			unsigned int FileSize; //�t�@�C���T�C�Y
		};
	public:
		ArchiveCreator(const tstring &strPass); //�R���X�g���N�^
		~ArchiveCreator(); //�f�X�g���N�^
	private:
		const tstring strPass; //�R���X�g���N�^�Ŏw�肳�ꂽ�p�X
		std::list<FILEINFO> liststrFileinfo; //�A�[�J�C�u�ɓ����X�̃t�@�C�����

		void EnumFileInfo(); //�A�[�J�C�u����X�̃t�@�C�������
		void Create(); //����
	};
}

/*�����̃t�@�C����1�̃t�@�C���ɂ܂Ƃ߂��A�[�J�C�u�t�@�C���̓ǂݍ��݂��s���N���X�B

class Archive

�t�@�C���̊g���q�� .arc �ƂȂ�B�R���X�g���N�^�ł̓t�@�C������n���B
�f�o�b�O���[�h�� false ���ƃA�[�J�C�u�t�@�C������ǂݏo�����A
true ���ƃA�[�J�C�u�t�@�C���Ɠ����t�H���_�ɂ���A�[�J�C�u�t�@�C��������
�g���q�����������O�̃t�H���_���Q�Ƃ���B
�f�o�b�O���͕p�ɂɃA�[�J�C�u���̃t�@�C���������������邽�߁A
���̋@�\���g�������֗��ł���B

�A�[�J�C�o����͓�����1�̃|�C���^�����擾�ł��Ȃ��B


class ArchiveCreator

�R���X�g���N�^�Ƀt�H���_�̃p�X���w�肷��ƁA���̃t�H���_���̃t�@�C����
���ׂĂ܂Ƃ߂�1�̃A�[�J�C�u�t�@�C���𐶐�����B
�A�[�J�C�u�t�@�C���́A�w�肵���t�H���_�̈��̊K�w�̃t�H���_����
�t�H���_��.arc
�Ƃ����t�@�C�����ō����B
*/

/*�A�[�J�C�u�t�@�C���̓����t�H�[�}�b�g

8Byte(Unicode��4����) "Arc " [�t�H�[�}�b�g���ʗp�^�O]
8Byte(Unicode��4����) "1.00" [�A�[�J�C�u�t�@�C���̃o�[�W����]
4Byte(unsigned int) [�i�[����Ă���t�@�C����]

//�e�t�@�C�����Ƃ̃w�b�_�F�ȉ��̊��ʂ̓��e���A�i�[����Ă���t�@�C����������
{
	512Byte(Unicode��256����) [�t�@�C�����iNULL �ŏI�[��\������j]
	4Byte(unsigned int) [�t�@�C���̊i�[�ʒu]�@�i�A�[�J�C�u�t�@�C���擪���琔����Byte���j
	4Byte(unsigned int) [�t�@�C���T�C�Y]
}

//����ȏ�͏�L�Ɏ����ꂽ�t�@�C���w�b�_�̒��g�݂̂��A�����ċL�^�����

*/
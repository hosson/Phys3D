#include "Archive.h"
#include "LibError.h"

namespace ho
{
	const TCHAR Archive_Base::Tag[] = TEXT("Arc "); //�t�@�C���t�H�[�}�b�g���ʗp�^�O
	const TCHAR Archive_Base::Ver[] = TEXT("1.00"); //�t�@�C���t�H�[�}�b�g�o�[�W����

	//�R���X�g���N�^
	Archive::Archive(const TCHAR *pArchiveName, bool Debug)
	{
		this->pArchiveName = pArchiveName;
		this->Debug = Debug;
		this->pFileBeginPointer = NULL;
	}

	//�f�X�g���N�^
	Archive::~Archive()
	{
		if (pFileBeginPointer) //�|�C���^���擾��Ԃ̏ꍇ
			EndFilePointer(); //�|�C���^�̏I������
	}

	//�A�[�J�C�u���̔C�ӂ̃t�@�C���̐擪�̃|�C���^���擾
	void *Archive::GetFilePointer(const TCHAR *pFilename) 
	{
		try
		{
			if (pFileBeginPointer) //���łɃt�@�C���|�C���^���g�p���Ă���ꍇ
				throw Exception(TEXT("�A�[�J�C�u�̃t�@�C���|�C���^��2�d�Ɏ擾���悤�Ƃ��܂����B"), __WFILE__, __LINE__, false);

			if (Debug) //�f�o�b�O���[�h�̏ꍇ
			{
				//�t�@�C�����𐶐�
				ho::tstring strFilename = pArchiveName;
				strFilename += TEXT("/");
				strFilename += pFilename;

				hFile = CreateFile(strFilename.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0); //�t�@�C�����J��
				if (hFile == INVALID_HANDLE_VALUE) //�J���Ȃ������ꍇ
				{
					tstring str;
					tPrint(str, TEXT("%s ���J���̂Ɏ��s���܂����B"), pFilename);
					throw Exception(str, __WFILE__, __LINE__, true);
				}

				hMap = CreateFileMapping(hFile, 0, PAGE_READONLY, 0, 0, NULL); //�t�@�C���}�b�v�n���h�����擾
				if (hMap == NULL) //�}�b�v�n���h���擾�Ɏ��s�����ꍇ
				{
					tstring str;
					LibError::GetLastErrorStr(str);
					str = TEXT("�t�@�C���}�b�v�n���h���̎擾�Ɏ��s���܂����B") + str;
					throw Exception(str, __WFILE__, __LINE__, true);
				}

				pFileBeginPointer = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0); //�t�@�C���|�C���^���擾
				if (!pFileBeginPointer) //�t�@�C���|�C���^�擾�Ɏ��s�����ꍇ
				{
					tstring str;
					LibError::GetLastErrorStr(str);
					str = TEXT("�t�@�C���|�C���^�̎擾�Ɏ��s���܂����B") + str;
					throw Exception(str, __WFILE__, __LINE__, true);
				}
			} else { //��f�o�b�O���[�h�̏ꍇ
				//�A�[�J�C�u�t�@�C�����𐶐�
				ho::tstring strArcFilename = pArchiveName;
				strArcFilename += TEXT(".arc");

				unsigned int FilePos = GetFilePos(strArcFilename, tstring(pFilename)); //�A�[�J�C�u�t�@�C���̒��̔C�ӂ̃t�@�C���ʒu���擾

				hFile = CreateFile(strArcFilename.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0); //�t�@�C�����J��
				if (hFile == INVALID_HANDLE_VALUE) //�J���Ȃ������ꍇ
				{
					tstring str;
					tPrint(str, TEXT("%s ���J���̂Ɏ��s���܂����B"), pFilename);
					throw Exception(str, __WFILE__, __LINE__, true);
				}

				hMap = CreateFileMapping(hFile, 0, PAGE_READONLY, 0, 0, NULL); //�t�@�C���}�b�v�n���h�����擾
				if (hMap == NULL) //�}�b�v�n���h���擾�Ɏ��s�����ꍇ
				{
					tstring str;
					LibError::GetLastErrorStr(str);
					str = TEXT("�t�@�C���}�b�v�n���h���̎擾�Ɏ��s���܂����B") + str;
					throw Exception(str, __WFILE__, __LINE__, true);
				}

				SYSTEM_INFO si;
				GetSystemInfo(&si);

				pFileBeginPointer = MapViewOfFile(hMap, FILE_MAP_READ, 0,
					(FilePos / si.dwAllocationGranularity) * si.dwAllocationGranularity, //�J�n�ʒu�i�o�C�g�j�i	si.dwAllocationGranularity�̗��x�Ŏw�肷��K�v����j
					this->NoDebugFileSize + (FilePos % si.dwAllocationGranularity)		 //�t�@�C���T�C�Y
				); //�t�@�C���|�C���^���擾
				if (!pFileBeginPointer) //�t�@�C���|�C���^�擾�Ɏ��s�����ꍇ
				{
					tstring str;
					LibError::GetLastErrorStr(str);
					str = TEXT("�t�@�C���|�C���^�̎擾�Ɏ��s���܂����B") + str;
					throw Exception(str, __WFILE__, __LINE__, true);
				}

				pNoDebugArcBeginPointer = pFileBeginPointer; //MapViewOfFile() �Ŏ擾�����|�C���^��ۑ��i������p�j


				pFileBeginPointer = (char *)pFileBeginPointer + (FilePos % si.dwAllocationGranularity); //�t�@�C���i�[�ʒu�փ|�C���^���ړ�
			}
		}
		catch (Exception &e)
		{
			ER(e.strMsg.c_str(), e.pFile, e.Line, e.Continue);
		}

		return pFileBeginPointer;
	}

	//�|�C���^����I��
	void Archive::EndFilePointer() 
	{
		try
		{
			if (!pFileBeginPointer) //�|�C���^�����g�p�̏ꍇ
				throw Exception(TEXT("�t�@�C���|�C���^�����g�p�̏�ԂŏI�����삪�Ă΂�܂����B"), __WFILE__, __LINE__, false);

			void *pClose;
			if (Debug) //�f�o�b�O���[�h��
				pClose = pFileBeginPointer;
			else
				pClose = pNoDebugArcBeginPointer;

			if (!UnmapViewOfFile(pClose)) //�t�@�C���|�C���^���N���[�Y���A���s�����ꍇ
			{
				tstring str;
				LibError::GetLastErrorStr(str);
				str = TEXT("�t�@�C���|�C���^�̃N���[�Y�Ɏ��s���܂����B") + str;
				throw Exception(str, __WFILE__, __LINE__, true);
			}
			pFileBeginPointer = NULL;
			CloseHandle(hMap);
			CloseHandle(hFile);
		}
		catch (Exception &e)
		{
			ER(e.strMsg.c_str(), e.pFile, e.Line, e.Continue);
		}

		return;
	}

	//�t�@�C���|�C���^���擾���Ă���t�@�C���̃T�C�Y���擾����
	DWORD Archive::GetFileSize() 
	{
		if (Debug) //�f�o�b�O���[�h��
			return ::GetFileSize(hFile, NULL);

		return this->NoDebugFileSize;
	}

	//�A�[�J�C�u������C�ӂ̃t�@�C���̈ʒu�i�A�[�J�C�u�t�@�C���擪����̃o�C�g���j���擾
	unsigned int Archive::GetFilePos(const tstring& strArcFilename, const tstring &Filename) 
	{
		//�t�@�C�����J��
		HANDLE hFile = CreateFile(strArcFilename.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) //�t�@�C�����J���Ȃ������ꍇ
		{
			ho::tstring str = strArcFilename + TEXT(" ���J���܂���ł����B");
			throw ho::Exception(str, __WFILE__, __LINE__, false);
		}

		DWORD FileSize = ::GetFileSize(hFile, NULL); //�t�@�C���T�C�Y�擾

		SetFilePointer(hFile, NULL, NULL, FILE_BEGIN); //�t�@�C���|�C���^��擪�Ɏ����Ă���

		//���̓f�[�^��ǂݍ���
		DWORD Bytes;

		//�^�O��o�[�W�����`�F�b�N
		{
			TCHAR CheckBuf[5]; //�o�[�W�����`�F�b�N���ǂݍ��ݗp�o�b�t�@
			ZeroMemory(CheckBuf, sizeof(TCHAR) * 5);

			ReadFile(hFile, CheckBuf, sizeof(TCHAR) * 4, &Bytes, NULL); //�t�H�[�}�b�g�^�O����ǂ�
			if (_tcscmp(CheckBuf, this->Tag)) //��v���Ȃ��ꍇ
			{
				ho::tstring str = strArcFilename + TEXT(" �̓A�[�J�C�u�t�@�C���ł͂���܂���B");
				throw ho::Exception(str, __WFILE__, __LINE__, false);
			}

			ReadFile(hFile, CheckBuf, sizeof(TCHAR) * 4, &Bytes, NULL); //�o�[�W��������ǂ�
			if (_tcscmp(CheckBuf, this->Ver)) //��v���Ȃ��ꍇ
			{
				ho::tstring str = strArcFilename + TEXT(" �̃A�[�J�C�u�t�@�C���t�H�[�}�b�g�̃o�[�W�������Ⴂ�܂��B");
				throw ho::Exception(str, __WFILE__, __LINE__, false);
			}
		}

		unsigned int FileNum; //�A�[�J�C�u���̃t�@�C����
		ReadFile(hFile, &FileNum, sizeof(unsigned int), &Bytes, NULL);

		unsigned int FilePos = 0; //�t�@�C���̊i�[�ʒu
		for (unsigned int i = 0; i < FileNum; i++) //�t�@�C�������𑖍�
		{
			TCHAR FilenameBuf[256]; //�t�@�C������ǂݍ��ނ��߂̃o�b�t�@
			ReadFile(hFile, FilenameBuf, sizeof(TCHAR) * 256, &Bytes, NULL);

			if (!_tcscmp(Filename.c_str(), FilenameBuf)) //�t�@�C��������v�����ꍇ
			{
				ReadFile(hFile, &FilePos, sizeof(unsigned int), &Bytes, NULL); //�t�@�C���i�[�ʒu��ǂ�
				ReadFile(hFile, &this->NoDebugFileSize, sizeof(unsigned int), &Bytes, NULL); //�t�@�C���T�C�Y��ǂ�
				break;
			} else {
				//���̃t�@�C���w�b�_�֔��
				SetFilePointer(hFile, 8, NULL, FILE_CURRENT); //8Byte�i�߂�
			}
		}

		if (!CloseHandle(hFile)) //�t�@�C��������Ȃ������ꍇ
		{
			ho::tstring str = strArcFilename + TEXT(" �����܂���ł����B");
			throw ho::Exception(str, __WFILE__, __LINE__, false);
		}

		hFile = NULL;

		if (!FilePos) //�t�@�C���i�[�ʒu���ǂ܂�Ă��Ȃ������ꍇ�i�t�@�C��������v������̂����������ꍇ�j
		{
			ho::tstring str = strArcFilename + TEXT(" �̒���") + Filename + TEXT("�͑��݂��܂���ł����B");
			throw ho::Exception(str, __WFILE__, __LINE__, false);
		}

		return FilePos; //�t�@�C���̊i�[�ʒu��Ԃ�
	}












	//�R���X�g���N�^
	ArchiveCreator::ArchiveCreator(const tstring &strPass)
		: strPass(strPass)
	{
		/*
		strPass
		�A�[�J�C�u�t�@�C���쐬���ƂȂ�t�H���_�ւ�\�ŏI���p�X������B
		��F
		Res\Resource\
		*/

		EnumFileInfo(); //�A�[�J�C�u����X�̃t�@�C�������
		Create(); //����
	}
	
	//�A�[�J�C�u����X�̃t�@�C�������
	void ArchiveCreator::EnumFileInfo() 
	{
		WIN32_FIND_DATA fd;
		HANDLE hFind;

		try
		{
			if (strPass.back() != TEXT('\\')) //�w�肳�ꂽ�p�X������̍Ōオ \ �ł͂Ȃ��ꍇ
				throw ho::Exception(TEXT("�p�X�i�t�H���_���j���w�肷�镶����̍Ōオ \\ �ł͂���܂���ł����B"), __WFILE__, __LINE__, false);
		
			tstring strFind = strPass + TEXT("*.*"); //�����Ɏg���t�@�C�����𐶐�
			hFind = FindFirstFile(strFind.c_str(), &fd); //�t�@�C�������n���h���擾

			if (hFind == INVALID_HANDLE_VALUE) //�����n���h��������Ɏ擾�ł��Ȃ������ꍇ
			{
				ho::tstring str;
				LibError::GetLastErrorStr(str); //�G���[������擾
				ER(str.c_str(), __WFILE__, __LINE__, false); //���C�u�����G���[�o��
			} else {
				auto fAdd = [&](const WIN32_FIND_DATA &fd) //API�̍\���̂���t�@�C���������X�g�ɒǉ����郉���_��
				{
					if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) //�f�B���N�g���ł͂Ȃ��ꍇ
						liststrFileinfo.push_back(ArchiveCreator::FILEINFO(tstring(fd.cFileName), fd.nFileSizeLow));
				};

				//API���瓾��ꂽ�t�@�C���������X�g�֒ǉ�
				fAdd(fd); //���X�g�ɒǉ�

				//�ȉ��AFindNextFile() ���g���ăt�@�C�������

				while (FindNextFile(hFind, &fd)) //�֐������������i���̃t�@�C�����������ꍇ�j
				{
					fAdd(fd); //���X�g�ɒǉ�
				}

				if (GetLastError() == ERROR_NO_MORE_FILES) //����ȏ�t�@�C����������Ȃ������ꍇ�i����j
				{
				} else {
					ho::tstring str;
					LibError::GetLastErrorStr(str); //�G���[������擾
					ER(str.c_str(), __WFILE__, __LINE__, false); //���C�u�����G���[�o��
				}

				if (!FindClose(hFind)) //�t�@�C���n���h��������ɕ����Ȃ������ꍇ
				{
					ho::tstring str;
					LibError::GetLastErrorStr(str); //�G���[������擾
					ER(str.c_str(), __WFILE__, __LINE__, false); //���C�u�����G���[�o��
				}
			}
		}
		catch (ho::Exception &e)
		{
			ER(e.strMsg.c_str(), e.pFile, e.Line, e.Continue); //���C�u�����G���[�o��
		}

		return;
	}

	//����
	void ArchiveCreator::Create()
	{
		//�A�[�J�C�u�𐶐�����t�@�C�������쐬
		tstring strWriteFilename = this->strPass;
		strWriteFilename.pop_back(); //�Ō�� \ �����
		strWriteFilename += TEXT(".arc"); //�g���q��������
		
		try
		{
			//�t�@�C�����J��
			HANDLE hFile = CreateFile(strWriteFilename.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE) //�t�@�C�����J���Ȃ������ꍇ
			{
				ho::tstring str = strWriteFilename + TEXT(" ���J���܂���ł����B");
				throw ho::Exception(str, __WFILE__, __LINE__, false);
			}

			SetFilePointer(hFile, NULL, NULL, FILE_BEGIN); //�t�@�C���|�C���^��擪�Ɏ����Ă���

			//�w�b�_�������݁i�w�b�_�͍��v�i20Byte�j�j
			DWORD Bytes = sizeof(this->Tag);
			WriteFile(hFile, this->Tag, 8, &Bytes, NULL); //�t�H�[�}�b�g�^�O
			WriteFile(hFile, this->Ver, 8, &Bytes, NULL); //�t�H�[�}�b�g�o�[�W����

			unsigned int FileNumber = this->liststrFileinfo.size(); //�i�[�t�@�C�������擾
			WriteFile(hFile, &FileNumber, sizeof(unsigned int), &Bytes, NULL); //�i�[�t�@�C����

			//�e�t�@�C���̃w�b�_�����������ށi1�t�@�C���ɂ�520Byte�j
			for (std::list<FILEINFO>::iterator itr = this->liststrFileinfo.begin(); itr != this->liststrFileinfo.end(); itr++) //�t�@�C�����𑖍�
			{
				if (itr->strFilename.size() > 255) //�t�@�C��������������ꍇ
					throw ho::Exception(TEXT("��������t�@�C����������܂����B"), __WFILE__, __LINE__, false);

				TCHAR Filename[256]; //�t�@�C�����o�b�t�@(NULL�ŏI�[��\������t�@�C�����j
				ZeroMemory(Filename, sizeof(TCHAR) * 256); //0�Ŗ��߂�
				memcpy(Filename, itr->strFilename.c_str(), itr->strFilename.size() * sizeof(TCHAR)); //��������o�b�t�@�փR�s�[

				//�i�[�ʒu���v�Z
				unsigned int PutAddr = 20 + this->liststrFileinfo.size() * (512 + 4 + 4); //�擪�w�b�_�Ɗe�t�@�C�����Ƃ̃w�b�_�̍��v
				//���O�܂ł̃t�@�C�����𑖍����ăt�@�C���T�C�Y�𑫂�
				for (std::list<FILEINFO>::iterator itr2 = this->liststrFileinfo.begin(); itr2 != itr; itr2++) //�t�@�C�����𑖍�
					PutAddr += itr2->FileSize;

				WriteFile(hFile, Filename, sizeof(TCHAR) * 256, &Bytes, NULL); //�t�@�C�����i512Byte�j
				WriteFile(hFile, &PutAddr, sizeof(unsigned int), &Bytes, NULL); //�t�@�C���i�[�ʒu
				WriteFile(hFile, &itr->FileSize, sizeof(unsigned int), &Bytes, NULL); //�t�@�C���T�C�Y

			}

			//�e�t�@�C���̒��g����������
			for (std::list<FILEINFO>::iterator itr = this->liststrFileinfo.begin(); itr != this->liststrFileinfo.end(); itr++) //�t�@�C�����𑖍�
			{
				void *pBuf = malloc(itr->FileSize); //�o�b�t�@�m��

				//�ǂݍ���
				{
					if (!pBuf)
						throw ho::Exception(TEXT("�o�b�t�@�m�ۂɎ��s���܂����B"), __WFILE__, __LINE__, false);

					tstring strReadFilename = this->strPass + itr->strFilename; //�ǂݍ��݃t�@�C����

					//�t�@�C�����J��
					HANDLE hFileRead = CreateFile(strReadFilename.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if (hFileRead == INVALID_HANDLE_VALUE) //�t�@�C�����J���Ȃ������ꍇ
					{
						ho::tstring str = strReadFilename + TEXT(" ���J���܂���ł����B");
						throw ho::Exception(str, __WFILE__, __LINE__, true);
					}

					DWORD FileSize = GetFileSize(hFileRead, NULL); //�t�@�C���T�C�Y�擾

					SetFilePointer(hFileRead, NULL, NULL, FILE_BEGIN); //�t�@�C���|�C���^��擪�Ɏ����Ă���

					//���̓f�[�^��ǂݍ���
					DWORD Bytes;

					ReadFile(hFileRead, pBuf, itr->FileSize, &Bytes, NULL); //�t�@�C���̒��g�S����ǂݍ���

					if (!CloseHandle(hFileRead)) //�t�@�C��������Ȃ������ꍇ
					{
						ho::tstring str = strReadFilename + TEXT(" �����܂���ł����B");
						throw ho::Exception(str, __WFILE__, __LINE__, true);
					}
					
					hFileRead = NULL;
				}

				//��������
				{
					WriteFile(hFile, pBuf, itr->FileSize, &Bytes, NULL); 
				}


				free(pBuf); //�o�b�t�@�J��
			}

			if (!CloseHandle(hFile)) //�t�@�C��������Ȃ������ꍇ
			{
				ho::tstring str = strWriteFilename + TEXT(" �����܂���ł����B");
				throw ho::Exception(str, __WFILE__, __LINE__, true);
			}

			hFile = NULL;
		}
		catch (ho::Exception &e)
		{
			ER(e.strMsg.c_str(), e.pFile, e.Line, e.Continue); //���C�u�����G���[�o��
		}


		return;
	}

	//�f�X�g���N�^
	ArchiveCreator::~ArchiveCreator()
	{
	}
}






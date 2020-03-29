#include "EnvironmentInfo.h"
#include <Windows.h>
#include "Debug.h"

//�R���X�g���N�^
ho::EnvironmentInfo::EnvironmentInfo()
{
	//OS�̖��O
	OSVERSIONINFO ovi;
	ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!GetVersionEx(&ovi))
	{
		tstring str;
		LibError::GetLastErrorStr(str);
		ERR(str.c_str(), __WFILE__, __LINE__, true);
	} else {
		//�o�[�W�����ԍ����疼�O���o��
		switch (ovi.dwPlatformId)
		{
		case VER_PLATFORM_WIN32s: //Windows 3.1��Ŏ��s����Win32s
			this->strOS = TEXT("Windows 3.1");
			break;
		case VER_PLATFORM_WIN32_WINDOWS: //Windows 9x�x�[�X��Win32
			switch (ovi.dwMinorVersion)
			{
			case 0:
				this->strOS = TEXT("Windows 95");
				break;
			case 10:
				this->strOS = TEXT("Windows 98");
				break;
			case 90:
				this->strOS = TEXT("Windows Me");
				break;
			}
			break;
		case VER_PLATFORM_WIN32_NT: //Windows NT�x�[�X��Win32
			switch (ovi.dwMajorVersion)
			{
			case 4:
				this->strOS = TEXT("Windows NT 4.0");
				break;
			case 5:
				switch (ovi.dwMinorVersion)
				{
				case 0:
					this->strOS = TEXT("Windows 2000");
					break;
				case 1:
					this->strOS = TEXT("Windows XP");
					break;
				case 2:
					this->strOS = TEXT("Windows XP x64");
					break;
				}
				break;
			case 6:
				switch (ovi.dwMinorVersion)
				{
				case 0:
					this->strOS = TEXT("Windows Vista");
					break;
				case 1:
					this->strOS = TEXT("Windows 7");
					break;
				case 2:
					this->strOS = TEXT("Windows 8");
					break;
				}
				break;
			}
			break;
		}
	}

	//�v���Z�b�T�[��
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	tPrint(this->strProcessorsNum, TEXT("Number of Processors : %d"), si.dwNumberOfProcessors);

	//�������e��
	MEMORYSTATUSEX ms;
	ms.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&ms);
	tPrint(this->strMemory, TEXT("Physical Memory : %lld Bytes"), ms.ullTotalPhys);
}

//�f�X�g���N�^
ho::EnvironmentInfo::~EnvironmentInfo() 
{
}

//Wave�t�@�C���̓��o�͂��s���N���X

#include <Windows.h>

namespace ho {

class WaveFileWrite
{
public:
	WaveFileWrite() {} //�R���X�g���N�^
	~WaveFileWrite() {} //�f�X�g���N�^

	BOOL Write(TCHAR *pFilename, WAVEFORMATEX *pWFE, void *pData, DWORD DataSize); //��������
private:
};

}
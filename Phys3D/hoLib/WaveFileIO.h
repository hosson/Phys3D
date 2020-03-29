//Waveファイルの入出力を行うクラス

#include <Windows.h>

namespace ho {

class WaveFileWrite
{
public:
	WaveFileWrite() {} //コンストラクタ
	~WaveFileWrite() {} //デストラクタ

	BOOL Write(TCHAR *pFilename, WAVEFORMATEX *pWFE, void *pData, DWORD DataSize); //書き込み
private:
};

}
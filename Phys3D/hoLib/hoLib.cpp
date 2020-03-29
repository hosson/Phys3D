#include "hoLib.h"
#include "LibError.h"
#include <Windows.h>
#include "Random.h"

namespace ho {
	//コンストラクタ
	hoLib::hoLib()
	{
		LibError::pLibErrorObj = new ho::LibError(); //ライブラリ内のエラー処理オブジェクトを作成
		Randomize(); //標準ランタイムライブラリのランダム関数を初期化
	}

	//デストラクタ
	hoLib::~hoLib()
	{
		SDELETE(LibError::pLibErrorObj);
	}
}

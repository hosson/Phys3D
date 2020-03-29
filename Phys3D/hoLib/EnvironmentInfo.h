//OSバージョンなどの環境情報クラス
#pragma once
#include "tstring.h"

namespace ho
{
	class EnvironmentInfo
	{
	public:
		EnvironmentInfo(); //コンストラクタ
		~EnvironmentInfo(); //デストラクタ

		//アクセッサ
		const tstring &GetstrOS() const { return strOS; } 
		const tstring &GetstrProcessorNum() const { return strProcessorsNum; }
		const tstring &GetstrMemory() const { return strMemory; }
	private:
		tstring strOS; //OSの名前（Windows7 など）
		tstring strProcessorsNum; //プロセッサー数
		tstring strMemory; //メモリに関する情報
	};
}

//データを CGI を介して Web サーバ上にアップロードするクラス
#pragma once
#include <string>
#include "tstring.h"

namespace ho
{
	class CGI_Upload
	{
	public:
		CGI_Upload(const std::string &strHost); //コンストラクタ
		~CGI_Upload(); //デストラクタ

		bool Upload(const void *pData, const unsigned int Size); //データを送信する
		bool Upload(const ho::tstring &strFilename, const std::string &strVer); //ファイルを送信する
	private:
		const std::string strHost; //ホスト名またはIPアドレス
	};
}
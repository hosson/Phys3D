//パスに関する関数群

#pragma once
#include <Windows.h>
#include "hoLib.h"
#include "tstring.h"

namespace ho
{
	tstring GetDirectory(TCHAR *pIn); //相対パスからファイル名の部分を消去したものを取得する
	//void SetCurrentDirectoryFromExe(const tstring &strRelativePath); //カレントディレクトリを、実行ファイルから見た相対パスで設定する
}
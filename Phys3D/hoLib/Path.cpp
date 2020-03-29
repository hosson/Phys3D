#include "Path.h"
#include "Debug.h"

namespace ho
{
	//相対パスからファイル名の部分を消去したものを取得する
	tstring GetDirectory(TCHAR *pIn) 
	{
		tstring strDir(pIn);

		int i2 = -1;
		for (int i = strDir.size() - 1; i >= 0; i--) //文字列を逆から走査
		{
			if ((TCHAR)strDir.at(i) == TEXT('/')) //区切り文字が見つかった場合
			{
				i2 = i; //区切り文字の位置を記憶
				break;
			}
		}

		if (i2 != -1) //区切り文字が見つかっていた場合
		{
			strDir.resize(i2 + 1); //区切り文字までは残して、その後を削除
			//strDir.resize(i2); //区切り文字とその後を削除
		}

		return strDir;
	}

	/*
	//カレントディレクトリを、実行ファイルから見た相対パスで設定する
	void SetCurrentDirectoryFromExe(const tstring &strRelativePath) 
	{
		//引数に空の文字列を指定すると、カレントディレクトリが実行ファイルの場所に戻される。

		try
		{
			TCHAR FullPath[MAX_PATH + 1]; //パス名を格納する文字列
			if (!GetModuleFileName(NULL, FullPath, MAX_PATH)) //実行ファイルのフルパス名を取得し、失敗した場合
			{
				tstring str;
				LibError::GetLastErrorStr(str); //エラー文字列取得
				throw ho::Exception(str.c_str(), __WFILE__, __LINE__, false);
			}

			//パスからドライブ名とディレクトリ名のみを抽出
			TCHAR Drive[_MAX_DRIVE + 1], Dir[_MAX_DIR + 1];
			//_tsplitpath(FullPath, Drive, Dir, NULL, NULL); //★この行で警告が出るのでこの部分は現在使われていない

			//ドライブとディレクトリのみを合成
			TCHAR CurrentPath[_MAX_DRIVE + _MAX_DIR + 1]; //実行ファイルが置かれているディレクトリまでの絶対パス
			errno_t error = _tmakepath_s(CurrentPath, sizeof(TCHAR) * (_MAX_DRIVE + _MAX_DIR + 1), Drive, Dir, NULL, NULL);
			if (error) //エラーが発生した場合
			{
				tstring str;
				tPrint(str, TEXT("ドライブ名とディレクトリ名の合成時にエラーが発生しました。エラーコード = %d"), error);
				throw ho::Exception(str.c_str(), __WFILE__, __LINE__, false);
			}

			tstring strSetPath = CurrentPath + strRelativePath; //実行ファイルが置かれているディレクトリまでの絶対パスと引数の相対パスを合成する
			if (!SetCurrentDirectory(strSetPath.c_str())) //カレントディレクトリを設定し、エラーが出た場合
			{
				tstring str;
				LibError::GetLastErrorStr(str);
				throw ho::Exception(str.c_str(), __WFILE__, __LINE__, false);
			}
		}
		catch (ho::Exception &e)
		{
			ER(e.strMsg.c_str(), e.pFile, e.Line, e.Continue);
		}

		return;
	}*/
}

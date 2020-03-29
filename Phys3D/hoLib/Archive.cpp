#include "Archive.h"
#include "LibError.h"

namespace ho
{
	const TCHAR Archive_Base::Tag[] = TEXT("Arc "); //ファイルフォーマット識別用タグ
	const TCHAR Archive_Base::Ver[] = TEXT("1.00"); //ファイルフォーマットバージョン

	//コンストラクタ
	Archive::Archive(const TCHAR *pArchiveName, bool Debug)
	{
		this->pArchiveName = pArchiveName;
		this->Debug = Debug;
		this->pFileBeginPointer = NULL;
	}

	//デストラクタ
	Archive::~Archive()
	{
		if (pFileBeginPointer) //ポインタが取得状態の場合
			EndFilePointer(); //ポインタの終了処理
	}

	//アーカイブ内の任意のファイルの先頭のポインタを取得
	void *Archive::GetFilePointer(const TCHAR *pFilename) 
	{
		try
		{
			if (pFileBeginPointer) //すでにファイルポインタを使用している場合
				throw Exception(TEXT("アーカイブのファイルポインタを2重に取得しようとしました。"), __WFILE__, __LINE__, false);

			if (Debug) //デバッグモードの場合
			{
				//ファイル名を生成
				ho::tstring strFilename = pArchiveName;
				strFilename += TEXT("/");
				strFilename += pFilename;

				hFile = CreateFile(strFilename.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0); //ファイルを開く
				if (hFile == INVALID_HANDLE_VALUE) //開けなかった場合
				{
					tstring str;
					tPrint(str, TEXT("%s を開くのに失敗しました。"), pFilename);
					throw Exception(str, __WFILE__, __LINE__, true);
				}

				hMap = CreateFileMapping(hFile, 0, PAGE_READONLY, 0, 0, NULL); //ファイルマップハンドルを取得
				if (hMap == NULL) //マップハンドル取得に失敗した場合
				{
					tstring str;
					LibError::GetLastErrorStr(str);
					str = TEXT("ファイルマップハンドルの取得に失敗しました。") + str;
					throw Exception(str, __WFILE__, __LINE__, true);
				}

				pFileBeginPointer = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0); //ファイルポインタを取得
				if (!pFileBeginPointer) //ファイルポインタ取得に失敗した場合
				{
					tstring str;
					LibError::GetLastErrorStr(str);
					str = TEXT("ファイルポインタの取得に失敗しました。") + str;
					throw Exception(str, __WFILE__, __LINE__, true);
				}
			} else { //非デバッグモードの場合
				//アーカイブファイル名を生成
				ho::tstring strArcFilename = pArchiveName;
				strArcFilename += TEXT(".arc");

				unsigned int FilePos = GetFilePos(strArcFilename, tstring(pFilename)); //アーカイブファイルの中の任意のファイル位置を取得

				hFile = CreateFile(strArcFilename.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0); //ファイルを開く
				if (hFile == INVALID_HANDLE_VALUE) //開けなかった場合
				{
					tstring str;
					tPrint(str, TEXT("%s を開くのに失敗しました。"), pFilename);
					throw Exception(str, __WFILE__, __LINE__, true);
				}

				hMap = CreateFileMapping(hFile, 0, PAGE_READONLY, 0, 0, NULL); //ファイルマップハンドルを取得
				if (hMap == NULL) //マップハンドル取得に失敗した場合
				{
					tstring str;
					LibError::GetLastErrorStr(str);
					str = TEXT("ファイルマップハンドルの取得に失敗しました。") + str;
					throw Exception(str, __WFILE__, __LINE__, true);
				}

				SYSTEM_INFO si;
				GetSystemInfo(&si);

				pFileBeginPointer = MapViewOfFile(hMap, FILE_MAP_READ, 0,
					(FilePos / si.dwAllocationGranularity) * si.dwAllocationGranularity, //開始位置（バイト）（	si.dwAllocationGranularityの粒度で指定する必要あり）
					this->NoDebugFileSize + (FilePos % si.dwAllocationGranularity)		 //ファイルサイズ
				); //ファイルポインタを取得
				if (!pFileBeginPointer) //ファイルポインタ取得に失敗した場合
				{
					tstring str;
					LibError::GetLastErrorStr(str);
					str = TEXT("ファイルポインタの取得に失敗しました。") + str;
					throw Exception(str, __WFILE__, __LINE__, true);
				}

				pNoDebugArcBeginPointer = pFileBeginPointer; //MapViewOfFile() で取得したポインタを保存（解放時用）


				pFileBeginPointer = (char *)pFileBeginPointer + (FilePos % si.dwAllocationGranularity); //ファイル格納位置へポインタを移動
			}
		}
		catch (Exception &e)
		{
			ER(e.strMsg.c_str(), e.pFile, e.Line, e.Continue);
		}

		return pFileBeginPointer;
	}

	//ポインタ操作終了
	void Archive::EndFilePointer() 
	{
		try
		{
			if (!pFileBeginPointer) //ポインタが未使用の場合
				throw Exception(TEXT("ファイルポインタが未使用の状態で終了操作が呼ばれました。"), __WFILE__, __LINE__, false);

			void *pClose;
			if (Debug) //デバッグモード時
				pClose = pFileBeginPointer;
			else
				pClose = pNoDebugArcBeginPointer;

			if (!UnmapViewOfFile(pClose)) //ファイルポインタをクローズし、失敗した場合
			{
				tstring str;
				LibError::GetLastErrorStr(str);
				str = TEXT("ファイルポインタのクローズに失敗しました。") + str;
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

	//ファイルポインタを取得しているファイルのサイズを取得する
	DWORD Archive::GetFileSize() 
	{
		if (Debug) //デバッグモード時
			return ::GetFileSize(hFile, NULL);

		return this->NoDebugFileSize;
	}

	//アーカイブ中から任意のファイルの位置（アーカイブファイル先頭からのバイト数）を取得
	unsigned int Archive::GetFilePos(const tstring& strArcFilename, const tstring &Filename) 
	{
		//ファイルを開く
		HANDLE hFile = CreateFile(strArcFilename.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) //ファイルが開けなかった場合
		{
			ho::tstring str = strArcFilename + TEXT(" を開けませんでした。");
			throw ho::Exception(str, __WFILE__, __LINE__, false);
		}

		DWORD FileSize = ::GetFileSize(hFile, NULL); //ファイルサイズ取得

		SetFilePointer(hFile, NULL, NULL, FILE_BEGIN); //ファイルポインタを先頭に持ってくる

		//入力データを読み込む
		DWORD Bytes;

		//タグやバージョンチェック
		{
			TCHAR CheckBuf[5]; //バージョンチェック情報読み込み用バッファ
			ZeroMemory(CheckBuf, sizeof(TCHAR) * 5);

			ReadFile(hFile, CheckBuf, sizeof(TCHAR) * 4, &Bytes, NULL); //フォーマットタグ情報を読む
			if (_tcscmp(CheckBuf, this->Tag)) //一致しない場合
			{
				ho::tstring str = strArcFilename + TEXT(" はアーカイブファイルではありません。");
				throw ho::Exception(str, __WFILE__, __LINE__, false);
			}

			ReadFile(hFile, CheckBuf, sizeof(TCHAR) * 4, &Bytes, NULL); //バージョン情報を読む
			if (_tcscmp(CheckBuf, this->Ver)) //一致しない場合
			{
				ho::tstring str = strArcFilename + TEXT(" のアーカイブファイルフォーマットのバージョンが違います。");
				throw ho::Exception(str, __WFILE__, __LINE__, false);
			}
		}

		unsigned int FileNum; //アーカイブ内のファイル数
		ReadFile(hFile, &FileNum, sizeof(unsigned int), &Bytes, NULL);

		unsigned int FilePos = 0; //ファイルの格納位置
		for (unsigned int i = 0; i < FileNum; i++) //ファイル数分を走査
		{
			TCHAR FilenameBuf[256]; //ファイル名を読み込むためのバッファ
			ReadFile(hFile, FilenameBuf, sizeof(TCHAR) * 256, &Bytes, NULL);

			if (!_tcscmp(Filename.c_str(), FilenameBuf)) //ファイル名が一致した場合
			{
				ReadFile(hFile, &FilePos, sizeof(unsigned int), &Bytes, NULL); //ファイル格納位置を読む
				ReadFile(hFile, &this->NoDebugFileSize, sizeof(unsigned int), &Bytes, NULL); //ファイルサイズを読む
				break;
			} else {
				//次のファイルヘッダへ飛ぶ
				SetFilePointer(hFile, 8, NULL, FILE_CURRENT); //8Byte進める
			}
		}

		if (!CloseHandle(hFile)) //ファイルが閉じれなかった場合
		{
			ho::tstring str = strArcFilename + TEXT(" を閉じれませんでした。");
			throw ho::Exception(str, __WFILE__, __LINE__, false);
		}

		hFile = NULL;

		if (!FilePos) //ファイル格納位置が読まれていなかった場合（ファイル名が一致するものが無かった場合）
		{
			ho::tstring str = strArcFilename + TEXT(" の中に") + Filename + TEXT("は存在しませんでした。");
			throw ho::Exception(str, __WFILE__, __LINE__, false);
		}

		return FilePos; //ファイルの格納位置を返す
	}












	//コンストラクタ
	ArchiveCreator::ArchiveCreator(const tstring &strPass)
		: strPass(strPass)
	{
		/*
		strPass
		アーカイブファイル作成元となるフォルダへの\で終わるパスを入れる。
		例：
		Res\Resource\
		*/

		EnumFileInfo(); //アーカイブする個々のファイル情報を列挙
		Create(); //生成
	}
	
	//アーカイブする個々のファイル情報を列挙
	void ArchiveCreator::EnumFileInfo() 
	{
		WIN32_FIND_DATA fd;
		HANDLE hFind;

		try
		{
			if (strPass.back() != TEXT('\\')) //指定されたパス文字列の最後が \ ではない場合
				throw ho::Exception(TEXT("パス（フォルダ名）を指定する文字列の最後が \\ ではありませんでした。"), __WFILE__, __LINE__, false);
		
			tstring strFind = strPass + TEXT("*.*"); //検索に使うファイル名を生成
			hFind = FindFirstFile(strFind.c_str(), &fd); //ファイル検索ハンドル取得

			if (hFind == INVALID_HANDLE_VALUE) //検索ハンドルが正常に取得できなかった場合
			{
				ho::tstring str;
				LibError::GetLastErrorStr(str); //エラー文字列取得
				ER(str.c_str(), __WFILE__, __LINE__, false); //ライブラリエラー出力
			} else {
				auto fAdd = [&](const WIN32_FIND_DATA &fd) //APIの構造体からファイル情報をリストに追加するラムダ式
				{
					if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) //ディレクトリではない場合
						liststrFileinfo.push_back(ArchiveCreator::FILEINFO(tstring(fd.cFileName), fd.nFileSizeLow));
				};

				//APIから得られたファイル名をリストへ追加
				fAdd(fd); //リストに追加

				//以下、FindNextFile() を使ってファイル名を列挙

				while (FindNextFile(hFind, &fd)) //関数が成功した（次のファイルを見つけた場合）
				{
					fAdd(fd); //リストに追加
				}

				if (GetLastError() == ERROR_NO_MORE_FILES) //これ以上ファイルが見つからなかった場合（正常）
				{
				} else {
					ho::tstring str;
					LibError::GetLastErrorStr(str); //エラー文字列取得
					ER(str.c_str(), __WFILE__, __LINE__, false); //ライブラリエラー出力
				}

				if (!FindClose(hFind)) //ファイルハンドルが正常に閉じられなかった場合
				{
					ho::tstring str;
					LibError::GetLastErrorStr(str); //エラー文字列取得
					ER(str.c_str(), __WFILE__, __LINE__, false); //ライブラリエラー出力
				}
			}
		}
		catch (ho::Exception &e)
		{
			ER(e.strMsg.c_str(), e.pFile, e.Line, e.Continue); //ライブラリエラー出力
		}

		return;
	}

	//生成
	void ArchiveCreator::Create()
	{
		//アーカイブを生成するファイル名を作成
		tstring strWriteFilename = this->strPass;
		strWriteFilename.pop_back(); //最後の \ を取る
		strWriteFilename += TEXT(".arc"); //拡張子を加える
		
		try
		{
			//ファイルを開く
			HANDLE hFile = CreateFile(strWriteFilename.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE) //ファイルが開けなかった場合
			{
				ho::tstring str = strWriteFilename + TEXT(" を開けませんでした。");
				throw ho::Exception(str, __WFILE__, __LINE__, false);
			}

			SetFilePointer(hFile, NULL, NULL, FILE_BEGIN); //ファイルポインタを先頭に持ってくる

			//ヘッダ書き込み（ヘッダは合計（20Byte））
			DWORD Bytes = sizeof(this->Tag);
			WriteFile(hFile, this->Tag, 8, &Bytes, NULL); //フォーマットタグ
			WriteFile(hFile, this->Ver, 8, &Bytes, NULL); //フォーマットバージョン

			unsigned int FileNumber = this->liststrFileinfo.size(); //格納ファイル数を取得
			WriteFile(hFile, &FileNumber, sizeof(unsigned int), &Bytes, NULL); //格納ファイル数

			//各ファイルのヘッダ情報を書き込む（1ファイルにつき520Byte）
			for (std::list<FILEINFO>::iterator itr = this->liststrFileinfo.begin(); itr != this->liststrFileinfo.end(); itr++) //ファイル情報を走査
			{
				if (itr->strFilename.size() > 255) //ファイル名が長すぎる場合
					throw ho::Exception(TEXT("長すぎるファイル名がありました。"), __WFILE__, __LINE__, false);

				TCHAR Filename[256]; //ファイル名バッファ(NULLで終端を表現するファイル名）
				ZeroMemory(Filename, sizeof(TCHAR) * 256); //0で埋める
				memcpy(Filename, itr->strFilename.c_str(), itr->strFilename.size() * sizeof(TCHAR)); //文字列をバッファへコピー

				//格納位置を計算
				unsigned int PutAddr = 20 + this->liststrFileinfo.size() * (512 + 4 + 4); //先頭ヘッダと各ファイルごとのヘッダの合計
				//直前までのファイル情報を走査してファイルサイズを足す
				for (std::list<FILEINFO>::iterator itr2 = this->liststrFileinfo.begin(); itr2 != itr; itr2++) //ファイル情報を走査
					PutAddr += itr2->FileSize;

				WriteFile(hFile, Filename, sizeof(TCHAR) * 256, &Bytes, NULL); //ファイル名（512Byte）
				WriteFile(hFile, &PutAddr, sizeof(unsigned int), &Bytes, NULL); //ファイル格納位置
				WriteFile(hFile, &itr->FileSize, sizeof(unsigned int), &Bytes, NULL); //ファイルサイズ

			}

			//各ファイルの中身を書き込む
			for (std::list<FILEINFO>::iterator itr = this->liststrFileinfo.begin(); itr != this->liststrFileinfo.end(); itr++) //ファイル情報を走査
			{
				void *pBuf = malloc(itr->FileSize); //バッファ確保

				//読み込み
				{
					if (!pBuf)
						throw ho::Exception(TEXT("バッファ確保に失敗しました。"), __WFILE__, __LINE__, false);

					tstring strReadFilename = this->strPass + itr->strFilename; //読み込みファイル名

					//ファイルを開く
					HANDLE hFileRead = CreateFile(strReadFilename.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if (hFileRead == INVALID_HANDLE_VALUE) //ファイルが開けなかった場合
					{
						ho::tstring str = strReadFilename + TEXT(" を開けませんでした。");
						throw ho::Exception(str, __WFILE__, __LINE__, true);
					}

					DWORD FileSize = GetFileSize(hFileRead, NULL); //ファイルサイズ取得

					SetFilePointer(hFileRead, NULL, NULL, FILE_BEGIN); //ファイルポインタを先頭に持ってくる

					//入力データを読み込む
					DWORD Bytes;

					ReadFile(hFileRead, pBuf, itr->FileSize, &Bytes, NULL); //ファイルの中身全部を読み込む

					if (!CloseHandle(hFileRead)) //ファイルが閉じれなかった場合
					{
						ho::tstring str = strReadFilename + TEXT(" を閉じれませんでした。");
						throw ho::Exception(str, __WFILE__, __LINE__, true);
					}
					
					hFileRead = NULL;
				}

				//書き込み
				{
					WriteFile(hFile, pBuf, itr->FileSize, &Bytes, NULL); 
				}


				free(pBuf); //バッファ開放
			}

			if (!CloseHandle(hFile)) //ファイルが閉じれなかった場合
			{
				ho::tstring str = strWriteFilename + TEXT(" を閉じれませんでした。");
				throw ho::Exception(str, __WFILE__, __LINE__, true);
			}

			hFile = NULL;
		}
		catch (ho::Exception &e)
		{
			ER(e.strMsg.c_str(), e.pFile, e.Line, e.Continue); //ライブラリエラー出力
		}


		return;
	}

	//デストラクタ
	ArchiveCreator::~ArchiveCreator()
	{
	}
}






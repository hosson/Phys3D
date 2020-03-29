//ファイルアーカイブ
#pragma once
#include <Windows.h>
#include "tstring.h"
#include <list>

namespace ho
{
	//バージョン情報を持たせるための基底クラス
	class Archive_Base
	{
	protected:
		static const TCHAR Tag[5], Ver[5];
	};

	//アーカイブを読むクラス
	class Archive : public Archive_Base
	{
	public:
		Archive(const TCHAR *pArchiveName, bool Debug); //コンストラクタ
		~Archive(); //デストラクタ

		void *GetFilePointer(const TCHAR *pFilename); //アーカイブ内の任意のファイルの先頭のポインタを取得
		void EndFilePointer(); //ポインタ操作終了
		DWORD GetFileSize(); //ファイルポインタを取得しているファイルのサイズを取得する
	private:
		const TCHAR *pArchiveName; //アーカイブ名
		bool Debug; //デバッグモード
		HANDLE hFile; //ファイルハンドル
		HANDLE hMap; //マップハンドル
		void *pFileBeginPointer; //開いたファイルの先頭のポインタ
		unsigned int NoDebugFileSize; //非デバッグモード時のファイルポインタを取得したアーカイブファイル内の個別ファイルのサイズ
		void *pNoDebugArcBeginPointer; //非デバッグモード時に MapViewOfFile() で取得したアーカイブファイル先頭を指すファイルポインタ（開放処理で必要）

		unsigned int GetFilePos(const tstring& strArcFilename, const tstring &Filename); //アーカイブ中から任意のファイルの位置（アーカイブファイル先頭からのバイト数）を取得
	};

	//アーカイブファイルを生成するクラス
	class ArchiveCreator : public Archive_Base
	{
	public:
		struct FILEINFO //個々のファイル情報
		{
			FILEINFO(tstring &strFilename, unsigned int FileSize) //コンストラクタ
				: strFilename(strFilename), FileSize(FileSize)
			{
			}

			tstring strFilename; //ファイル名
			unsigned int FileSize; //ファイルサイズ
		};
	public:
		ArchiveCreator(const tstring &strPass); //コンストラクタ
		~ArchiveCreator(); //デストラクタ
	private:
		const tstring strPass; //コンストラクタで指定されたパス
		std::list<FILEINFO> liststrFileinfo; //アーカイブに入れる個々のファイル情報

		void EnumFileInfo(); //アーカイブする個々のファイル情報を列挙
		void Create(); //生成
	};
}

/*複数のファイルを1つのファイルにまとめたアーカイブファイルの読み込みを行うクラス。

class Archive

ファイルの拡張子は .arc となる。コンストラクタではファイル名を渡す。
デバッグモードは false だとアーカイブファイルから読み出すが、
true だとアーカイブファイルと同じフォルダにあるアーカイブファイル名から
拡張子を除いた名前のフォルダを参照する。
デバッグ中は頻繁にアーカイブ内のファイルが書き換えられるため、
この機能を使う方が便利である。

アーカイバからは同時に1つのポインタしか取得できない。


class ArchiveCreator

コンストラクタにフォルダのパスを指定すると、そのフォルダ内のファイルを
すべてまとめて1つのアーカイブファイルを生成する。
アーカイブファイルは、指定したフォルダの一つ上の階層のフォルダ内に
フォルダ名.arc
というファイル名で作られる。
*/

/*アーカイブファイルの内部フォーマット

8Byte(Unicodeで4文字) "Arc " [フォーマット識別用タグ]
8Byte(Unicodeで4文字) "1.00" [アーカイブファイルのバージョン]
4Byte(unsigned int) [格納されているファイル数]

//各ファイルごとのヘッダ：以下の括弧の内容が、格納されているファイル数分続く
{
	512Byte(Unicodeで256文字) [ファイル名（NULL で終端を表現する）]
	4Byte(unsigned int) [ファイルの格納位置]　（アーカイブファイル先頭から数えたByte数）
	4Byte(unsigned int) [ファイルサイズ]
}

//これ以上は上記に示されたファイルヘッダの中身のみが連続して記録される

*/
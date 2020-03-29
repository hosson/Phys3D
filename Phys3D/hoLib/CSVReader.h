//CSV形式のファイルを読むクラス
#pragma once
#include <Windows.h>
#include <vector>
#include "Common.h"
#include "tstring.h"

namespace ho {
	class CSVReader
	{
	public:
		enum eMode {Resource, File, Memory}; //リソースかファイルかを表す列挙型
		enum eDataType {null, Int, Float, Double, String}; //データ型を表す列挙型

		//コンストラクタ
		CSVReader(HMODULE hModule, TCHAR *lpResName, TCHAR *lpResType); //リソースファイルから読む場合
		CSVReader(TCHAR *lpFilename); //ファイルから読む場合
		CSVReader(void *p); //メモリ上のポインタから読む場合

		bool Read(int *pBuffer); //現在のデータ位置のデータをint型として読む
		bool Read(DWORD *pBuffer); //現在のデータ位置のデータをDWORD型（unsigned long型）として読む
		bool Read(float *pBuffer); //現在のデータ位置のデータをfloat型として読む
		bool Read(double *pBuffer); //現在のデータ位置のデータをdouble型として読む
		bool Read(tstring *pBuffer); //現在のデータ位置のデータをtstring型として読む

		eDataType GetDataType(); //現在のデータ位置のデータ型を返す

		//演算子
		void operator ++() { NextPos(); } 
	private:
		eMode Mode; //リソースかファイルか
		HMODULE hModule; //モジュール名
		tstring strResName; //リソース名
		tstring strResType; //リソースタイプ名
		tstring strFilename; //ファイル名
		int Pos; //データ位置
		HGLOBAL hGM;
		TCHAR *p; //リソースやメモリ上のポインタ
		void *pMemory; //メモリ上のポインタ

		void GetStr(tstring *pStr); //現在のデータ位置の文字列を得る
		int FindStr(TCHAR **lpRes, TCHAR Token, bool SkipDoubleQuotes = false); //リソース解読中に次の区切り文字を探す
		void NextPos(); //データ位置を次へ移動
	};
}

/*CSVのような，（コンマ）でデータを区切られたファイルからデータを取り出して、
プログラム内の変数へデータを入れるクラス。

コンストラクタでファイル名を指定するとファイルが開かれ、Read系の命令で内容を読み、
deleteなどでデストラクタが呼ばれるとファイルを閉じるなどの終了処理が行われる。
*/
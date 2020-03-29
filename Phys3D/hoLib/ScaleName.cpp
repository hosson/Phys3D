#include "ScaleName.h"
#include <tchar.h>

namespace ho {
	//文字列を0～11に変換するためのテーブル
	const TCHAR SCALENAME::ScaleName[34][4] = { TEXT("C"), TEXT("c"),
		TEXT("Cis"), TEXT("cis"), TEXT("Des"), TEXT("des"),
		TEXT("D"), TEXT("d"),
		TEXT("Dis"), TEXT("dis"), TEXT("Es"), TEXT("es"),
		TEXT("E"), TEXT("e"),
		TEXT("F"), TEXT("f"),
		TEXT("Fis"), TEXT("fis"), TEXT("Ges"), TEXT("ges"),
		TEXT("G"), TEXT("g"),
		TEXT("Gis"), TEXT("gis"), TEXT("As"), TEXT("as"),
		TEXT("A"), TEXT("a"),
		TEXT("Ais"), TEXT("ais"), TEXT("B"), TEXT("b"),
		TEXT("H"), TEXT("h")
	};
	const int SCALENAME::ScaleNumber[34] = { 0, 0, 1, 1, 1, 1, 2, 2, 3, 3, 3, 3, 4, 4, 5, 5, 6, 6, 6, 6, 7, 7, 8, 8, 8, 8, 9, 9, 10, 10, 10, 10, 11, 11 };

	//コンストラクタ（0～11で指定）
	SCALENAME::SCALENAME(int Scale)
	{
		this->Scale = Scale;
	}

	//Cis Des 等のドイツ音名で指定）
	SCALENAME::SCALENAME(const TCHAR *pScale)
	{
		this->Scale = StringToNumber(pScale); //Cis Des 等のドイツ音名を0～11の数字に変換
	}

	//Cis Des 等のドイツ音名を0～11の数字に変換
	int SCALENAME::StringToNumber(const TCHAR *pStr) 
	{
		//定義されている全ての音名を一致するかどうかを調べる
		for (int i = 0; i < 34; i++)
		{
			if (!_tcscmp(pStr, ScaleName[i])) //一致する場合
				return ScaleNumber[i]; //テーブルに応じた数字を返す
		}

		return -1; //見つからなかった（失敗した）場合は-1を返す
	}

}

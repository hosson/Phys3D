//音階の階名のクラス

#include <Windows.h>

namespace ho {
	class SCALENAME
	{
	public:
		SCALENAME(int Scale); //コンストラクタ（0～11で指定）
		SCALENAME(const TCHAR *pScale); //Cis Des 等のドイツ音名で指定）

		int Scale; //C～Bまでの階名

	private:
		int StringToNumber(const TCHAR *pStr); //Cis Des 等のドイツ音名を0～11の数字に変換

		const static TCHAR ScaleName[34][4]; //文字列を0～11に変換するためのテーブル
		const static int ScaleNumber[34]; //文字列を0～11に変換するためのテーブル
	};
}

/*C～Hまでの12種類の音階を持つクラス。
内部的には0～11までの数字で表現されるが、ドイツ音名で表現したり変換したりなどの
外部との柔軟なインターフェースを持つ。*/

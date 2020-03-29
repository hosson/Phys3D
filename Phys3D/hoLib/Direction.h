//方向を定義したenum型
#pragma once
#include "PT.h"

namespace ho
{
	namespace e2LR
	{
		enum e {left, right};
	}
	namespace e2UD
	{
		enum e {up, down};
	}
	namespace e4Direction
	{
		enum e {left, right, up, down};
	}

	//8方向を示すクラス
	class Direction8 
	{
	public:
		enum eDirection { LeftUp, Up, RightUp, Left, Center, Right, LeftDown, Down, RightDown };

		//コンストラクタ
		Direction8() { Direction = Center; }
		Direction8(int Direction) { this->Direction = (eDirection)Direction; }
		Direction8(eDirection Direction) { this->Direction = Direction; }

		eDirection Direction; //方向を格納する変数
		PT<int> GetXY() { return PT<int>((Direction % 3) - 1, Direction / 3 - 1); } //方向を2次元座標（左上に(0, 0)が来る座標系）で取得
	};

}

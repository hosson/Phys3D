//�������`����enum�^
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

	//8�����������N���X
	class Direction8 
	{
	public:
		enum eDirection { LeftUp, Up, RightUp, Left, Center, Right, LeftDown, Down, RightDown };

		//�R���X�g���N�^
		Direction8() { Direction = Center; }
		Direction8(int Direction) { this->Direction = (eDirection)Direction; }
		Direction8(eDirection Direction) { this->Direction = Direction; }

		eDirection Direction; //�������i�[����ϐ�
		PT<int> GetXY() { return PT<int>((Direction % 3) - 1, Direction / 3 - 1); } //������2�������W�i�����(0, 0)��������W�n�j�Ŏ擾
	};

}

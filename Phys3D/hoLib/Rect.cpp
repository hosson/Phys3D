#include "Rect.h"
#include <vector>
#include <algorithm>

namespace ho
{
	//�l�̈�ԏ�����������Ԃ�
	e4Direction::e RECT::GetMinDirection() const
	{
		std::vector<int> vec(4);
		vec.at(0) = left;
		vec.at(1) = top;
		vec.at(2) = right;
		vec.at(3) = bottom;

		std::sort(vec.begin(), vec.end()); //�\�[�g

		if (vec.at(0) == left)
			return e4Direction::left;
		if (vec.at(0) == right)
			return e4Direction::right;
		if (vec.at(0) == top)
			return e4Direction::up;
		//if (vec.at(0) == down)
			return e4Direction::down;
	}

	//������W����`�̒��Ɋ܂܂�邩�ǂ����𔻒肷��
	bool RECT::JudgeContain(PT<int> pt) const
	{
		return JudgeContainAlg(pt.x, pt.y);
	}

	bool RECT::JudgeContain(int x, int y) const
	{
		return JudgeContainAlg(x, y);
	}

	//������W����`�̒��Ɋ܂܂�邩�ǂ����𔻒肷������A���S���Y��
	bool RECT::JudgeContainAlg(int x, int y) const
	{
		return (x >= left && x <= right && y >= top && y <= bottom);
	}




	//�R���X�g���N�^
	RectWH::RectWH(int left, int top, int width, int height)
	{
		this->left = left;
		this->top = top;
		this->width = width;
		this->height = height;
	}

	//�R���X�g���N�^
	//RECT�\���̂���ϊ�
	RectWH::RectWH(::RECT rect)
	{
		this->left = rect.left;
		this->top = rect.top;
		this->width = rect.right - rect.left;
		this->height = rect.bottom - rect.top;
	}
}

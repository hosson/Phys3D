//Rect�n�̃N���X
#pragma once
#include <Windows.h>
#include "PT.h"
#include "Direction.h"

namespace ho {
	//RECT�\���̂��p�����Ċ֐��Ȃǂ�ǉ������N���X
	class RECT : public ::RECT
	{
	public:
		RECT() {}
		RECT(int left, int top, int right, int bottom) { this->left = left; this->top = top; this->right = right; this->bottom = bottom; }

		e4Direction::e GetMinDirection() const; //�l�̈�ԏ�����������Ԃ�
		bool JudgeContain(PT<int> pt) const; //������W����`�̒��Ɋ܂܂�邩�ǂ����𔻒肷��
		bool JudgeContain(int x, int y) const;
		int GetWidth() const { return right - left; } //���𓾂�
		int GetHeight() const { return bottom - top; } //�����𓾂�
		void SetWidth(int Width) { this->right = this->left + Width; } //��������ݒ�
		void SetHeight(int Height) { this->bottom = this->top + Height; } //����������ݒ�
		void Set(int left, int top, int right, int bottom) { this->left = left; this->top = top; this->right = right; this->bottom = bottom; } //�ꊇ�Z�b�g
	private:
		bool JudgeContainAlg(int x, int y) const; //������W����`�̒��Ɋ܂܂�邩�ǂ����𔻒肷������A���S���Y��
	};

	//���W�ƕ��ƍ����ŕ\��RECT
	class RectWH
	{
	public:
		int left, top, width, height;

		//�R���X�g���N�^
		RectWH() { ZeroMemory(this, sizeof(RectWH)); }
		RectWH(int Left, int Top, int Width, int Height);
		RectWH(::RECT rect); //RECT�\���̂���ϊ�

		//���Z�q
		RectWH operator +(const RectWH &obj) const { return RectWH(this->left + obj.left, this->top + obj.top, this->width + obj.width, this->height + obj.height); }
		bool operator ==(const RectWH &obj) const { return this->left == obj.left && this->top == obj.top && this->width == obj.width && this->height == obj.height; }
		bool operator !=(const RectWH &obj) const { return !(*this == obj); }

		::RECT GetWinRECT(int Offset = 0) const { ::RECT rect = {left, top, left + width + Offset, top + height + Offset}; return rect; } //RECT�\���̂ŕԂ�
		RECT GethoRECT(int Offset = 0) const { RECT rect(left, top, left + width + Offset, top + height + Offset); return rect; } //RECT�\���̂ŕԂ�
	};
}

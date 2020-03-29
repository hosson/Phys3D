//STL �֘A�̋@�\
#pragma once
#include "LibError.h"

namespace ho
{
	namespace stl
	{
		//STL���X�g�̒�����C�ӂ̃C���f�b�N�X�̃C�e���[�^�𓾂�
		template <class LIST, class ITERATOR> ITERATOR GetSTLListItrFromNum(const LIST *pList, ITERATOR itr, const DWORD Num)
		{
			DWORD i = 0;
			while (itr != pList->end() && i != Num)
			{
				i++;
				itr++;
			}

			return itr;
		}
	}
}
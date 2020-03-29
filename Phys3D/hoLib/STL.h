//STL 関連の機能
#pragma once
#include "LibError.h"

namespace ho
{
	namespace stl
	{
		//STLリストの中から任意のインデックスのイテレータを得る
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
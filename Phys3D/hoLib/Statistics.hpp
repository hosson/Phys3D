//���v�Ɋւ��鏈��
#pragma once
#include <vector>
#include <algorithm>

namespace ho
{
	//0.0�`1.0�̔䗦����A�z��S�̂𒼗�ɍ��v�������̉��Ԗڂ̗v�f�Ɋ܂܂�邩�𓾂�
	template <class T> unsigned int GetElementFromRatio(std::vector<T> &vector, const double &Ratio)
	{
		T Total = 0;
		std::for_each(vector.begin(), vector.end(), [&](T v){ Total += v; }); //�z��̍��v�l���擾

		T TargetValue = Ratio * Total; //�䗦���A�z�񍇌v��1.0�Ƃ݂Ȃ����l�ɕϊ�

		Total = 0;
		for (unsigned int i = 0; i < vector.size(); i++) //�z��𑖍�
		{
			Total += vector.at(i);
			if (TargetValue <= Total)
				return i;
		}

		return vector.size();
	}
}

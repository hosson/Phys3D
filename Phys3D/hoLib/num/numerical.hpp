//�����I�ȕ֗��֐�
#pragma once

namespace ho
{
	namespace num
	{
		//�ϐ��̕�����Ԃ�
		template <typename T> int GetSign(const T &Value)
		{
			if (Value > 0)
				return 1;
			else if (Value < 0)
				return -1;

			return 0;
		}

		//�ϐ��̒l���w��͈͓��Ɏ��߂�
		template <typename T> void Clamp(T &Value, const T &Min, const T &Max)
		{
			if (Value < Min)
				Value = Min;
			else if (Value > Max)
				Value = Max;

			return;
		}

		//�ϐ��̒l���w��͈͓��Ɏ��߂�
		template <typename T> T Clamp(const T &Value, const T &Min, const T &Max)
		{
			if (Value < Min)
				return Min;
			else if (Value > Max)
				return Max;

			return Value;
		}

		//2�̕ϐ��̓��e����������
		template <typename T> void Swap(T &a, T &b)
		{
			T Buf = a;
			a = b;
			b = Buf;

			return;
		}

		//2�̕ϐ��̂����̑傫���ق��̒l�𓾂�
		template <typename T> T Max(const T &a, const T &b)
		{
			if (a > b)
				return a;
			return b;
		}

		//2�̕ϐ��̂����̏������ق��̒l�𓾂�
		template <typename T> T Min(const T &a, const T &b)
		{
			if (a < b)
				return a;
			return b;
		}

		//����lValue��Min=0.0�AMax=1.0�Ƃ����Ƃ��A�ǂ̊����ɂ��邩�𓾂�
		template <typename T> double GetRatio(const T &Value, const T &Min, const T &Max)
		{
			return double(Value - Min) / double(Max - Min);
		}

		//�C�ӂ�Min��Max�̒l�̒�����A�����i0.0�`1.0�j��Ratio�Ƃ���Ƃ��̒l�𓾂�
		template <typename T> T GetValueFromRatio(const double &Ratio, const T &Min, const T &Max)
		{
			return T(Min + (Max - Min) * Ratio);
		}
		//��L��float��
		template <typename T> T GetValueFromRatioF(const float &Ratio, const T &Min, const T &Max)
		{
			return T(Min + (Max - Min) * Ratio);
		}

	}
}
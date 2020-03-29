#include "vector.h"
#include "../Error.h"

namespace ho
{
	namespace num
	{
		//XY���ʏ��vec3d�^�ɕϊ�����
		vec3d vec2d::Getvec3d_XY(const double &z) const
		{
			return vec3d(x, y, z);
		}

		//XZ���ʏ��vec3d�^�ɕϊ�����
		vec3d vec2d::Getvec3d_XZ(const double &y) const
		{
			return vec3d(x, y, y);
		}

		
		//�����ȕ����̃x�N�g����Ԃ�
		vec2d vec2d::GetPerpendicular(const e2LR::e LR) const
		{
		switch (LR)
		{
		case e2LR::left: //�������𓾂�ꍇ
			return vec2d(-y, x);
			break;
		case e2LR::right: //�E�����𓾂�ꍇ
			return vec2d(y, -x);
			break;
		default:
			ERR(false, TEXT("���� LR �ɗL���͈͊O�̒l���n����܂����B"), __WFILE__, __LINE__); //�G���[�o��
			break;
		}

		return vec2d(0, 0); //�ꉞ0��Ԃ�
		}
		

		//2�̃x�N�g���̐����p�x�����߂�
		bool vec2d::GetAngle(double &out, const vec2d &vec1, const vec2d &vec2)
		{
			double Denominator = sqrt(vec1.x * vec1.x + vec1.y * vec1.y) * sqrt(vec2.x * vec2.x + vec2.y * vec2.y); //������Ɍv�Z����

			if (Denominator) //���ꂪ0�ȊO�̏ꍇ
			{
				double ToFunction = GetInnerProduct(vec1, vec2) / Denominator; //acos�֐��ɓ����l

				//acos�ɓ����l��1.0�`-1.0�͈̔͂Ɏ��߂�
				if (ToFunction < -1.0)
					ToFunction = -1.0 - fmod(ToFunction, 1.0);
				if (ToFunction > 1.0)
					ToFunction = 1.0 - fmod(ToFunction, 1.0);

				out = acos(ToFunction);
				return true;
			}

			return false; //�Е��܂͂������̃x�N�g���̑傫���� 0 �Ȃ̂Ŋp�x�����܂�Ȃ�����
		}

		/*
		//2�̒����̌�_�����߂�
		bool ho::vec2d::GetCrossPoint(const vec2d &vecLine1Begin, const vec2d &vecLine1End, const vec2d &vecLine2Begin, const vec2d &vecLine2End, ho::Enable<ho::vec2d> &vecCrossPos, bool &Inner)
		{
		double Height[2]; //����1����ɂ����A����2�̎n�_�ƏI�_�̍���

		Height[0] = vec2d::GetCrossProduct((vecLine1End - vecLine1Begin).GetNormalize(), vecLine2Begin - vecLine1Begin);
		Height[1] = vec2d::GetCrossProduct((vecLine1End - vecLine1Begin).GetNormalize(), vecLine2End - vecLine1Begin);

		if (Height[0] == Height[1]) //�����������ꍇ�i���s�̏ꍇ�j
		{
		if (Height[0] == 0.0) //����2������1�Əd�Ȃ��Ă���ꍇ
		{
		vecCrossPos = ho::Enable<ho::vec2d>((vecLine2Begin + vecLine2End) * 0.5); //����2�̒��S�ʒu��Ԃ�
		Inner = true;

		return true;
		} else {
		vecCrossPos = ho::Enable<ho::vec2d>();
		Inner = false;

		return false;
		}
		}


		int Sign[2] = {ho::GetSign(Height[0]), ho::GetSign(Height[1])}; //�����̕���
		if (Sign[0] * Sign[1] == 1) //�����������ꍇ
		{
		//��_���W���v�Z
		vecCrossPos = ho::Enable<ho::vec2d>(vecLine2Begin + (vecLine2End - vecLine2Begin) * (Height[0] / (Height[0] - Height[1])));

		Inner = false; //��_�͐����̊O���ɂ���
		} else {
		//��_���W���v�Z
		vecCrossPos = ho::Enable<ho::vec2d>(vecLine2Begin + (vecLine2End - vecLine2Begin) * (abs(Height[0]) / abs(Height[0] - Height[1])));

		//��_������1�̓����ɂ��邩�ǂ����𔻒�
		Inner = false;
		double DistanceLine1 = ho::vec2d::GetDistance(vecLine1Begin, vecLine1End); //����1�̎n�_�ƏI�_�̋���
		if (ho::vec2d::GetDistance(vecLine1Begin, vecCrossPos()) <= DistanceLine1 && ho::vec2d::GetDistance(vecLine1End, vecCrossPos()) <= DistanceLine1) //��_������1�͈͓̔��̏ꍇ
		{
		double DistanceLine2 = ho::vec2d::GetDistance(vecLine2Begin, vecLine2End); //����2�̎n�_�ƏI�_�̋���
		if (ho::vec2d::GetDistance(vecLine2Begin, vecCrossPos()) <= DistanceLine2 && ho::vec2d::GetDistance(vecLine2End, vecCrossPos()) <= DistanceLine2) //��_������2�͈͓̔��̏ꍇ
		{
		Inner = true; //��_�͐���1��2�͈͓̔�
		}
		}
		}

		return true;
		}
		*/



		vec3d vec3d::operator *(const Matrix &mtrx) const
		{
			//�x�N�g�����s��ɕϊ�
			Matrix mtrxVec(3, 1);
			mtrxVec(0, 0) = this->x;
			mtrxVec(1, 0) = this->y;
			mtrxVec(2, 0) = this->z;

			Matrix mtrxResult = mtrxVec * mtrx;

			return vec3d(mtrxResult(0, 0), mtrxResult(1, 0), mtrxResult(2, 0));
		}

		//2�̃x�N�g���̐����p�̊p�x��Ԃ�
		bool vec3d::GetAngle(double &out, const vec3d &vec1, const vec3d &vec2)
		{
			double Div = vec1.GetPower() * vec2.GetPower(); //���Z�Ɏg�p����l
			if (Div != 0.0) //���Z�ł���ꍇ
			{
				double ToFunction = double(vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z) / Div; //acos�֐��ɓ����l
				//acos�ɓ����l��1.0�`-1.0�͈̔͂Ɏ��߂�
				if (ToFunction < -1.0)
					ToFunction = -1.0 - fmod(ToFunction, 1.0);
				if (ToFunction > 1.0)
					ToFunction = 1.0 - fmod(ToFunction, 1.0);

				out = acos(ToFunction); //�A�[�N�R�T�C���̌��ʂ��v�Z
				return true;
			}

			return false; //�Е��܂��͗����̃x�N�g���̑傫���� 0 �̏ꍇ
		}

		//2�̃x�N�g���Ԃ̐����p�̊p�x�Ɖ�]���x�N�g���𓾂�
		bool vec3d::GetAngleAndAxis(double &Angle, vec3d &vecAxis, const vec3d &vec1, const vec3d &vec2)
		{
			if (!GetAngle(Angle, vec1, vec2)) //�p�x�����߁A���܂�Ȃ������ꍇ
				return false;

			vecAxis = GetCross(vec1, vec2); //�O�ςŉ�]���x�N�g�������߂�
			return true;
		}

		/*
		//2�̃x�N�g������Ȃ钼�����m�̈�Ԑڋ߂���2�_�����߂�
		//pSrc�ɂ͒����̒ʂ�2�_��2�̒������A���v4�_�̐擪�̃A�h���X������
		bool ho::vec3d::GetCrossPointsFromLines(std::vector<vec3d> &vecOut, vec3d pIn[4])
		{
		vecOut.resize(2);

		PLANE plnBase(pIn[0], pIn[1], pIn[0] + (pIn[3] - pIn[2])); //2�̒������܂ޕ��ʂ��쐬
		vec3d vecNormalLine = plnBase.GetNormalLine(); //���ʂ̖@���x�N�g�����擾

		int Result = 0; //�����ƕ��ʂ̌�_������������

		PLANE plnLine2(pIn[2], pIn[3], pIn[2] + vecNormalLine); //����2���̕��ʂ��쐬
		Result += plnLine2.GetCrossPointFromLine(vecOut.at(0), pIn[0], pIn[1]); //����1�ƒ���2���̕��ʂƂ̌�_���v�Z

		PLANE plnLine1(pIn[0], pIn[1], pIn[0] + vecNormalLine); //����1���̕��ʂ��쐬
		Result += plnLine1.GetCrossPointFromLine(vecOut.at(1), pIn[2], pIn[3]); //����2�ƒ���1���̕��ʂƂ̌�_���v�Z

		if (Result < 2) //�����񐔂�2��ȉ��������ꍇ
		return false; //���܂�Ȃ�����

		return true;
		}
		*/
	}
}


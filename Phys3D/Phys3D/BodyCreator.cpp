#include "Body.h"
#include "BodyCreator.h"
#include "../hoLib/Error.h"
#include "Mesh.h"

namespace Phys3D
{
	//���̂𐶐�����
	std::shared_ptr<Body> BodyCreator::CreateBody(vec3d vecPos, const vec3d &vecLinearVelocity, const Quaternion &qtAngle, const vec3d &vecOmega, std::vector<MASSPOINT> vectorMassPoint, std::vector<vec3d> vectorvecVertex, const std::vector<int> &vectorVtFaceIndex, const double &e, const double &mu, const bool &Fixed)
	{
		//���d�ʂ��v�Z
		double Mass = 0;
		if (Fixed) //�Œ蕨�̏ꍇ
		{
			Mass = DBL_MAX;
		} else {
			for (std::vector<MASSPOINT>::iterator itr = vectorMassPoint.begin(); itr != vectorMassPoint.end(); itr++)
				Mass += itr->Mass;
		}

		//�d�S���v�Z
		vec3d vecLocalCenter(0, 0, 0); //���[�J�����W�ł̏d�S
		for (std::vector<MASSPOINT>::const_iterator itr = vectorMassPoint.begin(); itr != vectorMassPoint.end(); itr++)
			vecLocalCenter += itr->vecPos * (itr->Mass / Mass);
		vecPos += vecLocalCenter; //���̂̍��W���d�S�ʒu�ɕύX

		//���_�̈ʒu���d�S���猩���ʒu�ɕύX
		for (std::vector<MASSPOINT>::iterator itr = vectorMassPoint.begin(); itr != vectorMassPoint.end(); itr++)
			itr->vecPos = itr->vecPos - vecLocalCenter;

		//���_�����d�S���猩�����W�ɕύX
		for (std::vector<vec3d>::iterator itr = vectorvecVertex.begin(); itr != vectorvecVertex.end(); itr++)
			*itr -= vecLocalCenter;

		std::vector<int> vectorVtEdgeIndex = GetvectorVtEdgeIndex(vectorVtFaceIndex); //�ӂ̒��_�C���f�b�N�X�𒊏o

		std::shared_ptr<Mesh> spMeshObj = ho::sp_init<Mesh>(new Mesh(vectorvecVertex, vectorVtEdgeIndex, vectorVtFaceIndex)); //���b�V�����쐬

		Matrix mtrxInertia = GetmtrxInertia(vectorMassPoint, Fixed); //���_�f�[�^���犵�����[�����g�e���\�����v�Z����

		vec3d vecL = vecOmega * mtrxInertia; //�p���x�� �~ �������[�����g�e���\��I �ɂ���� �p�^����L �����߂�

		return std::shared_ptr<Body>(new Body(vecPos, vecLinearVelocity, qtAngle, vecL, Mass, mtrxInertia, spMeshObj, e, mu, Fixed));
	}

	//���̂𐶐�����
	std::shared_ptr<Body> BodyCreator::CreateBody(vec3d vecPos, const vec3d &vecLinearVelocity, const Quaternion &qtAngle, const vec3d &vecOmega, const double &Density, std::vector<vec3d> vectorvecVertex, const std::vector<int> &vectorVtFaceIndex, const double &e, const double &mu, const bool &Fixed)
	{
		//���d�ʂ��v�Z
		double Mass = 0;
		double TotalVolume = 0;
		if (Fixed) //�Œ蕨�̏ꍇ
		{
			Mass = DBL_MAX;
		} else {
			for (std::vector<int>::const_iterator itr = vectorVtFaceIndex.begin(); itr != vectorVtFaceIndex.end(); itr += 3) //�S�Ă̖ʂ𑖍�
				TotalVolume += abs(vec3d::GetDot(vectorvecVertex.at(*itr), vec3d::GetCross(vectorvecVertex.at(*(itr + 2)), vectorvecVertex.at(*(itr + 1)))) / 6.0); //�l�ʑ̂̑̐ς����Z
			Mass = TotalVolume * Density; //���� = �̐� * ���x
		}

		//�d�S���v�Z
		vec3d vecLocalCenter(0, 0, 0); //���[�J�����W�ł̏d�S
		for (std::vector<int>::const_iterator itr = vectorVtFaceIndex.begin(); itr != vectorVtFaceIndex.end(); itr += 3) //�S�Ă̖ʂ𑖍�
		{
			double Volume = abs(vec3d::GetDot(vectorvecVertex.at(*itr), vec3d::GetCross(vectorvecVertex.at(*(itr + 2)), vectorvecVertex.at(*(itr + 1)))) / 6.0); //�l�ʑ̂̑̐�
			vec3d vecCentroid = (vectorvecVertex.at(*itr) + vectorvecVertex.at(*(itr + 1)) + vectorvecVertex.at(*(itr + 2))) * 0.25; //�l�ʑ̂̏d�S
			vecLocalCenter += vecCentroid * (Volume / TotalVolume);
		}
		vecPos += vecLocalCenter; //���̂̍��W���d�S�ʒu�ɕύX

		//���_�����d�S���猩�����W�ɕύX
		for (std::vector<vec3d>::iterator itr = vectorvecVertex.begin(); itr != vectorvecVertex.end(); itr++)
			*itr -= vecLocalCenter;

		std::vector<int> vectorVtEdgeIndex = GetvectorVtEdgeIndex(vectorVtFaceIndex); //�ӂ̒��_�C���f�b�N�X�𒊏o

		std::shared_ptr<Mesh> spMeshObj = ho::sp_init<Mesh>(new Mesh(vectorvecVertex, vectorVtEdgeIndex, vectorVtFaceIndex)); //���b�V�����쐬

		Matrix mtrxInertia = GetmtrxInertia(Density, vectorvecVertex, vectorVtFaceIndex, Fixed); //���x�ƃ��b�V���`�󂩂犵�����[�����g�e���\�����v�Z����

		vec3d vecL = vecOmega * mtrxInertia; //�p���x�� �~ �������[�����g�e���\��I �ɂ���� �p�^����L �����߂�

		return std::shared_ptr<Body>(new Body(vecPos, vecLinearVelocity, qtAngle, vecL, Mass, mtrxInertia, spMeshObj, e, mu, Fixed));
	}

	//�ʂ̒��_�C���f�b�N�X����ӂ̒��_�C���f�b�N�X�𒊏o
	std::vector<int> BodyCreator::GetvectorVtEdgeIndex(const std::vector<int> &vectorVtFaceIndex)
	{
		std::vector<int> vectorVtEdgeIndex; //�ӂ̒��_�C���f�b�N�X

		//���ݑ��݂���ӂ̒��_�C���f�b�N�X�̒��ɓ������̂����邩�ǂ����`�F�b�N
		auto fCheckVtEdgeIndexOverlap = [&](int Index1, int Index2)->bool
		{
			for (unsigned int i = 0; i < vectorVtEdgeIndex.size(); i += 2)
				if ((Index1 == vectorVtEdgeIndex.at(i) && Index2 == vectorVtEdgeIndex.at(i + 1)) || (Index1 == vectorVtEdgeIndex.at(i + 1) && Index2 == vectorVtEdgeIndex.at(i)))
					return true; //�d�����Ă����ꍇ

			return false; //�d�����Ă��Ȃ��ꍇ
		};


		//�ʂ̒��_�C���f�b�N�X����ӂ̒��_�C���f�b�N�X�𒊏o
		for (unsigned int i = 0; i < vectorVtFaceIndex.size(); i += 3)
		{
			if (!fCheckVtEdgeIndexOverlap(vectorVtFaceIndex.at(i), vectorVtFaceIndex.at(i + 1))) //�d�����Ă��Ȃ��ꍇ
			{
				vectorVtEdgeIndex.push_back(vectorVtFaceIndex.at(i));
				vectorVtEdgeIndex.push_back(vectorVtFaceIndex.at(i + 1));
			}
			if (!fCheckVtEdgeIndexOverlap(vectorVtFaceIndex.at(i + 1), vectorVtFaceIndex.at(i + 2))) //�d�����Ă��Ȃ��ꍇ
			{
				vectorVtEdgeIndex.push_back(vectorVtFaceIndex.at(i + 1));
				vectorVtEdgeIndex.push_back(vectorVtFaceIndex.at(i + 2));
			}
			if (!fCheckVtEdgeIndexOverlap(vectorVtFaceIndex.at(i + 2), vectorVtFaceIndex.at(i))) //�d�����Ă��Ȃ��ꍇ
			{
				vectorVtEdgeIndex.push_back(vectorVtFaceIndex.at(i + 2));
				vectorVtEdgeIndex.push_back(vectorVtFaceIndex.at(i));
			}
		}

		return vectorVtEdgeIndex;
	}

	//���_�f�[�^���犵�����[�����g�e���\�����v�Z����
	Matrix BodyCreator::GetmtrxInertia(const std::vector<MASSPOINT> &vectorMassPoint, const bool &Fixed)
	{
		Matrix mtrx;

		mtrx.Resize(3, 3); //�s���3�~3�̃T�C�Y�ɕύX
		mtrx.InitZero(); //�S�Ă̗v�f��0�ŏ�����

		if (Fixed) //�Œ蕨�̂̏ꍇ�i���ʂ͖�����Ƃ���j
		{
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 3; j++)
					mtrx(i, j) = DBL_MAX;
		} else {
			for (std::vector<MASSPOINT>::const_iterator itr = vectorMassPoint.begin(); itr != vectorMassPoint.end(); itr++) //���_�𑖍�
			{
				mtrx(0, 0) += itr->Mass * (itr->vecPos.y * itr->vecPos.y + itr->vecPos.z * itr->vecPos.z);
				mtrx(1, 0) -= itr->Mass * itr->vecPos.y * itr->vecPos.x;
				mtrx(2, 0) -= itr->Mass * itr->vecPos.z * itr->vecPos.x;
				mtrx(0, 1) -= itr->Mass * itr->vecPos.x * itr->vecPos.y;
				mtrx(1, 1) += itr->Mass * (itr->vecPos.x * itr->vecPos.x + itr->vecPos.z * itr->vecPos.z);
				mtrx(2, 1) -= itr->Mass * itr->vecPos.z * itr->vecPos.y;
				mtrx(0, 2) -= itr->Mass * itr->vecPos.x * itr->vecPos.z;
				mtrx(1, 2) -= itr->Mass * itr->vecPos.y * itr->vecPos.z;
				mtrx(2, 2) += itr->Mass * (itr->vecPos.x * itr->vecPos.x + itr->vecPos.y * itr->vecPos.y);
			}
		}

		return mtrx;
	}

	//���x�ƃ��b�V���`�󂩂犵�����[�����g�e���\�����v�Z����
	Matrix BodyCreator::GetmtrxInertia(const double &Density, const std::vector<vec3d> &vectorvecVertex, const std::vector<int> &vectorVtFaceIndex, const bool &Fixed)
	{
		Matrix mtrx;

		mtrx.Resize(3, 3); //�s���3�~3�̃T�C�Y�ɕύX
		mtrx.InitZero(); //�S�Ă̗v�f��0�ŏ�����

		if (Fixed) //�Œ蕨�̂̏ꍇ�i���ʂ͖�����Ƃ���j
		{
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 3; j++)
					mtrx(i, j) = DBL_MAX;
		} else {
			for (std::vector<int>::const_iterator itr = vectorVtFaceIndex.begin(); itr != vectorVtFaceIndex.end(); itr += 3) //�S�Ă̖ʂ𑖍�
			{
				Matrix mtrxFace(3, 3); //��1�ɑΉ�����l�ʑ̂̊����e���\��
				mtrxFace.InitZero();

				const vec3d v1 = vectorvecVertex.at(*itr);
				const vec3d v2 = vectorvecVertex.at(*(itr + 1));
				const vec3d v3 = vectorvecVertex.at(*(itr + 2));

				const double Volume = abs(vec3d::GetDot(v1, vec3d::GetCross(v3, v2)) / 6.0);
				const double Mass = Volume * Density;

				mtrxFace(0, 0) = Mass * (v1.y * v1.y + v1.y * v2.y + v2.y * v2.y + v2.y * v3.y + v3.y * v3.y + v3.y * v1.y + v1.z * v1.z + v1.z * v2.z + v2.z * v2.z + v2.z * v3.z + v3.z * v3.z* +v3.z * v1.z) / 10.0;
				mtrxFace(1, 0) = Mass * (-2.0 * v1.x * v1.y - v1.x * v2.y - v1.x * v3.y - v2.x * v1.y - 2.0 * v2.x * v2.y - v2.x * v3.y - v3.x * v1.y - v3.x * v2.y - 2.0 * v3.x * v3.y) / 20.0;
				mtrxFace(2, 0) = Mass * (-2.0 * v1.x * v1.z - v1.x * v2.z - v1.x * v3.z - v2.x * v1.z - 2.0 * v2.x * v2.z - v2.x * v3.z - v3.x * v1.z - v3.x * v2.z - 2.0 * v3.x * v3.z) / 20.0;
				mtrxFace(0, 1) = Mass * (-2.0 * v1.x * v1.y - v1.x * v2.y - v1.x * v3.y - v2.x * v1.y - 2.0 * v2.x * v2.y - v2.x * v3.y - v3.x * v1.y - v3.x * v2.y - 2.0 * v3.x * v3.y) / 20.0;
				mtrxFace(1, 1) = Mass * (v1.z * v1.z + v1.z * v2.z + v2.z * v2.z + v2.z * v3.z + v3.z * v3.z + v3.z * v1.z + v1.x * v1.x + v1.x * v2.x + v2.x * v2.x + v2.x * v3.x + v3.x * v3.x + v3.x * v1.x) / 10.0;
				mtrxFace(2, 1) = Mass * (-2.0 * v1.y * v1.z - v1.y * v2.z - v1.y * v3.z - v2.y * v1.z - 2.0 * v2.y * v2.z - v2.y * v3.z - v3.y * v1.z - v3.y * v2.z - 2.0 * v3.y * v3.z) / 20.0;
				mtrxFace(0, 2) = Mass * (-2.0 * v1.x * v1.z - v1.x * v2.z - v1.x * v3.z - v2.x * v1.z - 2.0 * v2.x * v2.z - v2.x * v3.z - v3.x * v1.z - v3.x * v2.z - 2.0 * v3.x * v3.z) / 20.0;
				mtrxFace(1, 2) = Mass * (-2.0 * v1.y * v1.z - v1.z * v2.z - v1.y * v3.z - v2.y * v1.z - 2.0 * v2.y * v2.z - v2.y * v3.z - v3.y * v1.z - v3.y * v2.z - 2.0 * v3.y * v3.z) / 20.0;
				mtrxFace(2, 2) = Mass * (v1.x * v1.x + v1.x * v2.x + v2.x * v2.x + v2.x * v3.x + v3.x * v3.x + v1.x * v3.x + v1.y * v1.y + v1.y * v2.y + v2.y * v2.y + v2.y * v3.y + v3.y * v3.y + v3.y * v1.y) / 10.0;

				mtrx = mtrx + mtrxFace;
			}
		}

		return mtrx;
	}
}
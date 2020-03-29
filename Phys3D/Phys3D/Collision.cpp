#include "Collision.h"
#include "../hoLib/Common.h"
#include "Body.h"
#include "Contact.h"
#include "../hoLib/hoLib.h"
#include "../hoLib/Debug.h"
#include "Mesh.h"
#include "../hoLib/num/Quaternion.h"
#include "../hoLib/num/constant.h"
#include "../hoLib/num/numerical.hpp"

using namespace ho::num;

namespace Phys3D
{
	//�R���X�g���N�^
	Collision::Collision()
	{
	}

	//�f�X�g���N�^
	Collision::~Collision()
	{
	}

	//�S�Ă̕��̂���ڐG�����擾
	void Collision::DetectContact(std::list<CONTACT> &listContact, const std::list<std::shared_ptr<Body>> &listspBodyObj)
	{
		std::list<PAIR> listPair; //�y�A���X�g
		GetPair(listPair, listspBodyObj); //�ڐG�\���̂���y�A���擾����

		for (std::list<PAIR>::iterator itr = listPair.begin(); itr != listPair.end(); itr++) //�S�Ẵy�A�𑖍�
		{
			CONTACT Contact(*itr); //�y�A���Z�b�g���ĐڐG���𐶐�

			if (CollisionJudge(Contact)) //���̂ǂ����̏Փ˔�����s���A�ڐG���ĐڐG��񂪎擾���ꂽ�ꍇ
				listContact.push_back(Contact);
		}

		return;
	}

	//�ڐG�\���̂���y�A���擾����
	void Collision::GetPair(std::list<PAIR> &listPair, const std::list<std::shared_ptr<Body>> &listspBodyObj)
	{
		std::list<std::shared_ptr<Body>>::const_iterator itr2;
		for (std::list<std::shared_ptr<Body>>::const_iterator itr1 = listspBodyObj.begin(); itr1 != listspBodyObj.end(); itr1++)
		{
			itr2 = itr1;
			itr2++;

			for (; itr2 != listspBodyObj.end(); itr2++)
				if (!(*itr1)->getFixed() || !(*itr2)->getFixed()) //�ǂ��炩���Œ蕨�łȂ��ꍇ
					if (((*itr2)->getvecPos() - (*itr1)->getvecPos()).GetPower() <= (*itr1)->getBoundingRadius() + (*itr2)->getBoundingRadius()) //���a���m�̔���ŏՓˉ\��������ꍇ
						listPair.push_back(PAIR(*itr1, *itr2));
		}

		return;
	}

	//���̓��m�̏Փ˔���
	bool Collision::CollisionJudge(CONTACT &Contact)
	{
		std::vector<std::shared_ptr<Mesh>> vectorspMeshObj(2); //�߂荞�݃��b�V��

		//���b�V���̃��[���h���_���W���X�V
		for (int i = 0; i < 2; i++)
			Contact.Pair.wpBodyObj[i].lock()->UpdateMeshVertexWorldPos();

		if (!GetSinkMesh(vectorspMeshObj, Contact.Pair.wpBodyObj[0].lock()->getspMeshObj(), Contact.Pair.wpBodyObj[1].lock()->getspMeshObj())) //2�̃��b�V�����Փ˔��肵�Փ˕����̃��b�V���𓾂āA�����Ȃ������ꍇ
			return false;

		//�߂荞�݃��b�V������ڐG�����v�Z
		Contact.vecN = CalcvecNFromSinkMesh(vectorspMeshObj); //�Փ˃��b�V�����ڐG�ʂ̖@���x�N�g�����v�Z
		Contact.vecWorldPos = CalcHitPos(vectorspMeshObj);//�Փ˃��b�V�����ڐG�ʒu���v�Z
		Contact.SinkDistance = CalcSinkDistance(vectorspMeshObj, Contact.vecWorldPos, Contact.vecN); //�ڐG���b�V���ƐڐG�ʒu�ƐڐG�ʂ̖@���x�N�g�����߂荞�݋������v�Z
		//Contact.vectorvecSubPos.clear(); //�ڐG���p�����Ă������̂��߂ɁA�z����N���A
		//GetSubPos(Contact.vectorvecSubPos, vectorspMeshObj, Contact.vecWorldPos, Contact.vecN); //�T�u�\���o�[�p�̐ڐG�ʒu���擾

		return true;
	}

	//2�̃��b�V�����Փ˔��肵�Փ˕����̃��b�V���𓾂�
	bool Collision::GetSinkMesh(std::vector<std::shared_ptr<Mesh>> &vectorspMeshObj, const std::weak_ptr<Mesh> &wpMeshObj1, const std::weak_ptr<Mesh> &wpMeshObj2)
	{	
		std::shared_ptr<Mesh> spMeshObj[2] = { wpMeshObj1.lock(), wpMeshObj2.lock() };

		std::vector<std::list<vec3d>> vectorlistvecCrossPoint[2]; //�����̃��b�V���̖ʂ��Ƃ̌�_
		vectorlistvecCrossPoint[0].resize(spMeshObj[0]->getvectorspFaceObj().size());
		vectorlistvecCrossPoint[1].resize(spMeshObj[1]->getvectorspFaceObj().size());

		bool Collision1 = false, Collision2 = false; //�ڐG�����������ǂ���
		std::vector<double> vectorVtHeight; //�������̃��b�V���̒��_�́A���胁�b�V���̖ʂɑ΂��鍂��
		for (int i = 0; i < 2; i++) //���b�V���𑖍�
		{
			//�������̃��b�V���̂��ׂĂ̒��_�́A���胁�b�V���̖ʂɑ΂��鍂�����v�Z����
			vectorVtHeight.resize(spMeshObj[i]->getvectorspVertexObj().size() * spMeshObj[1 - i]->getvectorspFaceObj().size()); //���_���~�ʂ̐�
			for (unsigned int j = 0; j < spMeshObj[i]->getvectorspVertexObj().size(); j++) //���_�𑖍�
				for (unsigned int k = 0; k < spMeshObj[1 - i]->getvectorspFaceObj().size(); k++) //�ʂ𑖍�
					vectorVtHeight.at(j * spMeshObj[1 - i]->getvectorspFaceObj().size() + k) = vec3d::GetDot(spMeshObj[i]->getvectorspVertexObj().at(j)->getvecWorldPos() - spMeshObj[1 - i]->getvectorspFaceObj().at(k)->getvectorwpVertexObj().at(0).lock()->getvecWorldPos(), spMeshObj[1 - i]->getvectorspFaceObj().at(k)->getvecWorldNormal());

			//���_������̃��b�V���̓����ɂ��邩�ǂ��������o
			for (unsigned int j = 0; j < spMeshObj[i]->getvectorspVertexObj(). size(); j++) //���_�𑖍�
			{
				bool VtInner = true; //���_���ʂ̓����ɂ��邩�ǂ���
				for (unsigned int k = 0; k < spMeshObj[1 - i]->getvectorspFaceObj().size(); k++) //�ʂ𑖍�
				{
					if (vectorVtHeight.at(j * spMeshObj[1 - i]->getvectorspFaceObj().size() + k) > 0) //�ʂ̊O���������ꍇ
					{
						VtInner = false;
						break;
					}
				}
				if (VtInner) //�����������ꍇ
				{
					Collision1 = true;
					for (std::vector<std::weak_ptr<Face>>::iterator itr = spMeshObj[i]->getvectorspVertexObj().at(j)->getvectorwpFaceObj().begin(); itr != spMeshObj[i]->getvectorspVertexObj().at(j)->getvectorwpFaceObj().end(); itr++) //���_�����݂���ʂ𑖍�
						vectorlistvecCrossPoint[i].at((*itr).lock()->getID()).push_back(spMeshObj[i]->getvectorspVertexObj().at(j)->getvecWorldPos()); //�ʂɌ�_�Ƃ��Ēǉ�����
				}
			}

			//���������b�V���̕ӂƑ��胁�b�V���̖ʂ𔻒�
			for (unsigned int j = 0; j < spMeshObj[i]->getvectorspEdgeObj().size(); j++) //�ӂ𑖍�
			{
				int ID[2] = { spMeshObj[i]->getvectorspEdgeObj().at(j)->getwpVertexObj1().lock()->getID(), spMeshObj[i]->getvectorspEdgeObj().at(j)->getwpVertexObj2().lock()->getID() }; //�ӂɐڑ����ꂽ2�̒��_��ID���擾

				for (unsigned int k = 0; k < spMeshObj[1 - i]->getvectorspFaceObj().size(); k++) //�ʂ𑖍�
				{
					//��_�����߂�
					vec3d vecCrossPoint; //��_
					double Height[2] = { vectorVtHeight.at(ID[0] * spMeshObj[1 - i]->getvectorspFaceObj().size() + k), vectorVtHeight.at(ID[1] * spMeshObj[1 - i]->getvectorspFaceObj().size() + k) }; //�ӂ�2�̒��_�̍���
					if ((Height[0] > 0 && Height[1] > 0) || (Height[0] < 0 && Height[1] < 0)) //���_�����ʂ̕Б��Ɋ���Ă���ꍇ
						continue;
					if (Height[0] == 0 && Height[1] == 0) //�����̒��_���ʂ̓����ɂ���ꍇ
						vecCrossPoint = (spMeshObj[i]->getvectorspEdgeObj().at(j)->getwpVertexObj1().lock()->getvecWorldPos() + spMeshObj[i]->getvectorspEdgeObj().at(j)->getwpVertexObj2().lock()->getvecWorldPos()) * 0.5;
					else
						vecCrossPoint = spMeshObj[i]->getvectorspEdgeObj().at(j)->getwpVertexObj1().lock()->getvecWorldPos() + (spMeshObj[i]->getvectorspEdgeObj().at(j)->getwpVertexObj2().lock()->getvecWorldPos() - spMeshObj[i]->getvectorspEdgeObj().at(j)->getwpVertexObj1().lock()->getvecWorldPos()) * (abs(Height[0]) / (abs(Height[0]) + abs(Height[1])));

					//��_���ʂ̓����ɂ��邩�ǂ����𒲂ׂ�
					bool Inner = true;
					int lNext;
					for (int l = 0; l < 3; l++) //�ʂ̕ӂ𑖍�
					{
						lNext = (l + 1) % 3;
						if (0 < vec3d::GetDot(vec3d::GetCross(vecCrossPoint - spMeshObj[1 - i]->getvectorspFaceObj().at(k)->getvectorwpVertexObj().at(l).lock()->getvecWorldPos(), spMeshObj[1 - i]->getvectorspFaceObj().at(k)->getvectorwpVertexObj().at(lNext).lock()->getvecWorldPos() - spMeshObj[1 - i]->getvectorspFaceObj().at(k)->getvectorwpVertexObj().at(l).lock()->getvecWorldPos()), spMeshObj[1 - i]->getvectorspFaceObj().at(k)->getvecWorldNormal())) //�ʂ̕ӂ̊O���ɂ������ꍇ
						{
							Inner = false;
							break;
						}
					}

					if (Inner) //��_���ʂ̓����������ꍇ
					{
						Collision2 = true;
						vectorlistvecCrossPoint[1 - i].at(k).push_back(vecCrossPoint); //����̖ʂɌ�_��ǉ�
						for (std::vector<std::weak_ptr<Face>>::const_iterator itr = spMeshObj[i]->getvectorspEdgeObj().at(j)->getvectorwpFaceObj().begin(); itr != spMeshObj[i]->getvectorspEdgeObj().at(j)->getvectorwpFaceObj().end(); itr++) //�ӂ��܂܂��ʂ𑖍�
							vectorlistvecCrossPoint[i].at((*itr).lock()->getID()).push_back(vecCrossPoint); //�����̃��b�V���̖ʂɌ�_��ǉ�
					}
				}
			}
		}

		if (!Collision2) //�ڐG���Ă��Ȃ������ꍇ
			return false; //Collision1�݂̂�true�̏ꍇ�́A���ĂȂ��}�`�̔��Α��ɒ��_���߂荞��ł���݂̂Ȃ̂ŁA�ڐG���Ă���Ƃ͌����Ȃ�

		//��_�������ɂ߂荞�݃��b�V�����\������ʂ��擾����
		for (int i = 0; i < 2; i++) //���b�V���𑖍�
		{
			vectorspMeshObj[i] = ho::sp_init<Mesh>(new Mesh()); //�߂荞�݃��b�V���I�u�W�F�N�g�𐶐�

			for (unsigned int j = 0; j < spMeshObj[i]->getvectorspFaceObj().size(); j++) //�ʂ𑖍�
				GetSinkMeshFace(vectorspMeshObj[i], spMeshObj[i]->getvectorspFaceObj().at(j), vectorlistvecCrossPoint[i].at(j)); //��_����ʂ��擾
		}

		return true;
	}

	//�Փː����ŕ������A�߂荞�ݕ����̃��b�V�����\������ʂ𓾂�
	void Collision::GetSinkMeshFace(const std::shared_ptr<Mesh> &spMeshObj, const std::shared_ptr<Face> &spFaceObj, std::list<vec3d> &listvecCrossPoint)
	//void Collision::GetSinkMeshFace(std::list<Face *> *plistpFaceObj, Face *pFaceObj, std::list<vec3d> *plistvecCrossPoint)
	{
		if (listvecCrossPoint.size() < 3) //�Փˌ�_��3�����̏ꍇ
			return; //�O�p�`�����Ȃ��̂ł����ŏI��

		//���d�����Ă�����W���J�b�g���A�c������_��3�ɖ����Ȃ��ꍇ��return����i�������j
		CreateOutLine(listvecCrossPoint, spFaceObj->getvecWorldNormal()); //���_���Q�̊O���̕ӂ��쐬���Đڑ����đ��p�`�����
		DividePolygonIntoTriangles(spMeshObj, listvecCrossPoint); //���p�`�𕡐��̎O�p�`�̖ʂɕ���
		
		return;
	}

	//���_���X�g����O���̕ӂ��\�����钸�_�������c���A�c����폜����
	void Collision::CreateOutLine(std::list<vec3d> &listvecCrossPoint, const vec3d &vecFaceN)
	{
		//�O�����z����悤�ɕ��ёւ���
		for (std::list<vec3d>::iterator itr1 = listvecCrossPoint.begin(); itr1 != listvecCrossPoint.end();)
		{
			std::list<vec3d>::iterator itr2 = itr1;
			itr2++;

			bool Find = false; //itr1����O���ɑ������̒��_�������������ǂ���
			while (itr2 != listvecCrossPoint.end())
			{
				std::list<vec3d>::iterator itr3 = listvecCrossPoint.begin();

				bool Result = true;
				vec3d vec1To2 = (*itr2 - *itr1).GetNormalize(); //itr1��itr2 �̒��_�֌����������x�N�g��

				while (itr3 != listvecCrossPoint.end())
				{
					if (itr3 != itr2 && itr3 != itr1)
					{
						if (vec3d::GetDot(vec3d::GetCross(*itr3 - *itr1, vec1To2), vecFaceN) > 0) //itr1�`itr2�̒�������itr3���O���ɂ���ꍇ
						{
							Result = false;
							break;
						}
					}

					itr3++;
				}

				if (Result) //itr2��itr1����O���ɑ������̒��_�������ꍇ
				{
					Find = true;

					std::list<vec3d>::iterator itr = itr1;
					itr++;
					listvecCrossPoint.insert(itr, *itr2);
					listvecCrossPoint.erase(itr2);
					break;
				}

				itr2++;
			}

			if (!Find) //�O���ɑ������_��������Ȃ������ꍇ
			{
				if (itr1 == listvecCrossPoint.begin()) //���ёւ���ꂽ���_�����݂��Ȃ��ꍇ
				{
					itr1 = listvecCrossPoint.erase(itr1);
				} else {
					//���̌�͊O���̃��C���Ɋ܂܂�Ȃ����_�Ȃ̂ō폜
					itr1++;
					for (std::list<vec3d>::iterator itr = itr1; itr != listvecCrossPoint.end();)
						itr = listvecCrossPoint.erase(itr);
					break; //���̌�͕��ёւ���ꂽ�ŏ��̒��_�ɂȂ��邵���Ȃ��̂ŏI���
				}
			} else {
				itr1++;
			}
		}
		
		return;
	}

	//���p�`�𕡐��̎O�p�`�̖ʂɕ���
	void Collision::DividePolygonIntoTriangles(const std::shared_ptr<Mesh> &spMeshObj, const std::list<vec3d> &listvecCrossPoint)
	//void Collision::DividePolygonIntoTriangles(std::list<Face *> *plistpFaceObj, std::list<vec3d> *plistvecCrossPoint)
	{
		std::vector<vec3d> vectorvec(3);

		std::list<vec3d>::const_iterator itrBegin, itr1, itr2;
		itrBegin = listvecCrossPoint.begin();
		itr1 = itrBegin;
		itr1++;
		itr2 = itr1;
		itr2++;

		int ID = 0;
		while (itr2 != listvecCrossPoint.end())
		{
			vectorvec.at(0) = *itrBegin;
			vectorvec.at(1) = *itr1;
			vectorvec.at(2) = *itr2;
			spMeshObj->AddFace(vectorvec); //�߂荞�݃��b�V���ɎO�p�`�̖ʂ�ǉ�

			itr1 = itr2;
			itr2++;
		}
		
		return;
	}

	//�ڐG���b�V�����@���x�N�g�����v�Z����
	vec3d Collision::CalcvecNFromSinkMesh(std::vector<std::shared_ptr<Mesh>> &vectorspMeshObj)
	{
		vec3d vecMeshN[2] = { vec3d(0, 0, 0), vec3d(0, 0, 0) }; //���ꂼ��̃��b�V���̖ʂō\�������@���x�N�g��

		for (int i = 0; i < 2; i++) //���̂𑖍�
		{
			for (std::vector<std::shared_ptr<Face>>::const_iterator itr = vectorspMeshObj.at(i)->getvectorspFaceObj().begin(); itr != vectorspMeshObj.at(i)->getvectorspFaceObj().end(); itr++) //�ڐG���b�V���̖ʂ𑖍�
				vecMeshN[i] += (*itr)->getvecLocalNormal() * (*itr)->CalcArea(); //�ʂ̖@���x�N�g���ɖʂ̖ʐς��d�݂Ƃ��Ċ|�������̂����Z����
			vecMeshN[i] = vecMeshN[i].GetNormalize(); //���K��
		}

		return (-vecMeshN[0] + vecMeshN[1]).GetNormalize();
	}

	//�ڐG���b�V�����ڐG�ʒu���v�Z����
	vec3d Collision::CalcHitPos(std::vector<std::shared_ptr<Mesh>> &vectorspMeshObj)
	{
		//���b�V���̍��W�̕��ϒl�̒��S�����߂�
		int SumNum = 0;
		vec3d vecAverageCenter(0, 0, 0); //���b�V���̒��_���W�̕��ψʒu
		for (int i = 0; i < 2; i++) //���b�V���̕��̂𑖍�
		{
			for (std::vector<std::shared_ptr<Face>>::const_iterator itr = vectorspMeshObj.at(i)->getvectorspFaceObj().begin(); itr != vectorspMeshObj.at(i)->getvectorspFaceObj().end(); itr++)
			{
				vecAverageCenter += (*itr)->getvectorwpVertexObj().at(0).lock()->getvecLocalPos();
				SumNum++;
			}
		}
		vecAverageCenter /= SumNum;

		vec3d vecCentroid(0, 0, 0); //�d�S
		double TotalVolume = 0; //�̐ς̍��v
		double Vol;
		vec3d Cen;
		for (int i = 0; i < 2; i++) //���b�V���̕��̂𑖍�
		{
			for (std::vector<std::shared_ptr<Face>>::const_iterator itr = vectorspMeshObj.at(i)->getvectorspFaceObj().begin(); itr != vectorspMeshObj.at(i)->getvectorspFaceObj().end(); itr++)
			{
				Vol = (*itr)->CalcVolume(vecAverageCenter); //�ʂƒ��S���W����̐ς��v�Z
				Cen = (*itr)->CalcCentroid(vecAverageCenter); //�ʂƒ��S���W����d�S���v�Z
				vecCentroid += Cen * Vol;
				TotalVolume += Vol;
			}
		}

		if (TotalVolume == 0) //�̐ς̍��v��0�̏ꍇ
		{
			//�O�p�`�̖ʐςŏd�S�����߂�
			double TotalArea = 0; //�ʐς̍��v
			double Area;
			vecCentroid = vec3d(0, 0, 0);
			for (int i = 0; i < 2; i++) //���b�V���̕��̂𑖍�
			{
				for (std::vector<std::shared_ptr<Face>>::const_iterator itr = vectorspMeshObj.at(i)->getvectorspFaceObj().begin(); itr != vectorspMeshObj.at(i)->getvectorspFaceObj().end(); itr++)
				{
					Area = (*itr)->CalcArea(); //�ʂ̖ʐς��v�Z
					Cen = (*itr)->CalcCentroid(vecAverageCenter); //�ʂƒ��S���W����d�S���v�Z
					vecCentroid += Cen * Area;
					TotalArea += Area;
				}
			}
			if (TotalArea != 0)
				vecCentroid /= TotalArea;
		} else {
			vecCentroid /= TotalVolume;
		}

		return vecCentroid;
	}

	//�ڐG���b�V���ƏՓˈʒu�ƏՓ˖@�����A�߂荞�݋������v�Z����
	double Collision::CalcSinkDistance(const std::vector<std::shared_ptr<Mesh>> &vectorspMeshObj, const vec3d &vecHitPos, const vec3d &vecN)
	{
		double TotalDistance = 0, Distance;
		for (int i = 0; i < 2; i++) //���b�V���̕��̂𑖍�
			for (std::vector<std::shared_ptr<Face>>::const_iterator itr = vectorspMeshObj.at(i)->getvectorspFaceObj().begin(); itr != vectorspMeshObj.at(i)->getvectorspFaceObj().end(); itr++)
				if ((*itr)->CalcDistance(Distance, vecHitPos, vecN)) //�ڐG�ʒu�ƐڐG�@������A�ʂƂ̏Փ˔�������A�Փ˂��Ă����ꍇ
					TotalDistance += abs(Distance); //���������Z

		return TotalDistance;
	}

	//�T�u�\���o�[�p�̐ڐG�ʒu���擾
	void Collision::GetSubPos(std::vector<vec3d> &vectorvecSubPos, const std::vector<std::shared_ptr<Mesh>> &vectorspMeshObj, const vec3d &vecHitPos, const vec3d &vecN)
	{
		const int Num = 3; //��������ʒu��
		const double Ratio = 0.8; //���S����ڐG�ʒu�܂ł̋����䗦�i1.0���ǁj

		const vec3d vecAxis1 = [&]()->vec3d
		{
			if (vecN != vec3d(1.0, 0.0, 0.0) && vecN != vec3d(-1.0, 0.0, 0.0))
				return vec3d(1.0, 0.0, 0.0);
			return vec3d(0.0, 1.0, 0.0);
		}(); //�@���Ɛ����ȃx�N�g���𓾂邽�߂̎�

		vec3d vecT = vec3d::GetCross(vecN, vecAxis1).GetNormalize(); //�@���Ɛ����ȃx�N�g��

		double Distance;

		for (int i = 0; i < Num; i++) //�ʒu���̐���������
		{
			Quaternion qt = Quaternion(vecN, PI2 * (double(i) / Num));
			vec3d vecDir; //�����x�N�g��
			qt.TransformVector(vecDir, vecT);

			for (int j = 0; j < 2; j++) //���b�V���̕��̂𑖍�
			{
				for (std::vector<std::shared_ptr<Face>>::const_iterator itr = vectorspMeshObj.at(j)->getvectorspFaceObj().begin(); itr != vectorspMeshObj.at(j)->getvectorspFaceObj().end(); itr++)
				{
					if ((*itr)->CalcDistance(Distance, vecHitPos, vecDir)) //�ڐG�ʒu�ƕ����x�N�g������A�ʂƂ̏Փ˔�������A�Փ˂��Ă����ꍇ
					{
						Distance = abs(Distance);
						vectorvecSubPos.push_back(vecHitPos + vecDir * Distance * Ratio); //�T�u�\���o�[�ʒu��z��ɒǉ�
						goto BREAK1;
					}
				}
			}
		BREAK1:;
		}

		return;
	}
}

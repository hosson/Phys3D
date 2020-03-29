#include "Body.h"
#include "Contact.h"
#include "Mesh.h"
#include "World.h"
#include "../hoLib/num/Matrix.h"
#include "../hoLib/Error.h"

using namespace ho::num;

namespace Phys3D
{
	//�R���X�g���N�^
	Body::Body(const vec3d &vecPos, const vec3d &vecLinearVelocity, const Quaternion &qtAngle, const vec3d &vecL, const double &Mass, const Matrix &mtrxInertia, const std::shared_ptr<Mesh> &spMeshObj, const double &e, const double &mu, const bool &Fixed)
		: vecPos(vecPos), vecLinearVelocity(vecLinearVelocity), qtAngle(qtAngle), vecL(vecL), Mass(Mass), mtrxInertia(mtrxInertia), spMeshObj(spMeshObj), e(e), mu(mu), Fixed(Fixed), LastUpdateFrame(-1)
	{
		if (this->Fixed) //�Œ蕨�̏ꍇ
			this->MassInv = 0;
		else
			this->MassInv = 1.0 / this->Mass;

		//�������[�����g�e���\���̋t�s������߂�
		if (this->Fixed) //�Œ蕨�̏ꍇ
		{
			this->mtrxInertiaInv.Resize(3, 3);
			this->mtrxInertiaInv.InitZero(); //�S������0
		} else {
			this->mtrxInertiaInv = this->mtrxInertia.CalcInverseMatrix(); //�t�s����v�Z
		}
		if (this->mtrxInertiaInv.GetxSize() == 0)
			ERR(true, TEXT("�t�s�񂪋��܂�Ȃ��������[�����g�e���\�����w�肳��܂����B"), __WFILE__, __LINE__);

		CalcmtrxInertiaNowInv(); //���ݎ����i���ݎp���j�ł̊����e���\���̋t�s����v�Z����
		this->vecOmega = this->vecL * mtrxInertiaNowInv;//�p�^���ʃx�N�g���Ɗ����e���\���̋t�s���ώZ���āA�p���x�ւ����߂Ă����B

		this->BoundingRadius = spMeshObj->GetBoundingRadius();
	}
	
	//�f�X�g���N�^
	Body::~Body() 
	{}

	//�O���t�B�b�N�X�p�̍��W�ϊ��s����擾����
	Matrix Body::GetCoordTransMatrix() const
	{
		Matrix mtrx = this->qtAngle.GetMatrix4x4(); //���ݎp���̉�]�s����擾

		//���s�ړ�������ݒ�
		mtrx(0, 3) = this->vecPos.x;
		mtrx(1, 3) = this->vecPos.y;
		mtrx(2, 3) = this->vecPos.z;

		return mtrx;
	}

	/*
	//���ׂĂ̒��_���W�����[���h���W�Ŏ擾
	void Body::GetAllvecVtWorld(std::vector<vec3d> *pvectorvecVtWorld)
	{
		*pvectorvecVtWorld = vectorvecVt; //���[�J�����W���R�s�[

		for (unsigned int i = 0; i < pvectorvecVtWorld->size(); i++)
		{
			qtAngle.TransformVector(&pvectorvecVtWorld->at(i)); //���[���h���W�ɉ�]�ϊ�
			pvectorvecVtWorld->at(i) += vecPos; //���s�ړ�
		}
		
		return;
	}
	*/

	//�ړ�
	void Body::Move(const double &dt) 
	{
		vecOmega = vecL * mtrxInertiaNowInv;//�p�^���ʃx�N�g���Ɗ����e���\���̋t�s���ώZ���āA�p���x�ւ����߂�

		vecPos += vecLinearVelocity * dt;

		/*
		{
			//�s��œ�����]������ꍇ�̋L�q
			vec3d vecOmegaDt = vecOmega * dt;
			Matrix mtrxOmega(vecOmegaDt.GetNormalize(), vecOmegaDt.GetPower());
			Matrix mtrx = this->qtAngle.GetMatrix3x3();
			mtrx = mtrxOmega * mtrx; 
			this->qtAngle = Quaternion(mtrx);
			qtAngle.Normalize(); //�N�H�[�^�j�I���𐳋K��
		}*/
		
		
		//qtAngle += 0.5 * qtAngle * dt * Quaternion(0, vecOmega.x, vecOmega.y, vecOmega.z); //�p���x�ւɉ����Ďp����ω�������
		qtAngle += 0.5 * Quaternion(0, vecOmega.x, vecOmega.y, vecOmega.z) * dt * qtAngle; //�p���x�ւɉ����Ďp����ω�������
		qtAngle.Normalize(); //�N�H�[�^�j�I���𐳋K��
	
		CalcmtrxInertiaNowInv(); //�p���i�N�H�[�^�j�I���j���ω������̂ŁA���ݎ����i���ݎp���j�ł̊����e���\���̋t�s����v�Z���Ă���

		return;
	}

	//���ݎ����i���ݎp���j�ł̊����e���\���̋t�s����v�Z����
	void Body::CalcmtrxInertiaNowInv()
	{
		if (Fixed) //�Œ蕨�̏ꍇ
		{
			this->mtrxInertiaNowInv.Resize(3, 3);
			this->mtrxInertiaNowInv.InitZero();
		} else {
			//���݂̎p�������]�s��Ƃ��̓]�u�s������߂�
			Matrix mtrxRotation = qtAngle.GetMatrix3x3(); //���݂̎p������3X3�̉�]�s����擾
			Matrix mtrxTransposedRotation = mtrxRotation.CalcTransposedMatrix(); //��]�s��̓]�u�s��

			//���݂̎p���ł̊������[�����g�e���\���̋t�s������߂�
			this->mtrxInertiaNowInv = mtrxRotation * mtrxInertiaInv * mtrxTransposedRotation;
		}

		/*
		if (Fixed) //�Œ蕨�̏ꍇ
		{
			mtrxInverseInertiaTensorT.Resize(3, 3);
			mtrxInverseInertiaTensorT.InitZero(); //�S�Ă̐�����0
		} else {
			//���v���ǁI�@G14�y�[�W��ǂނƁA�Ō�̊����e���\���̋t�s��̌v�Z���Ȃ��邱�Ƃ������Ă���B��Ɋ�{�p���̊����e���\���̋t�s����v�Z���Ă����B

			//���݂̎p�������]�s��Ƃ��̓]�u�s������߂�
			ho::Matrix mtrxRotation; //��]�s��
			qtAngle.CalcMatrix(&mtrxRotation); //���݂̎p������3X3�̉�]�s����擾
			ho::Matrix mtrxTransposedRotation = mtrxRotation.CalcTransposedMatrix(); //��]�s��̓]�u�s��
			//ho::Matrix mtrxInverseRotation = mtrxRotation.CalcInverseMatrix(); //��]�s��̋t�s��i��]�s��ɂ����ẮA�t�s��=�]�u�s��ƂȂ�̂ŁA���̍s�̌v�Z�̑����]�u�s����g���j

			//��{�p���̊����e���\������A���݂̎p���̊����e���\�������߂�
			ho::Matrix mtrxInertiaTensorNow = mtrxRotation * mtrxInertiaTensor * mtrxTransposedRotation;

			ho::Matrix mtrxInverseInertiaTensorNow; //���ݎp���̊����e���\���̋t�s��
			mtrxInverseInertiaTensorT = mtrxInertiaTensorNow.CalcInverseMatrix(); //�t�s����v�Z
		}*/
	
		return;
	}

	//���[���h���W��̈�_�ł̍��̊Ԃ̑��Α��x���v�Z
	vec3d Body::GetRelativeVelocity(const std::weak_ptr<Body> &wpBodyObj1, const std::weak_ptr<Body> &wpBodyObj2, const vec3d &vecWorldPos)
	{
		const std::shared_ptr<Body> spBodyObj1 = wpBodyObj1.lock();
		const std::shared_ptr<Body> spBodyObj2 = wpBodyObj2.lock();

		//�����]���̊p���x�Əd�S����̃x�N�g���̊O�ςł��̉�]���ɑ΂���p���x���o��

		vec3d vecLocalPos[2]; //���[���h���W��̈�_�����[�J�����W�ɕϊ��������W
		vecLocalPos[0] = vecWorldPos - spBodyObj1->getvecPos();
		vecLocalPos[1] = vecWorldPos - spBodyObj2->getvecPos();
		vec3d vecOmega[2]; //�p���x
		vecOmega[0] = spBodyObj1->getvecOmega();
		vecOmega[1] = spBodyObj2->getvecOmega();

		vec3d vecV[2]; //���x�x�N�g��
		vecV[0] = spBodyObj1->getvecLinearVelocity() + vec3d::GetCross(vecOmega[0], vecLocalPos[0]);//
		vecV[1] = spBodyObj2->getvecLinearVelocity() + vec3d::GetCross(vecOmega[1], vecLocalPos[1]);//

		/*
		//���[�J�����W�Ɗp���x���E��n���W�ɕϊ�
		for (int i = 0; i < 2; i++)
		{
			vecLocalPos[i] = vecLocalPos[i].GetExchangeLeftRight();
			vecOmega[i] = vecOmega[i].GetExchangeLeftRight();
		}

		vec3d vecV[2]; //���x�x�N�g��
		vecV[0] = pBodyObj1->GetvecParallelV() + vec3d::GetCrossProduct(vecLocalPos[0], vecOmega[0]).GetExchangeLeftRight();
		vecV[1] = pBodyObj2->GetvecParallelV() + vec3d::GetCrossProduct(vecLocalPos[1], vecOmega[1]).GetExchangeLeftRight();
		*/

		return vecV[0] - vecV[1];
	}

	//�d�͂�t������
	void Body::ApplyGravity(const vec3d &vecGravity)
	{
		if (Fixed)
		{
			this->vecLinearVelocity = vec3d(0, 0, 0);
			this->vecL = vec3d(0, 0, 0);
		} else {
			this->vecLinearVelocity += vecGravity;
		}

		return;
	}

	//�͐ς�������
	void Body::ApplyForce(const vec3d &vecForce, const vec3d &vecLocalPos) 
	{
		if (Fixed)
			return;

		this->vecLinearVelocity += vecForce * this->MassInv; //���i�����x�͈ʒu�Ɋ֌W�Ȃ����Z�����
		this->vecL += vec3d::GetCross(vecLocalPos, vecForce); //�p�����x�͑��Έʒu�Ɨ͂̃x�N�g���Ƃ̊O�ςŋ��܂�
		this->vecOmega = this->vecL * this->mtrxInertiaNowInv;//�p�^���ʃx�N�g���Ɗ����e���\���̋t�s���ώZ���āA�p���x�ւ����߂�

		return;
	}

	//�ʒu�𓮂����͂�������
	void Body::ApplyCorrect(const vec3d &vecForce, const vec3d &vecLocalPos) 
	{
		if (Fixed)
			return;

		this->vecPos += vecForce * this->MassInv; //���i�����x�͈ʒu�Ɋ֌W�Ȃ����Z�����

		vec3d vecLocalL = vec3d::GetCross(vecLocalPos, vecForce); //�p���ω��ɉe������p�^���ʂ͑��Έʒu�Ɨ͂̃x�N�g���Ƃ̊O�ςŋ��܂�
		vec3d vecOmega2 = vecLocalL * this->mtrxInertiaNowInv;//�ω�����p�^���ʃx�N�g���Ɗ����e���\���̋t�s���ώZ���āA�ω�����p�x�ւ����߂�

		/* //����͍s��œ�����]������ꍇ
		ho::Matrix mtrx, mtrxOmega(&vecOmega2.Normalize(), vecOmega2.GetPower());
		qtAngle.CalcMatrix(&mtrx);
		mtrx = mtrxOmega * mtrx;
		qtAngle = ho::Quaternion(&mtrx);
		*/

		qtAngle += 0.5 * Quaternion(0, vecOmega2.x, vecOmega2.y, vecOmega2.z) * qtAngle; //�p���x�ւɉ����Ďp����ω�������
		qtAngle.Normalize(); //�N�H�[�^�j�I���𐳋K��

		CalcmtrxInertiaNowInv(); //�p���i�N�H�[�^�j�I���j���ω������̂ŁA���ݎ����i���ݎp���j�ł̊����e���\���̋t�s����v�Z���Ă���

		return;
	}


	//�����ӂ��܂܂��ʂ����ׂė񋓂���i�ӂ̒��_�C���f�b�N�X�z��̃C���f�b�N�X�̕ӂɂ��܂܂��ʂ̒��_�C���f�b�N�X�z�񒆂̃C���f�b�N�X��񋓁j
	void Body::GetlistVtFaceiiFromVtEdgeii(std::list<int> *plistVtFaceii, int VtEdgeii)
	{
		plistVtFaceii->clear();

		for (unsigned int i = 0; i < vectorVtFaceIndex.size(); i+= 3) //���ׂĂ̖ʂ𑖍�
		{
			//�ӂ��܂܂�Ă����ꍇ
			if ((vectorVtFaceIndex.at(i) == vectorVtEdgeIndex.at(VtEdgeii) && vectorVtFaceIndex.at(i + 1) == vectorVtEdgeIndex.at(VtEdgeii + 1)) ||
				(vectorVtFaceIndex.at(i) == vectorVtEdgeIndex.at(VtEdgeii + 1) && vectorVtFaceIndex.at(i + 1) == vectorVtEdgeIndex.at(VtEdgeii)) ||
				(vectorVtFaceIndex.at(i + 1) == vectorVtEdgeIndex.at(VtEdgeii) && vectorVtFaceIndex.at(i + 2) == vectorVtEdgeIndex.at(VtEdgeii + 1)) ||
				(vectorVtFaceIndex.at(i + 1) == vectorVtEdgeIndex.at(VtEdgeii + 1) && vectorVtFaceIndex.at(i + 2) == vectorVtEdgeIndex.at(VtEdgeii)) ||
				(vectorVtFaceIndex.at(i + 2) == vectorVtEdgeIndex.at(VtEdgeii) && vectorVtFaceIndex.at(i) == vectorVtEdgeIndex.at(VtEdgeii + 1)) ||
				(vectorVtFaceIndex.at(i + 2) == vectorVtEdgeIndex.at(VtEdgeii + 1) && vectorVtFaceIndex.at(i) == vectorVtEdgeIndex.at(VtEdgeii)))
			{
				plistVtFaceii->push_back(i);
			}
		}

		return;
	}

	/*
	//���ׂĂ̖ʂ̌��݂̎p���ł̖@���x�N�g�����擾����
	void Body::GetvectorvecFaceNRotation(std::vector<vec3d> *pvectorvecFaceN) 
	{
		*pvectorvecFaceN = vectorvecFaceN; //�V�����z��ɃR�s�[

		for (unsigned int i = 0; i < pvectorvecFaceN->size(); i++)
			qtAngle.TransformVector(&pvectorvecFaceN->at(i)); //�@���x�N�g�������݂̎p���ɉ�]

		return;
	}
	*/

	//���b�V���̃��[���h���W���X�V����
	void Body::UpdateMeshVertexWorldPos()
	{
		//if (this->LastUpdateFrame < this->wpWorldObj.lock()->getFrame()) //���݂̃t���[���ł͂܂��X�V����Ă��Ȃ��ꍇ
			this->spMeshObj->UpdateVertexWorldPos(this->qtAngle, this->vecPos); //���b�V���̍��W���X�V

		return;
	}

	//�ڐG�I�u�W�F�N�g��ǉ�
	void Body::AddpContactObj(Contact *pContactObj)
	{
		/*
		listpContactObj.push_back(pContactObj);
		*/

		return;
	}

	//�ڐG�I�u�W�F�N�g���폜
	bool Body::ErasepContactObj(Contact *pContactObj)
	{
		/*
		for (std::list<Contact *>::iterator itr = listpContactObj.begin(); itr != listpContactObj.end(); itr++)
		{
			if (*itr == pContactObj)
			{
				listpContactObj.erase(itr);
				return TRUE; //�폜�ł���
			}
		}

		return FALSE; //�폜�Ώۂ̃I�u�W�F�N�g��������Ȃ�����
		*/

		return true;
	}
}
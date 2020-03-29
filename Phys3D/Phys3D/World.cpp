#include "World.h"
#include "Body.h"
#include "Collision.h"
#include "Contact.h"
#include <algorithm>
#include "Phys3D.h"
#include "Solver.h"

using namespace ho::num;

namespace Phys3D
{
	//�R���X�g���N�^
	World::World(const std::weak_ptr<Phys3D> &wpPhys3DObj, const double &dt, const vec3d &vecGravity)
		: wpPhys3DObj(wpPhys3DObj), dt(dt), vecGravity(vecGravity)
	{
		this->wpThisObj = this->sp_this;
		this->FrameFlag = 0;
	}

	//�f�X�g���N�^
	World::~World()
	{
	}

	//1�t���[�����̏���
	void World::Update() 
	{
		Delete(); //�G���W���O���ŎQ�Ƃ���Ă��Ȃ����̂��폜����
		ApplyGravity(); //�S�Ă̍��̂ɏd�͉����x��������
	
		std::list<CONTACT> listContact; //�ڐG��񃊃X�g
		this->wpPhys3DObj.lock()->getspCollisionObj()->DetectContact(listContact, this->listspBodyObj); //�S�Ă̕��̂���ڐG�����擾

		CreateSolver(listContact); //�ڐG��񂩂�\���o�[����
		Solve(); //�S���������s��

		Move(); //�S�Ă̍��̂��ړ�

		FrameFlag = 1 - FrameFlag;

		return;
	}

	//�쐬�ς݂̕��̃I�u�W�F�N�g��ǉ�����
	void World::AddspBodyObj(const std::shared_ptr<Body> &spBodyObj) 
	{
		spBodyObj->setwpWorldObj(this->wpThisObj);
		this->listspBodyObj.push_back(spBodyObj);

		return;
	}

	//�G���W���O���ŎQ�Ƃ���Ă��Ȃ����̂��폜����
	void World::Delete()
	{
		for (std::list<std::shared_ptr<Body>>::iterator itr = this->listspBodyObj.begin(); itr != this->listspBodyObj.end();)
		{
			if (itr->use_count() == 1)
				itr = this->listspBodyObj.erase(itr);
			else
				itr++;
		}

		return;
	}

	//�S�Ă̍��̂ɏd�͉����x��������
	void World::ApplyGravity() 
	{
		const vec3d vecGravity = this->vecGravity * this->dt;

		std::for_each(this->listspBodyObj.begin(), this->listspBodyObj.end(), [&](const std::shared_ptr<Body> &spBodyObj)
		{
			spBodyObj->ApplyGravity(vecGravity);
		});

		return;
	}

	//�ڐG��񂩂�\���o�[�𐶐�
	void World::CreateSolver(std::list<CONTACT> listContact)
	{
		this->listspSolverObj[this->FrameFlag].clear(); //�\���o�[���N���A

		for (std::list<std::shared_ptr<Solver>>::iterator itr1 = this->listspSolverObj[1 - this->FrameFlag].begin(); itr1 != this->listspSolverObj[1 - this->FrameFlag].end(); itr1++) //1�t���[���O�̃\���o�[���X�g�𑖍�
		{
			for (std::list<CONTACT>::iterator itr2 = listContact.begin(); itr2 != listContact.end(); itr2++) //�ڐG���X�g�𑖍�
			{
				if ((*itr1)->getContact().Pair == itr2->Pair) //���̂̑g�ݍ��킹�������ꍇ
				{
					(*itr1)->Continue(*itr2); //�ڐG�p���̂��߁A�ڐG�����X�V
					this->listspSolverObj[this->FrameFlag].push_back(*itr1); //���݂̃t���[���̃��X�g�ɃR�s�[

					//�\���o�[���p�����ꂽ�ڐG�I�u�W�F�N�g�͍폜
					itr2 = listContact.erase(itr2);
					break;
				}
			}
		}

		const std::weak_ptr<Collision> wpCollisionObj = this->wpPhys3DObj.lock()->getspCollisionObj();

		for (std::list<CONTACT>::const_iterator itr = listContact.begin(); itr != listContact.end(); itr++) //�ڐG���X�g�𑖍�
			this->listspSolverObj[this->FrameFlag].push_back(std::shared_ptr<Solver>(new Solver(*itr, wpCollisionObj))); //�ڐG��񂩂�\���o�[�𐶐����ă��X�g�ɒǉ�

		return;
	}

	//�S���������s��
	void World::Solve()
	{
		const int SolveNum = this->wpPhys3DObj.lock()->getSolveNum(); //������

		for (std::list<std::shared_ptr<Solver>>::iterator itr = this->listspSolverObj[this->FrameFlag].begin(); itr != this->listspSolverObj[this->FrameFlag].end(); itr++) //���݂̃t���[���̃\���o�[�𑖍�
			(*itr)->PreSolve(); //�S�������̎��O����

		for (int i = 0; i < SolveNum; i++)
			for (std::list<std::shared_ptr<Solver>>::iterator itr = this->listspSolverObj[this->FrameFlag].begin(); itr != this->listspSolverObj[this->FrameFlag].end(); itr++) //���݂̃t���[���̃\���o�[�𑖍�
				(*itr)->SolveVelocity(); //���x�������S������

		for (int i = 0; i < 1; i++)
			for (std::list<std::shared_ptr<Solver>>::iterator itr = this->listspSolverObj[this->FrameFlag].begin(); itr != this->listspSolverObj[this->FrameFlag].end(); itr++) //���݂̃t���[���̃\���o�[�𑖍�
				(*itr)->SolvePosition(); //�ʒu�������S������

		return;
	}

	//�S�Ă̍��̂��ړ�
	void World::Move() 
	{
		std::for_each(this->listspBodyObj.begin(), this->listspBodyObj.end(), [&](const std::shared_ptr<Body> &spBodyObj)
		{
			spBodyObj->Move(dt);
		});
		
		return;
	}
}
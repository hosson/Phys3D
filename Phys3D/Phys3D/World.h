//���E�N���X
#pragma once
#include "../hoLib/num/vector.h"
#include <map>
#include <list>
#include <memory>
#include "../hoLib/sp_init.hpp"
#include "Contact.h"

using namespace ho::num;

namespace Phys3D
{
	class Phys3D;
	class Body;
	class Collision;
	class Solver;

	class World : public ho::sp_base<World>
	{
	public:
		World(const std::weak_ptr<Phys3D> &wpPhys3DObj, const double &dt, const vec3d &vecGravity); //�R���X�g���N�^
		~World(); //�f�X�g���N�^

		void Update(); //1�t���[�����̏���
		void AddspBodyObj(const std::shared_ptr<Body> &spBodyObj); //�쐬�ς݂̕��̃I�u�W�F�N�g��ǉ�����

		//�A�N�Z�b�T
		const int &getFrame() const { return Frame; }
	private:
		std::weak_ptr<World> wpThisObj;
		const std::weak_ptr<Phys3D> wpPhys3DObj;
		double dt; //���ݎ���t[�b]
		vec3d vecGravity; //�d�͉����x[m/s^2] �i�ʏ�̒n����Ȃ� (0, -9.80665) ������j
		std::list<std::shared_ptr<Body>> listspBodyObj; //���̃I�u�W�F�N�g���X�g
		std::list<std::shared_ptr<Solver>> listspSolverObj[2]; //�\���o�[���X�g
		int FrameFlag; //1�t���[�����Ƃ�0��1�����݂ɐ؂�ւ��
		int Frame; //���݂܂ł̌o�߃t���[����

		void Delete(); //�G���W���O���ŎQ�Ƃ���Ă��Ȃ����̂��폜����

		void ApplyGravity(); //�S�Ă̍��̂ɏd�͉����x��������
		void CreateSolver(std::list<CONTACT> listContact); //�ڐG��񂩂�\���o�[�𐶐�
		void Solve(); //�S���������s��
		void Move(); //�S�Ă̍��̂��ړ�
	};

}

/*Phys3D�N���X��1�܂��͕�����World�N���X�I�u�W�F�N�g�����B
������World�N���X�I�u�W�F�N�g�����ƁA�����ɕʁX�̐��E���V�~�����[�V�����ł���B
�Ⴆ�Ώ�ʓ]���Ȃǂŉ�ʊO������������E�����̂܂ܕێ����邱�Ƃ��ł���B*/
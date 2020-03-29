#include "hoLib.h"
#include "LibError.h"
#include <Windows.h>
#include "Random.h"

namespace ho {
	//�R���X�g���N�^
	hoLib::hoLib()
	{
		LibError::pLibErrorObj = new ho::LibError(); //���C�u�������̃G���[�����I�u�W�F�N�g���쐬
		Randomize(); //�W�������^�C�����C�u�����̃����_���֐���������
	}

	//�f�X�g���N�^
	hoLib::~hoLib()
	{
		SDELETE(LibError::pLibErrorObj);
	}
}

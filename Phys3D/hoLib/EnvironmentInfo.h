//OS�o�[�W�����Ȃǂ̊����N���X
#pragma once
#include "tstring.h"

namespace ho
{
	class EnvironmentInfo
	{
	public:
		EnvironmentInfo(); //�R���X�g���N�^
		~EnvironmentInfo(); //�f�X�g���N�^

		//�A�N�Z�b�T
		const tstring &GetstrOS() const { return strOS; } 
		const tstring &GetstrProcessorNum() const { return strProcessorsNum; }
		const tstring &GetstrMemory() const { return strMemory; }
	private:
		tstring strOS; //OS�̖��O�iWindows7 �Ȃǁj
		tstring strProcessorsNum; //�v���Z�b�T�[��
		tstring strMemory; //�������Ɋւ�����
	};
}

//LOD(Level of Detail)�N���X

#pragma once
#include <vector>
#include "Common.h"
#include "tstring.h"

namespace ho
{
	template <class T> class LOD
	{
	public:
		//�R���X�g���N�^
		LOD(int Objects)
		{
			this->Objects = Objects;
			vectorpMeshObj.resize(Objects, NULL);
			vectorDistance.resize(Objects, 0);
		}

		//�f�X�g���N�^
		~LOD() {}

		//���b�V���I�u�W�F�N�g��o�^
		bool RegistMeshObj(T *pMeshObj, int Index, float Distance = 0)
		{
			if (Index < 0 || Index >= Objects) //�o�^�\�ȃC���f�b�N�X�͈̔͊O�̏ꍇ
				return FALSE;
		
			vectorpMeshObj.at(Index) = pMeshObj;

			if (Distance != 0) //����p�������w�肳��Ă����ꍇ
				vectorDistance.at(Index) = Distance;

			return TRUE;
		}

		//�����ɉ����ēK�؂ȃ��b�V���I�u�W�F�N�g�𓾂�
		T *GetpMeshObjFromDistance(float Distance)
		{
			for (int i = 0; i < Objects; i++)
			{
				if (Distance < vectorDistance.at(i))
					return vectorpMeshObj.at(i);
			}

			return vectorpMeshObj.at(Objects - 1); //�����͈̔͊O�̏ꍇ�͈�ԉ����p�̃I�u�W�F�N�g��Ԃ�
		}
	private:
		int Objects; //���b�V���I�u�W�F�N�g�̐�
		std::vector<T *> vectorpMeshObj; //���b�V���I�u�W�F�N�g�ւ̃|�C���^�z��	
		std::vector<float> vectorDistance; //����p�����z��
	};
}

/*�f�B�e�[���̈قȂ镡���̃��b�V���I�u�W�F�N�g�ւ̃|�C���^�����B
1�̕��̂ɑ΂��Ă���LOD�N���X�̃C���X�^���X��1���ARegistMeshObj()�֐���
���b�V���I�u�W�F�N�g��o�^���Ă����B�`�掞�ɂ́AGetpMeshObjFromDistance()�֐���
�J�����ʒu����I�u�W�F�N�g�܂ł̋�����n���A�����ɉ������f�B�e�[����
���b�V���I�u�W�F�N�g�𓾂ĕ`�悷��B

���̃N���X�̓��b�V���I�u�W�F�N�g�̃|�C���^���������ŁA���̂����o������
�Ǘ�������폜����@�\�͍��̂Ƃ��뎝���Ȃ��B�Ȃ̂ŁA�f�X�g���N�^���Ăяo���Ă�
�o�^���ꂽ���b�V���I�u�W�F�N�g���͍̂폜����Ȃ��̂Ń��������[�N���ɑ΂���
���ӂ��K�v�ł���B*/

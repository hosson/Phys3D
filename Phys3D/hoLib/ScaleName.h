//���K�̊K���̃N���X

#include <Windows.h>

namespace ho {
	class SCALENAME
	{
	public:
		SCALENAME(int Scale); //�R���X�g���N�^�i0�`11�Ŏw��j
		SCALENAME(const TCHAR *pScale); //Cis Des ���̃h�C�c�����Ŏw��j

		int Scale; //C�`B�܂ł̊K��

	private:
		int StringToNumber(const TCHAR *pStr); //Cis Des ���̃h�C�c������0�`11�̐����ɕϊ�

		const static TCHAR ScaleName[34][4]; //�������0�`11�ɕϊ����邽�߂̃e�[�u��
		const static int ScaleNumber[34]; //�������0�`11�ɕϊ����邽�߂̃e�[�u��
	};
}

/*C�`H�܂ł�12��ނ̉��K�����N���X�B
�����I�ɂ�0�`11�܂ł̐����ŕ\������邪�A�h�C�c�����ŕ\��������ϊ�������Ȃǂ�
�O���Ƃ̏_��ȃC���^�[�t�F�[�X�����B*/

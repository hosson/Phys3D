#include "D3DResource.h"
#include "Debug.h"

namespace ho
{
	//�R���X�g���N�^
	D3DResourceManager::D3DResourceManager()
	{
	}

	//�f�X�g���N�^
	D3DResourceManager::~D3DResourceManager()
	{
		Optimize();	//�œK���i���g�p���\�[�X���폜�j

		if (listpD3DResourceObj.size() > 0) //�܂��������Ă��Ȃ����\�[�X������ꍇ
			ER(TEXT("�܂��������Ă��Ȃ����\�[�X������܂��B"), __WFILE__, __LINE__, true);
	}

	//���\�[�X��o�^����
	void D3DResourceManager::Regist(D3DResource *pD3DResourceObj) 
	{
		listpD3DResourceObj.push_back(pD3DResourceObj);

		return;
	}

	//�œK���i���g�p���\�[�X���폜�j
	void D3DResourceManager::Optimize()
	{
		for (std::list<D3DResource *>::iterator itr = listpD3DResourceObj.begin(); itr != listpD3DResourceObj.end();)
		{
			if ((*itr)->GetRefCount() == 1) //�Q�ƃJ�E���^��1�̏ꍇ�i���\�[�X�N���X�̒��ɂ���COM�|�C���^���������\�[�X���Q�Ƃ��Ă����Ԃ̏ꍇ�j
			{
				SDELETE(*itr); //���\�[�X�N���X���폜
				itr = listpD3DResourceObj.erase(itr);
			} else {
				itr++;
			}
		}

		return;
	}

	//D3DPOOL_DEFAULT �t���O�ō쐬���ꂽ���\�[�X�����
	void D3DResourceManager::OnLostDevice() 
	{
		for (std::list<D3DResource *>::iterator itr = listpD3DResourceObj.begin(); itr != listpD3DResourceObj.end(); itr++)
			(*itr)->OnLostDevice();

		return;
	}

	//D3DPOOL_DEFAULT �t���O�ō쐬���ꂽ���\�[�X���č쐬����
	void D3DResourceManager::OnResetDevice(LPDIRECT3DDEVICE9 pD3DDevice) 
	{
		for (std::list<D3DResource *>::iterator itr = listpD3DResourceObj.begin(); itr != listpD3DResourceObj.end(); itr++)
			(*itr)->OnResetDevice(pD3DDevice);

		return;
	}
}


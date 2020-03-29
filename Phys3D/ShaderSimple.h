//�V���v���ȃV�F�[�_
#include "hoLib\Shader.h"
#include <boost\shared_ptr.hpp>
#include "hoLib\ComPtr.hpp"
#include "ShaderSimple_base.h"

namespace ho
{
	namespace D3D11
	{
		class D3D11;
	}
}

using namespace DirectX;

class ShaderSimple : public ho::D3D11::Shader_Base, public ShaderSimple_base
{
public:
	ShaderSimple(const std::shared_ptr<ho::D3D11::D3D11> &spD3D11Obj); //�R���X�g���N�^
	~ShaderSimple(); //�f�X�g���N�^

	void Draw() override; //�`��
	void DrawUp(CustomVertex *const pVertex, const unsigned int &Vertices, unsigned int *const pIndex, const unsigned int &Indices, const XMMATRIX &mtrxWorld); //���_�����w�肵�ĕ`��
private:

	ho::ComPtr<ID3D11VertexShader *> cpVS; //���_�V�F�[�_
	ho::ComPtr<ID3D11InputLayout *> cpIL; //���̓��C�A�E�g
	ho::ComPtr<ID3D11GeometryShader *> cpGS; //�W�I���g���V�F�[�_
	ho::ComPtr<ID3D11PixelShader *> cpPS; //�s�N�Z���V�F�[�_

	ho::ComPtr<ID3D11Buffer *> cpVB; //���_�o�b�t�@
	ho::ComPtr<ID3D11Buffer *> cpCB; //�萔�o�b�t�@

	ho::ComPtr<ID3D11Buffer *> CreateVertexBuffer(CustomVertex *const pVertex, const unsigned int &Vertices); //���_�o�b�t�@����
	ho::ComPtr<ID3D11Buffer *> CreateIndexBuffer(unsigned int *const pIndex, const unsigned int &Indices); //�C���f�b�N�X�o�b�t�@����
	void DrawInit(); //�`��ɂ���ĕύX���ꂽ�f�o�C�X�̏�Ԃ����ɖ߂�
};
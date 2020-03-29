//2D�e�N�X�`����`�悷��V�F�[�_
#include "hoLib\Shader.h"
#include <boost\shared_ptr.hpp>
#include "hoLib\ComPtr.hpp"

namespace ho
{
	namespace D3D11
	{
		class D3D11;
	}
}

class Shader2DTexture : public ho::D3D11::Shader_Base
{
public:
	//���_�V�F�[�_�֑��邽�߂̒��_���\����
	struct CustomVertex
	{
		DirectX::XMFLOAT3 position; //�ʒu���
		DirectX::XMFLOAT2 Texcoord; //UV
	};
public:
	Shader2DTexture(const std::shared_ptr<ho::D3D11::D3D11> &spD3D11Obj); //�R���X�g���N�^
	~Shader2DTexture(); //�f�X�g���N�^

	void Draw() override; //�`��
	void setBuffer(void *p); //�摜�o�b�t�@���Z�b�g
private:
	ho::ComPtr<ID3D11VertexShader *> cpVS; //���_�V�F�[�_
	ho::ComPtr<ID3D11InputLayout *> cpIL; //���̓��C�A�E�g
	ho::ComPtr<ID3D11PixelShader *> cpPS; //�s�N�Z���V�F�[�_

	ho::ComPtr<ID3D11Buffer *> cpVB; //���_�o�b�t�@

	ho::ComPtr<ID3D11Texture2D *> cpTexture; //�e�N�X�`��
	ho::ComPtr<ID3D11ShaderResourceView *> cpSRV; //�e�N�X�`���p�̃V�F�[�_���\�[�X�r���[
	ho::ComPtr<ID3D11SamplerState *> cpSampler; //�T���v��
};
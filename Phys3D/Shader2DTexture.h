//2Dテクスチャを描画するシェーダ
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
	//頂点シェーダへ送るための頂点情報構造体
	struct CustomVertex
	{
		DirectX::XMFLOAT3 position; //位置情報
		DirectX::XMFLOAT2 Texcoord; //UV
	};
public:
	Shader2DTexture(const std::shared_ptr<ho::D3D11::D3D11> &spD3D11Obj); //コンストラクタ
	~Shader2DTexture(); //デストラクタ

	void Draw() override; //描画
	void setBuffer(void *p); //画像バッファをセット
private:
	ho::ComPtr<ID3D11VertexShader *> cpVS; //頂点シェーダ
	ho::ComPtr<ID3D11InputLayout *> cpIL; //入力レイアウト
	ho::ComPtr<ID3D11PixelShader *> cpPS; //ピクセルシェーダ

	ho::ComPtr<ID3D11Buffer *> cpVB; //頂点バッファ

	ho::ComPtr<ID3D11Texture2D *> cpTexture; //テクスチャ
	ho::ComPtr<ID3D11ShaderResourceView *> cpSRV; //テクスチャ用のシェーダリソースビュー
	ho::ComPtr<ID3D11SamplerState *> cpSampler; //サンプラ
};
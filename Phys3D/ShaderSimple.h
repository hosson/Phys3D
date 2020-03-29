//シンプルなシェーダ
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
	ShaderSimple(const std::shared_ptr<ho::D3D11::D3D11> &spD3D11Obj); //コンストラクタ
	~ShaderSimple(); //デストラクタ

	void Draw() override; //描画
	void DrawUp(CustomVertex *const pVertex, const unsigned int &Vertices, unsigned int *const pIndex, const unsigned int &Indices, const XMMATRIX &mtrxWorld); //頂点情報を指定して描画
private:

	ho::ComPtr<ID3D11VertexShader *> cpVS; //頂点シェーダ
	ho::ComPtr<ID3D11InputLayout *> cpIL; //入力レイアウト
	ho::ComPtr<ID3D11GeometryShader *> cpGS; //ジオメトリシェーダ
	ho::ComPtr<ID3D11PixelShader *> cpPS; //ピクセルシェーダ

	ho::ComPtr<ID3D11Buffer *> cpVB; //頂点バッファ
	ho::ComPtr<ID3D11Buffer *> cpCB; //定数バッファ

	ho::ComPtr<ID3D11Buffer *> CreateVertexBuffer(CustomVertex *const pVertex, const unsigned int &Vertices); //頂点バッファ生成
	ho::ComPtr<ID3D11Buffer *> CreateIndexBuffer(unsigned int *const pIndex, const unsigned int &Indices); //インデックスバッファ生成
	void DrawInit(); //描画によって変更されたデバイスの状態を元に戻す
};
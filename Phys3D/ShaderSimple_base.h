//ShaderSimple.h の基底クラス
//構造体を継承させるため
#pragma once

#include <DirectXMath.h>
using namespace DirectX;

class ShaderSimple_base
{
public:
	//頂点シェーダへ送るための頂点情報構造体
	struct CustomVertex
	{
		XMFLOAT3 position; //位置情報
		XMFLOAT4 Color; //色
		XMFLOAT3 Normal; //法線ベクトル
	};

	//定数バッファ構造体
	struct ConstantBuffer
	{
		XMMATRIX mtrxWorld, mtrxView, mtrxProj;
		XMFLOAT3 vecLight; //ライト方向（光源へ向かう方向）
	};
};
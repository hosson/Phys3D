//2Dのテクスチャを描画するシェーダ

//テクスチャとサンプラ
Texture2D  tex0  : register(t0);
SamplerState sam0  : register(s0);

/////////////////////////////////////////////////////////////////////////////////////
// VSInput structure
/////////////////////////////////////////////////////////////////////////////////////
struct VSInput
{
	float4 Position : POSITION;     //!< 位置座標です.
	float2 Texcoord : TEXCOORD;		//テクスチャ座標
};

/////////////////////////////////////////////////////////////////////////////////////
// GSPSInput structure
/////////////////////////////////////////////////////////////////////////////////////
struct GSPSInput
{
	float4 Position : SV_POSITION;  //位置座標（この SV_POSITION は　float4 であることが必須）
	float2 Texcoord : TEXCOORD;		//テクスチャ座標
};

//-----------------------------------------------------------------------------------
//! @brief      頂点シェーダエントリーポイントです.
//-----------------------------------------------------------------------------------
GSPSInput VSFunc(VSInput input)
{
	GSPSInput output = (GSPSInput)0;

	// 入力データをそのまま流す.
	output.Position = input.Position;
	output.Texcoord = input.Texcoord;

	return output;
}

//------------------------------------------------------------------------------------
//! @brief      ピクセルシェーダエントリーポイントです.
//------------------------------------------------------------------------------------
float4 PSFunc(GSPSInput output) : SV_TARGET0
{
	return tex0.Sample(sam0, output.Texcoord);
}

//シンプルな描画を行うシェーダ

//コンスタントバッファ
cbuffer CBPerFrame : register(b0)
{
	matrix mtrxWorld, mtrxView, mtrxProj;
	float3 vecLight; //ライト方向（光源へ向かう方向）
};

//頂点シェーダへの入力構造体
struct VSInput
{
	float3 Position : POSITION;		//位置
	float4 Color : COLOR0;			//頂点カラー
	float3 Normal : NORMAL0;		//法線ベクトル
};

//ジオメトリシェーダへの入力構造体
struct GSPSInput
{
	float4 Position : SV_POSITION;  //位置
	float4 Color : COLOR0;			//頂点カラー
	float3 Normal : NORMAL0;		//法線ベクトル
};


//頂点シェーダ
GSPSInput VSFunc(VSInput input)
{
	GSPSInput output = (GSPSInput)0;

	//入力データをそのまま流す
	output.Position = float4(input.Position.xyz, 1.0f);
	output.Color = input.Color;
	output.Normal = input.Normal;

	return output;
}

//ジオメトリシェーダ
[maxvertexcount(3)]
void GSFunc(triangle GSPSInput input[3], inout TriangleStream<GSPSInput> stream)
{
	float3 vecNormal = mul(mtrxWorld, input[0].Normal); //法線ベクトルをワールド座標に変換
	float Dot = dot(vecNormal, vecLight); //法線ベクトルと光源へ向かう方向ベクトルとの内積
	float Light = Dot * 0.5f + 0.5f; //内積より色に影響を与える明るさを計算

	for (int i = 0; i < 3; i++)
	{
		GSPSInput output = (GSPSInput)0;

		//出力値設定
		output.Position = mul(mtrxWorld, input[i].Position);
		output.Position = mul(mtrxView, output.Position);
		output.Position = mul(mtrxProj, output.Position);

		output.Color = input[i].Color;
		output.Color.xyz = output.Color.xyz * Light; //RGB成分を光源との角度に応じて調整

		//ストリームに追加
		stream.Append(output);
	}

	stream.RestartStrip();
}

//ピクセルシェーダ
float4 PSFunc(GSPSInput output) : SV_TARGET0
{
	return output.Color;
}

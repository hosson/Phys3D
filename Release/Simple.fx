//�V���v���ȕ`����s���V�F�[�_

//�R���X�^���g�o�b�t�@
cbuffer CBPerFrame : register(b0)
{
	matrix mtrxWorld, mtrxView, mtrxProj;
	float3 vecLight; //���C�g�����i�����֌����������j
};

//���_�V�F�[�_�ւ̓��͍\����
struct VSInput
{
	float3 Position : POSITION;		//�ʒu
	float4 Color : COLOR0;			//���_�J���[
	float3 Normal : NORMAL0;		//�@���x�N�g��
};

//�W�I���g���V�F�[�_�ւ̓��͍\����
struct GSPSInput
{
	float4 Position : SV_POSITION;  //�ʒu
	float4 Color : COLOR0;			//���_�J���[
	float3 Normal : NORMAL0;		//�@���x�N�g��
};


//���_�V�F�[�_
GSPSInput VSFunc(VSInput input)
{
	GSPSInput output = (GSPSInput)0;

	//���̓f�[�^�����̂܂ܗ���
	output.Position = float4(input.Position.xyz, 1.0f);
	output.Color = input.Color;
	output.Normal = input.Normal;

	return output;
}

//�W�I���g���V�F�[�_
[maxvertexcount(3)]
void GSFunc(triangle GSPSInput input[3], inout TriangleStream<GSPSInput> stream)
{
	float3 vecNormal = mul(mtrxWorld, input[0].Normal); //�@���x�N�g�������[���h���W�ɕϊ�
	float Dot = dot(vecNormal, vecLight); //�@���x�N�g���ƌ����֌����������x�N�g���Ƃ̓���
	float Light = Dot * 0.5f + 0.5f; //���ς��F�ɉe����^���閾�邳���v�Z

	for (int i = 0; i < 3; i++)
	{
		GSPSInput output = (GSPSInput)0;

		//�o�͒l�ݒ�
		output.Position = mul(mtrxWorld, input[i].Position);
		output.Position = mul(mtrxView, output.Position);
		output.Position = mul(mtrxProj, output.Position);

		output.Color = input[i].Color;
		output.Color.xyz = output.Color.xyz * Light; //RGB�����������Ƃ̊p�x�ɉ����Ē���

		//�X�g���[���ɒǉ�
		stream.Append(output);
	}

	stream.RestartStrip();
}

//�s�N�Z���V�F�[�_
float4 PSFunc(GSPSInput output) : SV_TARGET0
{
	return output.Color;
}

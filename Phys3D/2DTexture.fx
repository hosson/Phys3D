//2D�̃e�N�X�`����`�悷��V�F�[�_

//�e�N�X�`���ƃT���v��
Texture2D  tex0  : register(t0);
SamplerState sam0  : register(s0);

/////////////////////////////////////////////////////////////////////////////////////
// VSInput structure
/////////////////////////////////////////////////////////////////////////////////////
struct VSInput
{
	float4 Position : POSITION;     //!< �ʒu���W�ł�.
	float2 Texcoord : TEXCOORD;		//�e�N�X�`�����W
};

/////////////////////////////////////////////////////////////////////////////////////
// GSPSInput structure
/////////////////////////////////////////////////////////////////////////////////////
struct GSPSInput
{
	float4 Position : SV_POSITION;  //�ʒu���W�i���� SV_POSITION �́@float4 �ł��邱�Ƃ��K�{�j
	float2 Texcoord : TEXCOORD;		//�e�N�X�`�����W
};

//-----------------------------------------------------------------------------------
//! @brief      ���_�V�F�[�_�G���g���[�|�C���g�ł�.
//-----------------------------------------------------------------------------------
GSPSInput VSFunc(VSInput input)
{
	GSPSInput output = (GSPSInput)0;

	// ���̓f�[�^�����̂܂ܗ���.
	output.Position = input.Position;
	output.Texcoord = input.Texcoord;

	return output;
}

//------------------------------------------------------------------------------------
//! @brief      �s�N�Z���V�F�[�_�G���g���[�|�C���g�ł�.
//------------------------------------------------------------------------------------
float4 PSFunc(GSPSInput output) : SV_TARGET0
{
	return tex0.Sample(sam0, output.Texcoord);
}

/*=================================================
//								FILE : DepthOfField.hlsl
//		�t�@�C������ :
//      ��ʊE�[�x��ݒ肷��V�F�[�_�v���O����
//
//							HAL��� : ���{ �Y�V��
=================================================*/
Texture2D g_ColorTex : register(t0);        //���̃f�[�^
Texture2D g_GaussianTex : register(t1);     //�ڂ������e�N�X�`��
Texture2D g_NormalTex : register(t2);		//�@�����(w�ɐ[�x���)
SamplerState g_samLinear : register(s0);

cbuffer WorldMatrix : register(b0)
{
    matrix g_mWVP; //���[���h����ˉe�܂ł̕ϊ��s��
};

//��ʊE�[�x�̐ݒ���s���p�����[�^
cbuffer DoFData : register(b1)
{
    float2 vScreenPos; //�s���g�����킹��X�N���[�����W
    float vDoFRange;   //�s���g�{�P�͈̔́B�傫���Ƃ����{�P��
    float vBlur;              //�{�P�̑傫��
}

//�o�[�e�b�N�X�o�b�t�@�[�o�͍\����
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

VS_OUTPUT VS(float4 Pos : POSITION, float2 Tex : TEX)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Pos = mul(Pos, g_mWVP);
	//�e�N�X�`���[���W
    output.Tex = Tex;

    return output;
}

// TODO �����I�ɂ͒����_�̃��C���΂�����̂��̂Ƀs���g�������悤�ɂ���
float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 color = g_ColorTex.Sample(g_samLinear, input.Tex);
    float4 Blur = g_GaussianTex.Sample(g_samLinear, input.Tex);
    float4 Depth = g_NormalTex.Sample(g_samLinear, input.Tex).w; //���W
    float4 Origin = g_NormalTex.Sample(g_samLinear, vScreenPos).w; //�ǂ̈ʒu�Ƀs���g�����킹�邩�H(���݂͉�ʒ���)

    Depth -= Origin;        //�n�_�����킹��ʒu��0�ɂ��Đ��l��
    Depth *= vDoFRange; //�{�P��̒���(�A�v�������珑����������Ƃ���)

    Depth *= Depth;         //��悵�ĕ���������
    Depth *= vBlur; //�{�P��̒���(�A�v�������珑����������Ƃ���)
    Depth *= 0.25f;         //�Ō�̃{�P�̒����B�傫����΃{�P���傫���Ȃ�

    Depth = saturate(Depth);
	
    return lerp(color, Blur, Depth);
}
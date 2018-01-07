/*=================================================
//								FILE : RadialBlur.hlsl
//		�t�@�C������ :
//      �󂯎�����e�N�X�`���Ƀ��W�A���u���[���ʂ�t�^���ďo�͂���V�F�[�_
//
//							HAL��� : ���{ �Y�V��
=================================================*/
Texture2D g_Tex : register(t0); //���̃f�[�^
SamplerState g_samLinear : register(s0);

cbuffer WorldMatrix : register(b0)
{
    matrix g_mWVP; //���[���h����ˉe�܂ł̕ϊ��s��
};

cbuffer RadialBlurData : register(b1)
{
    float2 vBlurPos;        //�ڂ����N�_
    float vDelta;             //�ڂ����̑傫���B-0.3���炢�����傤�ǂ���
    float vBlurNum;              //�ڂ����񐔁B�����ƂȂ߂炩�ɂȂ�
};

//�o�[�e�b�N�X�o�b�t�@�[�o�͍\����
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

float4 RadialBlur(float2 Pos2d)
{
    float2 Dir = vBlurPos - Pos2d;
    float Len = length(Dir);

    float4 Color = g_Tex.Sample(g_samLinear, Pos2d);
    float4 Sum = Color;

    float Delta = vDelta;
    float add = (1.0 / vBlurNum);
    for (int i = 0; i < (int) vBlurNum; i++)
    {
        Sum += g_Tex.Sample(g_samLinear, Pos2d + Dir * Delta);
        Delta += add;
    }
    Sum *= 1.0 / (vBlurNum + 1.0);
	
    float t = saturate(Len) * 2;
    float4 FinalColor = lerp(Color, Sum, t);

    return FinalColor;
}

VS_OUTPUT VS(float4 Pos : POSITION, float2 Tex : TEX)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Pos = mul(Pos, g_mWVP);
	//�e�N�X�`���[���W
    output.Tex = Tex;

    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    return RadialBlur(input.Tex);

}
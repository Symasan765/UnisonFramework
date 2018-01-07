/*=================================================
//								FILE : Sprite2DDraw.hlsl
//		�t�@�C������ :
//      2D�X�v���C�g�̕`����s��HLSL
//
//							HAL��� : ���{ �Y�V��
=================================================*/
Texture2D g_texDecal : register(t0);
SamplerState g_samLinear : register(s0);

#include "PerlinNoise.hlsl"

cbuffer WorldMatrix : register(b0)
{
    matrix g_mWVP; //���[���h����ˉe�܂ł̕ϊ��s��
};

//�o�[�e�b�N�X�o�b�t�@�[�o�͍\����
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

VS_OUTPUT VS(float4 Pos : POSITION, float2 Tex : TEXCOORD)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Pos = mul(Pos, g_mWVP);
	//�e�N�X�`���[���W
    output.Tex = Tex;

    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 color = g_texDecal.Sample(g_samLinear, input.Tex);

	//�Z�s�A
    //float3 Gray = 0.299f * color.r + 0.587f * color.g + 0.114f * color.b;
    //color.xyz = Gray;

    //color.x = Gray * 240.0f / 255.0f;
    //color.y = Gray * 200.0f / 255.0f;
    //color.z = Gray * 148.0f / 255.0f;

    //float col = PerlinNoise(input.Tex, 1.0f / 8.0f, 1.0);
   
    //color.xyz = col;
    
    //if (color.x > 1.0f || color.x < -1.0f)
    //{
    //    color.x = 1.0f;

    //    color.yz = 0.0f;

    //}

        return color;

    }
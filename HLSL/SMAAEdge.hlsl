/*=================================================
//								FILE : SMAAEdge.hlsl
//		�t�@�C������ :
//		SMAA�ɂ�������i�K�̃G�b�W���o���s���V�F�[�_
//
//							HAL��� : ���{ �Y�V��
=================================================*/
// SMAA.h�ŕK�v��define��`

// �V�F�[�_���f���@DX11�Ȃ̂�4.1
#define SMAA_HLSL_4_1	1	

// �s�N�Z���T�C�Y�i��ʉ𑜓x�j
#define SMAA_PIXEL_SIZE float2(1.0/1280.0, 1.0/720.0)

// �i��
//#define SMAA_PRESET_LOW		1	//(%60 of the quality)
//#define SMAA_PRESET_MEDIUM	1	//(%80 of the quality)
#define SMAA_PRESET_HIGH	1	//(%95 of the quality)
//#define SMAA_PRESET_ULTRA	1	//(%99 of the quality)


#include "SMAA.h"

//SMAA 1x�̂ݑΉ�

Texture2D g_colorTex : register(t0); //�f�B�t���[�Y�e�N�X�`��
SamplerState g_samLinear : register(s0);

cbuffer WorldMatrix : register(b0) {
    matrix g_mWVP; //���[���h����ˉe�܂ł̕ϊ��s��
};

//----------------------------------------
// �G�b�W���o
void VS(
		float4 position : POSITION,
		out float4 svPosition : SV_POSITION,
		inout float2 texcoord : TEX,
		out float4 offset[3] : TEXCOORD1 )
{
	position = mul(position, g_mWVP);
	SMAAEdgeDetectionVS(position, svPosition, texcoord, offset);
}

float4 PS(
		float4 position : SV_POSITION,
		float2 texcoord : TEX,
		float4 offset[3] : TEXCOORD1) : SV_TARGET
{
	//float4 color = SMAAColorEdgeDetectionPS(texcoord, offset, g_colorTex);
	float4 color = SMAALumaEdgeDetectionPS(texcoord, offset, g_colorTex);
	color = 1.0f;
	return color;
}
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

Texture2D txEdge : register(t0);		//���p�X�ō쐬�����G�b�W�e�N�X�`��
Texture2D txArea : register(t1);		//���O�v�Z�e�N�X�`��
Texture2D txSearch : register(t2);	//���O�v�Z�e�N�X�`��

cbuffer WorldMatrix : register(b0) {
    matrix g_mWVP; //���[���h����ˉe�܂ł̕ϊ��s��
};

//----------------------------------------
// �G�b�W���o
void VS(
		float4 position : POSITION,
		out float4 svPosition : SV_POSITION,
		inout float2 texcoord : TEX,
		out float2 pixcoord : TEXCOORD1,
		out float4 offset[3] : TEXCOORD2 )
{
	position = mul(position, g_mWVP);
	SMAABlendingWeightCalculationVS(position, svPosition, texcoord, pixcoord, offset);
}

float4 PS(
		float4 position : SV_POSITION,
		float2 texcoord : TEX,
		float2 pixcoord : TEXCOORD1,
		float4 offset[3] : TEXCOORD2) : SV_TARGET
{
	int4 subsampleIndices = {0, 0, 0, 0};
	float4 color = 1.0f;
	color =  SMAABlendingWeightCalculationPS(texcoord, pixcoord, offset, txEdge, txArea, txSearch, subsampleIndices);
	color.a = 1.0f;
    return color;
}
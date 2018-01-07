/*=================================================
//								FILE : SMAAEdge.hlsl
//		ファイル説明 :
//		SMAAにおける第一段階のエッジ抽出を行うシェーダ
//
//							HAL大阪 : 松本 雄之介
=================================================*/
// SMAA.hで必要なdefine定義

// シェーダモデル　DX11なので4.1
#define SMAA_HLSL_4_1	1	

// ピクセルサイズ（画面解像度）
#define SMAA_PIXEL_SIZE float2(1.0/1280.0, 1.0/720.0)

// 品質
//#define SMAA_PRESET_LOW		1	//(%60 of the quality)
//#define SMAA_PRESET_MEDIUM	1	//(%80 of the quality)
#define SMAA_PRESET_HIGH	1	//(%95 of the quality)
//#define SMAA_PRESET_ULTRA	1	//(%99 of the quality)


#include "SMAA.h"

//SMAA 1xのみ対応

Texture2D txEdge : register(t0);		//第一パスで作成したエッジテクスチャ
Texture2D txArea : register(t1);		//事前計算テクスチャ
Texture2D txSearch : register(t2);	//事前計算テクスチャ

cbuffer WorldMatrix : register(b0) {
    matrix g_mWVP; //ワールドから射影までの変換行列
};

//----------------------------------------
// エッジ検出
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
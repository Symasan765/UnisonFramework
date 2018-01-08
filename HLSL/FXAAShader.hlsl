#define FXAA_PC 1
#define FXAA_HLSL_5 1
//#define FXAA_QUALITY__PRESET 12
#include "FXAA.h"

#define    PIXEL_SIZE float2(1.0/1280.0, 1.0/720.0)
#define    SUBPIX 1.0f
#define    EDGE_THRESHOLD 0.063f
#define    EDGE_THRESHOLD_MIN 0.0312f

Texture2D g_texDecal : register(t0);
SamplerState g_samLinear : register(s0);


cbuffer WorldMatrix : register(b0)
{
    matrix g_mWVP; //ワールドから射影までの変換行列
};

//バーテックスバッファー出力構造体
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

VS_OUTPUT VS(float4 Pos : POSITION, float2 Tex : TEX)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Pos = mul(Pos, g_mWVP);
	//テクスチャー座標
    output.Tex = Tex;

    return output;
}


float4 PS(VS_OUTPUT In) : SV_TARGET0
{
    float2 uv = In.Tex;
    FxaaTex InputFXAATex = { g_samLinear, g_texDecal };
    float4 color = FxaaPixelShader(
        uv,                         // FxaaFloat2 pos,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),     // FxaaFloat4 fxaaConsolePosPos,
        InputFXAATex,                   // FxaaTex tex,
        InputFXAATex,                   // FxaaTex fxaaConsole360TexExpBiasNegOne,
        InputFXAATex,                   // FxaaTex fxaaConsole360TexExpBiasNegTwo,
        PIXEL_SIZE,                 // FxaaFloat2 fxaaQualityRcpFrame,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),     // FxaaFloat4 fxaaConsoleRcpFrameOpt,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),     // FxaaFloat4 fxaaConsoleRcpFrameOpt2,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),     // FxaaFloat4 fxaaConsole360RcpFrameOpt2,
        SUBPIX,                     // FxaaFloat fxaaQualitySubpix,
        EDGE_THRESHOLD,             // FxaaFloat fxaaQualityEdgeThreshold,
        EDGE_THRESHOLD_MIN,         // FxaaFloat fxaaQualityEdgeThresholdMin,
        0.0f,                          // FxaaFloat fxaaConsoleEdgeSharpness,
        0.0f,                          // FxaaFloat fxaaConsoleEdgeThreshold,
        0.0f,                          // FxaaFloat fxaaConsoleEdgeThresholdMin,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f)      // FxaaFloat fxaaConsole360ConstDir,
    );
	color.a = 1.0f;
	return color;
}
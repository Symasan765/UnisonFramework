/*=================================================
//								FILE : GaussianFilterX.hlsl
//		ファイル説明 :
//      ガウスフィルターの横方向のぼかしを行うシェーダ
//
//							HAL大阪 : 松本 雄之介
=================================================*/
//ガウスフィルタを作成するシェーダ
Texture2D g_Tex : register(t0);
SamplerState g_Sampler : register(s0);

cbuffer WorldMatrix : register(b0)
{
    matrix g_mWVP; //ワールドから射影までの変換行列
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord0 : TEXCOORD0;
    float2 TexCoord1 : TEXCOORD1;
    float2 TexCoord2 : TEXCOORD2;
    float2 TexCoord3 : TEXCOORD3;
    float2 TexCoord4 : TEXCOORD4;
    float2 TexCoord5 : TEXCOORD5;
    float2 TexCoord6 : TEXCOORD6;
    float2 TexCoord7 : TEXCOORD7;
};


VS_OUTPUT VS(float4 Pos : POSITION, float2 Tex : TEX)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;

    output.Position = mul(Pos, g_mWVP);

    //width == 1280.0fにて計算
    output.TexCoord0 = Tex + float2(-0.00078125f,0.0f);  //-1.0f / width
    output.TexCoord1 = Tex + float2(-0.00234375f, 0.0f); //-3.0f / width
    output.TexCoord2 = Tex + float2(-0.00390625f, 0.0f); //-5.0f / width
    output.TexCoord3 = Tex + float2(-0.00546875f, 0.0f); //-7.0f / width
    output.TexCoord4 = Tex + float2(-0.00703125f, 0.0f); //-9.0f / width
    output.TexCoord5 = Tex + float2(-0.00859375f, 0.0f); //-11.0f / width
    output.TexCoord6 = Tex + float2(-0.01015625f, 0.0f); //-13.0f / width
    output.TexCoord7 = Tex + float2(-0.01171875f, 0.0f); //-15.0f / width

    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float4 output = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float OffsetX = 0.0125f; //16.0f / width

    //ガウス関数の結果を計算せずに配列に入れておくことで処理負荷を軽減する
    float Weights[8] =
    {
        0.156617f, 0.133460f, 0.096912f, 0.059968f, 0.031620f, 0.014208f, 0.005440f, 0.001775f
    };

    output += Weights[0] * (g_Tex.Sample(g_Sampler, input.TexCoord0) + g_Tex.Sample(g_Sampler, input.TexCoord7 + float2(OffsetX , 0.0f)));
    output += Weights[1] * (g_Tex.Sample(g_Sampler, input.TexCoord1) + g_Tex.Sample(g_Sampler, input.TexCoord6 + float2(OffsetX, 0.0f)));
    output += Weights[2] * (g_Tex.Sample(g_Sampler, input.TexCoord2) + g_Tex.Sample(g_Sampler, input.TexCoord5 + float2(OffsetX, 0.0f)));
    output += Weights[3] * (g_Tex.Sample(g_Sampler, input.TexCoord3) + g_Tex.Sample(g_Sampler, input.TexCoord4 + float2(OffsetX, 0.0f)));
   
    output += Weights[4] * (g_Tex.Sample(g_Sampler, input.TexCoord4) + g_Tex.Sample(g_Sampler, input.TexCoord3 + float2(OffsetX, 0.0f)));
    output += Weights[5] * (g_Tex.Sample(g_Sampler, input.TexCoord5) + g_Tex.Sample(g_Sampler, input.TexCoord2 + float2(OffsetX, 0.0f)));
    output += Weights[6] * (g_Tex.Sample(g_Sampler, input.TexCoord6) + g_Tex.Sample(g_Sampler, input.TexCoord1 + float2(OffsetX, 0.0f)));
    output += Weights[7] * (g_Tex.Sample(g_Sampler, input.TexCoord7) + g_Tex.Sample(g_Sampler, input.TexCoord0 + float2(OffsetX, 0.0f)));
  
    return output;
}
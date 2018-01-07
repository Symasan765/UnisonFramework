/*=================================================
//								FILE : DeferredBaseLighting.hlsl
//		ファイル説明 :
//      ディファードレンダリングで利用するライティング処理を行うシェーダ
//
//							HAL大阪 : 松本 雄之介
=================================================*/
Texture2D g_colorTex : register(t0); //ディフューズテクスチャ
Texture2D g_shadowTex : register(t1); //シャドウマップ
Texture2D g_positionTex : register(t2);
SamplerState g_samLinear : register(s0);

//指数フォグ
float4 ExpFog(float4 diffuse, float depth, float density, float4 fogColor)
{
    float d = depth; //距離
    float e = 2.71828;
    float f = pow(e, -d * density); //フォグファクター
    f = saturate(f);
    float4 C = f * diffuse + (1 - f) * fogColor;
    return C;
}

cbuffer WorldMatrix : register(b0)
{
    matrix g_mWVP; //ワールドから射影までの変換行列
};

cbuffer FogData : register(b1)
{
    float4 vFogColor; //フォグのカラー
    float vFogFactor; //フォグの強さを調整する
    int ShadowFlag; //シャドウを適用するならtrue,使わないならfalse
}

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

float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 color = g_colorTex.Sample(g_samLinear, input.Tex); //まずは元の色を割り出す
    float4 Pos = g_positionTex.Sample(g_samLinear, input.Tex); //座標

    if (ShadowFlag)
        color *= g_shadowTex.Sample(g_samLinear, input.Tex); //最後にシャドウの合成を行う

    color = ExpFog(color, Pos.w, vFogFactor, vFogColor);
    
    return color;
}
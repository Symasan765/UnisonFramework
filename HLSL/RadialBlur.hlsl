/*=================================================
//								FILE : RadialBlur.hlsl
//		ファイル説明 :
//      受け取ったテクスチャにラジアルブラー効果を付与して出力するシェーダ
//
//							HAL大阪 : 松本 雄之介
=================================================*/
Texture2D g_Tex : register(t0); //元のデータ
SamplerState g_samLinear : register(s0);

cbuffer WorldMatrix : register(b0)
{
    matrix g_mWVP; //ワールドから射影までの変換行列
};

cbuffer RadialBlurData : register(b1)
{
    float2 vBlurPos;        //ぼかし起点
    float vDelta;             //ぼかしの大きさ。-0.3くらいがちょうどいい
    float vBlurNum;              //ぼかし回数。多いとなめらかになる
};

//バーテックスバッファー出力構造体
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
	//テクスチャー座標
    output.Tex = Tex;

    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    return RadialBlur(input.Tex);

}
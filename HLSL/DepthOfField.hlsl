/*=================================================
//								FILE : DepthOfField.hlsl
//		ファイル説明 :
//      被写界深度を設定するシェーダプログラム
//
//							HAL大阪 : 松本 雄之介
=================================================*/
Texture2D g_ColorTex : register(t0);        //元のデータ
Texture2D g_GaussianTex : register(t1);     //ぼかしたテクスチャ
Texture2D g_NormalTex : register(t2);		//法線情報(wに深度情報)
SamplerState g_samLinear : register(s0);

cbuffer WorldMatrix : register(b0)
{
    matrix g_mWVP; //ワールドから射影までの変換行列
};

//被写界深度の設定を行うパラメータ
cbuffer DoFData : register(b1)
{
    float2 vScreenPos; //ピントを合わせるスクリーン座標
    float vDoFRange;   //ピントボケの範囲。大きいとすぐボケる
    float vBlur;              //ボケの大きさ
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

// TODO 将来的には注視点のレイを飛ばした先のものにピントがあうようにする
float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 color = g_ColorTex.Sample(g_samLinear, input.Tex);
    float4 Blur = g_GaussianTex.Sample(g_samLinear, input.Tex);
    float4 Depth = g_NormalTex.Sample(g_samLinear, input.Tex).w; //座標
    float4 Origin = g_NormalTex.Sample(g_samLinear, vScreenPos).w; //どの位置にピントを合わせるか？(現在は画面中央)

    Depth -= Origin;        //始点を合わせる位置を0にして数値化
    Depth *= vDoFRange; //ボケ具合の調整(アプリ側から書き換えられるといい)

    Depth *= Depth;         //二乗して符号を消す
    Depth *= vBlur; //ボケ具合の調整(アプリ側から書き換えられるといい)
    Depth *= 0.25f;         //最後のボケの調整。大きければボケも大きくなる

    Depth = saturate(Depth);
	
    return lerp(color, Blur, Depth);
}
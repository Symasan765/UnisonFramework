/*=================================================
//								FILE : GaussBlur.hlsl
//		ファイル説明 :
//		1パスにてガウスぼかしを行うシェーダ
//
//							HAL大阪 : 松本 雄之介
=================================================*/
Texture2D g_colorTex : register(t0); //ディフューズテクスチャ
SamplerState g_Sampler : register(s0);

//処理不可軽減のためpixel数1280.0f , 720.0f / 2.0f のサイズのガウスをあらかじめ計算
static const float avSampleWeights[13] = {
		0.024882,
		0.067638,
		0.111515,
		0.067638,
		0.024882,
		0.111515,
		0.183858,
		0.111515,
		0.024882,
		0.067638,
		0.111515,
		0.067638,
		0.024882
	};

static const float2 avSampleOffsets[13] = {
		{-0.003125f,0.0f},
		{-0.001563,-0.002778f},
		{-0.001563,0.0f},
		{-0.001563,0.002778f},
		{0.0f,-0.005556f},
		{0.0f,-0.002778f},
		{0.0f,0.0f},
		{0.0f,0.002778f},
		{0.0f,0.005556f},
		{0.001563f,-0.002778f},
		{0.001563f,0.0f},
		{0.001563f,0.002778f},
		{0.003125f,0.0f}
	};

cbuffer WorldMatrix : register(b0) {
    matrix g_mWVP; //ワールドから射影までの変換行列
};

//バーテックスバッファー出力構造体
struct VS_OUTPUT {
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
	float4 sample = 0.0f;

	for( int i=0; i < 13; i++ ) {
		sample += avSampleWeights[i]
					 * g_colorTex.Sample(g_Sampler, input.Tex + avSampleOffsets[i]) ;
	}
	
	return sample;
}
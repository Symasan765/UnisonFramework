/*=================================================
//								FILE : CelShading.hlsl
//		ファイル説明 :
//		トゥーンレンダリングのポストエフェクト処理を行う
//
//							HAL大阪 : 松本 雄之介
=================================================*/
Texture2D g_sssssTex : register(t0); //ディフューズテクスチャ
Texture2D g_depthTex : register(t1); //深度マップ
Texture2D g_diffuseTex : register(t2); //全体カラー情報

SamplerState g_samLinear : register(s0);
SamplerState g_samPoint : register(s1);

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

float4 BlurPS(float2 texcoord, uniform float2 step) : SV_TARGET {
	float correction = 5.0f;

	//現在のピクセルの周りの6つのサンプルのガウス重み
	// -3 -2 -1 +1 +2 +3
    float w[6] = { 0.006,   0.061,   0.242,  0.242,  0.061, 0.006 };
    float o[6] = {  -1.0, -0.6667, -0.3333, 0.3333, 0.6667,   1.0 };
	
	//現在のピクセルの色と深度
    float4 colorM = g_sssssTex.Sample(g_samPoint, texcoord);
    float depthM = g_depthTex.Sample(g_samPoint, texcoord).w;
	
	//中心サンプルにガウス重みを掛けて累積する
    float4 colorBlurred = colorM;
    colorBlurred.rgb *= 0.382;
	
	//周囲のピクセルをフェッチするために利用するステップを計算し、
	//"step"は次の通り
	//step = SSSStrength * gausssianEidth * pixelSize * dir
	//ピクセルが近いほど効果が強くなければならないため、因子1.0 / 深さM
    float2 finalStep = colorM.a * step / depthM;
	
	//他のサンプルを累積する
    [unroll]
    for (int i = 0; i < 6; i++) {
		// 現在の色と深度を取得する
        float2 offset = texcoord + o[i] * finalStep;
        float3 color = g_sssssTex.SampleLevel(g_samLinear, offset, 0).rgb;
        float depth = g_depthTex.SampleLevel(g_samPoint, offset, 0).w;
		
		//深度の差が大きい場合はcolorMを「colorM」に戻します
        float s = min(0.0125 * correction * abs(depthM - depth), 1.0);
        color = lerp(color, colorM.rgb, s);
		
		//累積
        colorBlurred.rgb += w[i] * color;
    }
	
    return colorBlurred;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
	//SSSSSフラグがあれば処理する
	// TODO SSSSSのパラメータ化を行う
	float data = 0.5f;
	if(g_sssssTex.Sample(g_samPoint, input.Tex).w > 0.5f){
		float2 dir = data * data * (float2)(1.0f / 1280.0f,1.0f / 720.0f) * data;
		return BlurPS(input.Tex,dir);
	}

	//なければ普通にカラーを返す
    return g_diffuseTex.Sample(g_samPoint, input.Tex);
}
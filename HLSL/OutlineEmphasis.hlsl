/*=================================================
//								FILE : OutlineEmphasis.hlsl
//		ファイル説明 :
//		法線情報と深度情報を元に輪郭線の強調を行う
//		輪郭線は完全な黒ではなく、周辺色と馴染むように処理している
//							HAL大阪 : 松本 雄之介
=================================================*/
Texture2D g_colorTex : register(t0); //ディフューズテクスチャ
Texture2D g_NormalTex : register(t1); //法線
SamplerState g_samLinear : register(s0);

//トゥーンレンダリングの輪郭線抽出関数に渡すデータ
struct ToonEdge {
    float4 Color; //輪郭線でなければこの色を返す色
    float2 Tex; //求めたいテクスチャ座標
    float offsetScall; //解像度で割る値。数値が大きいとそれだけ多くのテクセルをまたいで判定を行うので線が太くなる
    float DepthRange; //輪郭線として取り扱うサイズ。線の太さではなく有効範囲の拡大
    float NormalRange; //輪郭線として取り扱うサイズ。線の太さではなく有効範囲の拡大
    Texture2D normTex; //法線テクスチャ
    SamplerState samp; //サンプラー
};

cbuffer WorldMatrix : register(b0) {
    matrix g_mWVP; //ワールドから射影までの変換行列
};

cbuffer OutlineParam : register(b1) {
    float g_offsetScall;
	float g_DepthRange;
	float g_NormalRange;
	float g_OutlineSum;
};

//トゥーンレンダリングでの輪郭線抽出を法線情報と深度情報を用いて判定を行う関数
float4 ToonEdgeWithNormalAndDepth(ToonEdge Data)
{
    float2 offsetCoord[9] =
    {
        {-1.0, -1.},
        {-1.0,0.0},
        {-1.0, 1.0},
        { 0.0, -1.0},
        {0.0, 0.0},
        {0.0, 1.0},
        {1.0, -1.0},
        {1.0, 0.0},
        {1.0, 1.0}
    };

// ラプラシアンカーネル
    float weight[] =
    {
        -1.0, -1.0, -1.0,
		-1.0, 8.0, -1.0,
		-1.0, -1.0, -1.0
    };

    float2 offsetScale = (float2) (Data.offsetScall / 1280.0f, Data.offsetScall / 720.0f);
    float4 destColor = Data.Color;
    float3 normalColor = (float3) (0.0);
    float3 tmpColor = (float3) (1.0);
    float depthEdge = 0.0;
    float normalEdge = 0.0;
	float4 colorData = 0.0f;
	colorData.w = 1.0f;

	//求める座標を中心に周囲９マスの判定を行う
    for (int i = 0; i < 9; ++i)
    {
        float2 offset = Data.Tex + offsetCoord[i] * offsetScale;
		float4 NormalTexData = Data.normTex.Sample(Data.samp, clamp(offset, 0.0f, 1.0f));
        depthEdge += (NormalTexData.w + 1.0f) / 2.0f * weight[i];
        normalColor += (NormalTexData.rgb + 1.0f) / 2.0f * weight[i];
		colorData.xyz += g_colorTex.Sample(Data.samp, clamp(offset, 0.0f, 1.0f));
    }

	//まず法線が範囲以上かチェックする
    normalEdge = dot(abs(normalColor), tmpColor) / 3.0f;
    if(normalEdge < Data.NormalRange)
        normalEdge = 0.0f;

	//その後で深度と法線のどちらが大きいか確認し、最終的な判断をする
    float edge = 1.0f;		//輪郭線ではない前提。
    if(max(abs(depthEdge), normalEdge)  > Data.DepthRange)
    {
        destColor.xyz = (colorData.xyz / 9.0f) * (g_OutlineSum);	//色を少し暗くする
    }

    return destColor;
}

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
    ToonEdge toonData;
    toonData.Color = g_colorTex.Sample(g_samLinear, input.Tex); //輪郭線でなければこの色を返す色
    toonData.Tex = input.Tex; //求めたいテクスチャ座標
    toonData.offsetScall = g_offsetScall * g_NormalTex.Sample(g_samLinear, input.Tex); //解像度で割る値。数値が大きいとそれだけ多くのテクセルをまたいで判定を行うので線が太くなる
    toonData.DepthRange = g_DepthRange; //輪郭線として取り扱うサイズ。線の太さではなく有効範囲の拡大
    toonData.normTex = g_NormalTex; //法線テクスチャ
    toonData.samp = g_samLinear; //サンプラー
    toonData.NormalRange = g_NormalRange;

    return ToonEdgeWithNormalAndDepth(toonData);
}
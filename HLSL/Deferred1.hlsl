/*=================================================
//								FILE : Deferred1.hlsl
//		ファイル説明 :
//      ディファードレンダリングにおいてG-Bufferを作成するための
//      基本的なシェーダ
//							HAL大阪 : 松本 雄之介
=================================================*/
Texture2D g_tex : register(t0);
Texture2D g_LightingShadowTex : register(t1);
Texture2D g_LightingHighTex : register(t2);
Texture2D g_MaterialMaskTex : register(t3);

Texture2D g_shadowTex : register(t4);           //シャドウマップを作成するために使用する
SamplerState g_samLinear : register(s0);

#include "BoneCalculation.hlsl"

//頂点シェーダ
INPUT_RAW_DATA VS(
	float4 Pos : POSITION, // ローカル位置座標
	float4 Normal : NORMAL, // 法線ベクトル
	float2 Tex : TEX, //テクスチャ座標
    uint4 Bones : BONE_INDEX, //ボーンのインデックス
    float4 Weights : BONE_WEIGHT //ボーンの重み
	)
{
    INPUT_RAW_DATA output = (INPUT_RAW_DATA) 0;

	//スタティックメッシュの場合、ボーン計算を無視する
    if (Bones.x != STATIC_MESH)
    {
         //ボーンを適用したローカル座標に更新する
        Skin skinPos = SkinVert(Pos, Normal, Bones, Weights);
        Pos = skinPos.Pos;
        Normal = skinPos.Normal;
    }

    output.Pos = Pos;
    output.Normal = Normal;
    output.Tex = Tex;
    output.Bones = Bones;
    output.Weights = Weights;

    return output;
}

// ジオメトリ シェーダの関数(特になにもしてないけど一応入れてる)
[maxvertexcount(3)]
void GS(triangle INPUT_RAW_DATA input[3],
	inout TriangleStream<PS_INPUT> TriStream)
{
    PS_INPUT output = (PS_INPUT) 0;

    for (int i = 0; i < 3; ++i)
    {
        output.Pos = mul(input[i].Pos, mWVP);
        output.Normal = mul(input[i].Normal, mW);
        output.UV = input[i].Tex;
        float4 lightVewPos = mul(input[i].Pos, mWLP); //ライト位置での座用を出して…
        output.LightDepth = lightVewPos.z; //その深度値を求める。(深度値の線形補間行列をアプリ側でかけてるのでZをそのまま代入)
        output.LightUV = mul(input[i].Pos, mWLPB); //この点が、ライトビューであったときの位置がわかる
        output.WorldPos = mul(input[i].Pos, mW);
        output.Depth = mul(input[i].Pos, mDepthWVP);

		// 出力
        TriStream.Append(output);
    }
    TriStream.RestartStrip();
}

float4 CookTorrance(float3 normal,float3 wPos,float3 Eye){
	float3 L = -LightDir.xyz; // ライトベクトル
    float3 N = normalize(normal); // 法線ベクトル
    float3 V = normalize(Eye - wPos); // 視線ベクトル
    float3 H = normalize(L + V); // ハーフベクトル

	// 計算に使うそれぞれの角度
    float NV = dot(N, V);
    float NH = dot(N, H);
    float VH = dot(V, H);
    float NL = dot(N, L);
    float LH = dot(L, H);

	// Beckmann 分布関数
    const float m = 0.3f; // 荒さ(初期0.3)
    float NH2 = NH * NH;
    float D = exp(-(1 - NH2) / (NH2 * m * m)) / (4 * m * m * NH2 * NH2);

	// 幾何減衰率
    float G = min(1, min(2 * NH * NV / VH, 2 * NH * NL / VH));

	// フレネル
    float n = 6.5f; //						 反射する光の量的な感じ(初期6.5)
    float g = sqrt(n * n + LH * LH - 1);
    float gpc = g + LH;
    float gnc = g - LH;
    float cgpc = LH * gpc - 1;
    float cgnc = LH * gnc + 1;
    float F = 0.2f * gnc * gnc * (1 + cgpc * cgpc / (cgnc * cgnc)) / (gpc * gpc); //この先頭の定数で反射の大きさ変わる

	// 金属の色
    float4 ks = { 2.0f * 0.65f, 2.0f * 0.65f, 2.0f * 0.65f, 1.0f }; //ここで色変わる

    return ks * max(0, F * D * G / NV); // 鏡面反射光
}

//ピクセルシェーダ
PS_OUTPUT PS(PS_INPUT In)
{
    PS_OUTPUT Out = (PS_OUTPUT) 0;

    Out.vPosition = In.WorldPos;                   //座標
    Out.vNormal = normalize(In.Normal);                         //法線
	Out.vNormal.w = In.Depth.z;
	Out.vScreenSpaceSSS = g_MaterialMaskTex.Sample(g_samLinear, In.UV).r;  //SSSSS

    //深度値の比較のためにテクスチャから情報取得
	// TODO 影内判定を入れる
    //In.LightUV /= In.LightUV.w;
    //float TexValue = g_shadowTex.Sample(g_samLinear, In.LightUV).r;     //ライト位置から見た深度
    //Out.vShadow = (TexValue + 0.0001f < In.LightDepth) ? 0.6f : 1.0f; //比較して代入

	//カラー計算
	Out.vColor = g_tex.Sample(g_samLinear, In.UV);  //描画色
	Out.vBright = Emission; //自発光色

	//金属面の処理
	if(g_MaterialMaskTex.Sample(g_samLinear, In.UV).g >= 0.5f){
		Out.vColor += CookTorrance(In.Normal.xyz,In.WorldPos.xyz,CameraPos.xyz);
		Out.vColor.w = 1.0f;
		//高輝度部抽出
		Out.vBright += Out.vColor;
		Out.vBright -= 1.5f;		//ここを強めると高輝度部が狭まる
		Out.vBright *= 30.0f;		//ここを強めるとはっきりとした色になる
		return Out;
	}

	//試しに照明計算をしてみる
	float3 N = In.Normal.xyz;
	float3 L = -LightDir.xyz;
	float3 eye = normalize(CameraPos.xyz - In.WorldPos.xyz);
	float3 H = normalize(L + eye);

	//ランバートで照明
	 float deif = dot(In.Normal.xyz, -LightDir.xyz);
	 deif = deif * 0.5f + 0.5;
	 deif = deif * deif;

	//ブリンフォンで照明
	//float deif = max(0.0f,dot(In.Normal.xyz,-LightDir.xyz)) + pow(max(0,dot(N,H)),10);

	// TODO 照明値をパラメータ化すること
	if(deif >= 0.90f)
		Out.vColor = g_LightingHighTex.Sample(g_samLinear, In.UV);  //ハイライト
	else if(deif >= 0.3f)
		Out.vColor = g_tex.Sample(g_samLinear, In.UV);  //描画色
	else
		Out.vColor = g_LightingShadowTex.Sample(g_samLinear, In.UV);  //シャドウ部

	//スクリーンスペースサブサーフェススキャッタリングのディフューズ情報を入れる。wにはマスクを入れたのでこれでSSSSS部のディフューズだけ取り出せる
	Out.vScreenSpaceSSS.xyz = Out.vScreenSpaceSSS.w * Out.vColor.xyz;
   
    return Out;
}
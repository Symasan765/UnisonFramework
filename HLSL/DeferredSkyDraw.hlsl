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
        output.Normal = 0.0f;
        output.UV = input[i].Tex;
        output.LightDepth = 0.0f;
        output.LightUV = 0.0f;
        output.WorldPos = 0.0f;
        output.Depth = 0.0f;

		// 出力
        TriStream.Append(output);
    }
    TriStream.RestartStrip();
}


//ピクセルシェーダ
PS_OUTPUT PS(PS_INPUT In)
{
    PS_OUTPUT Out = (PS_OUTPUT) 0;

    Out.vPosition = 0.0f;                   //座標
    Out.vNormal = 0.0f;
	Out.vScreenSpaceSSS = 0.0f;

    //深度値の比較のためにテクスチャから情報取得
	// TODO 影内判定を入れる
    //In.LightUV /= In.LightUV.w;
    //float TexValue = g_shadowTex.Sample(g_samLinear, In.LightUV).r;     //ライト位置から見た深度
    //Out.vShadow = (TexValue + 0.0001f < In.LightDepth) ? 0.6f : 1.0f; //比較して代入

	//カラー計算
	Out.vColor = g_tex.Sample(g_samLinear, In.UV);  //描画色
	Out.vBright = 0.0f; //自発光色

    return Out;
}
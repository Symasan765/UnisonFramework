/*=================================================
//								FILE : ShadowMap.hlsl
//		ファイル説明 :
//      深度マップの作成を行うシェーダa
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "BoneCalculation.hlsl"

struct PS_SHADOW_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Depth : TEXCOORD0;
};


INPUT_RAW_DATA VS(
    float4 Pos : POSITION, // ローカル位置座標
	float4 Normal : NORMAL, // 法線ベクトル   使わないけど他との汎用性のために残す
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
	inout TriangleStream<PS_SHADOW_INPUT> TriStream)
{
    PS_SHADOW_INPUT output = (PS_SHADOW_INPUT) 0;

    for (int i = 0; i < 3; ++i)
    {
        output.Pos = mul(input[i].Pos, mWLP);
        output.Depth = output.Pos;
		// 出力
        TriStream.Append(output);
    }
    TriStream.RestartStrip();
}

float4 PS(PS_SHADOW_INPUT In) : SV_Target
{
    return In.Depth.z / In.Depth.w;
}
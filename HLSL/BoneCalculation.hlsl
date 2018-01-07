/*=================================================
//								FILE : BoneCalculation
//		ファイル説明 :
//
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "ShaderCBuffer.hlsl"
#include "ShaderStruct.hlsl"

//ボーン情報を元にしてローカル座標を修正する関数
Skin SkinVert(float4 Pos, float4 Normal, uint4 Bones, float4 Weights)
{
    Skin output = (Skin) 0;
    //ボーン0
    uint iBone = Bones.x;
    float fWeight = Weights.x;
    matrix m = g_mConstBoneWorld[iBone];
    output.Pos += fWeight * mul(Pos, m);
    output.Normal += fWeight * mul(Normal, m);
         //ボーン1
    iBone = Bones.y;
    fWeight = Weights.y;
    m = g_mConstBoneWorld[iBone];
    output.Pos += fWeight * mul(Pos, m);
    output.Normal += fWeight * mul(Normal, m);
        //ボーン2
    iBone = Bones.z;
    fWeight = Weights.z;
    m = g_mConstBoneWorld[iBone];
    output.Pos += fWeight * mul(Pos, m);
    output.Normal += fWeight * mul(Normal, m);
        //ボーン3
    iBone = Bones.w;
    fWeight = Weights.w;
    m = g_mConstBoneWorld[iBone];
    output.Pos += fWeight * mul(Pos, m);
    output.Normal += fWeight * mul(Normal, m);

    return output;
}
/*=================================================
//								FILE : BoneCalculation
//		�t�@�C������ :
//
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "ShaderCBuffer.hlsl"
#include "ShaderStruct.hlsl"

//�{�[���������ɂ��ă��[�J�����W���C������֐�
Skin SkinVert(float4 Pos, float4 Normal, uint4 Bones, float4 Weights)
{
    Skin output = (Skin) 0;
    //�{�[��0
    uint iBone = Bones.x;
    float fWeight = Weights.x;
    matrix m = g_mConstBoneWorld[iBone];
    output.Pos += fWeight * mul(Pos, m);
    output.Normal += fWeight * mul(Normal, m);
         //�{�[��1
    iBone = Bones.y;
    fWeight = Weights.y;
    m = g_mConstBoneWorld[iBone];
    output.Pos += fWeight * mul(Pos, m);
    output.Normal += fWeight * mul(Normal, m);
        //�{�[��2
    iBone = Bones.z;
    fWeight = Weights.z;
    m = g_mConstBoneWorld[iBone];
    output.Pos += fWeight * mul(Pos, m);
    output.Normal += fWeight * mul(Normal, m);
        //�{�[��3
    iBone = Bones.w;
    fWeight = Weights.w;
    m = g_mConstBoneWorld[iBone];
    output.Pos += fWeight * mul(Pos, m);
    output.Normal += fWeight * mul(Normal, m);

    return output;
}
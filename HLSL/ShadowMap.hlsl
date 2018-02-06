/*=================================================
//								FILE : ShadowMap.hlsl
//		�t�@�C������ :
//      �[�x�}�b�v�̍쐬���s���V�F�[�_a
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "BoneCalculation.hlsl"

struct PS_SHADOW_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Depth : TEXCOORD0;
};


INPUT_RAW_DATA VS(
    float4 Pos : POSITION, // ���[�J���ʒu���W
	float4 Normal : NORMAL, // �@���x�N�g��   �g��Ȃ����Ǒ��Ƃ̔ėp���̂��߂Ɏc��
	float2 Tex : TEX, //�e�N�X�`�����W
    uint4 Bones : BONE_INDEX, //�{�[���̃C���f�b�N�X
    float4 Weights : BONE_WEIGHT //�{�[���̏d��
)
{
    INPUT_RAW_DATA output = (INPUT_RAW_DATA) 0;

	//�X�^�e�B�b�N���b�V���̏ꍇ�A�{�[���v�Z�𖳎�����
    if (Bones.x != STATIC_MESH)
    {
         //�{�[����K�p�������[�J�����W�ɍX�V����
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

// �W�I���g�� �V�F�[�_�̊֐�(���ɂȂɂ����ĂȂ����ǈꉞ����Ă�)
[maxvertexcount(3)]
void GS(triangle INPUT_RAW_DATA input[3],
	inout TriangleStream<PS_SHADOW_INPUT> TriStream)
{
    PS_SHADOW_INPUT output = (PS_SHADOW_INPUT) 0;

    for (int i = 0; i < 3; ++i)
    {
        output.Pos = mul(input[i].Pos, mWLP);
        output.Depth = output.Pos;
		// �o��
        TriStream.Append(output);
    }
    TriStream.RestartStrip();
}

float4 PS(PS_SHADOW_INPUT In) : SV_Target
{
    return In.Depth.z / In.Depth.w;
}
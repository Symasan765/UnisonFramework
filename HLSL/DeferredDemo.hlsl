/*=================================================
//								FILE : Deferred1.hlsl
//		�t�@�C������ :
//      �f�B�t�@�[�h�����_�����O�ɂ�����G-Buffer���쐬���邽�߂�
//      ��{�I�ȃV�F�[�_
//							HAL��� : ���{ �Y�V��
=================================================*/
Texture2D g_tex : register(t0);
Texture2D g_LightingShadowTex : register(t1);
Texture2D g_LightingHighTex : register(t2);
Texture2D g_MaterialMaskTex : register(t3);

Texture2D g_shadowTex : register(t4);           //�V���h�E�}�b�v���쐬���邽�߂Ɏg�p����
SamplerState g_samLinear : register(s0);

#include "BoneCalculation.hlsl"

//���_�V�F�[�_
INPUT_RAW_DATA VS(
	float4 Pos : POSITION, // ���[�J���ʒu���W
	float4 Normal : NORMAL, // �@���x�N�g��
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
	inout TriangleStream<PS_INPUT> TriStream)
{
    PS_INPUT output = (PS_INPUT) 0;

    for (int i = 0; i < 3; ++i)
    {
        output.Pos = mul(input[i].Pos, mWVP);
        output.Normal = mul(input[i].Normal, mW);
        output.UV = input[i].Tex;
        float4 lightVewPos = mul(input[i].Pos, mWLP); //���C�g�ʒu�ł̍��p���o���āc
        output.LightDepth = lightVewPos.z; //���̐[�x�l�����߂�B(�[�x�l�̐��`��ԍs����A�v�����ł����Ă�̂�Z�����̂܂ܑ��)
        output.LightUV = mul(input[i].Pos, mWLPB); //���̓_���A���C�g�r���[�ł������Ƃ��̈ʒu���킩��
        output.WorldPos = mul(input[i].Pos, mW);
        output.Depth = mul(input[i].Pos, mDepthWVP);

		// �o��
        TriStream.Append(output);
    }
    TriStream.RestartStrip();
}

//�s�N�Z���V�F�[�_
PS_OUTPUT PS(PS_INPUT In)
{
    PS_OUTPUT Out = (PS_OUTPUT) 0;
	
	//�����o�[�g�ŏƖ�
	 float deif = dot(normalize(In.Normal.xyz), -normalize(LightDir.xyz));
	 deif = deif * 0.5f + 0.5;
	 deif = deif * deif;

	Out.vColor = g_tex.Sample(g_samLinear, In.UV);  //�n�C���C�g
	Out.vColor.xyz *= deif;

    return Out;
}
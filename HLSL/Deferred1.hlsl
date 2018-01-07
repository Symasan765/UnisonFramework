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

float4 CookTorrance(float3 normal,float3 wPos,float3 Eye){
	float3 L = -LightDir.xyz; // ���C�g�x�N�g��
    float3 N = normalize(normal); // �@���x�N�g��
    float3 V = normalize(Eye - wPos); // �����x�N�g��
    float3 H = normalize(L + V); // �n�[�t�x�N�g��

	// �v�Z�Ɏg�����ꂼ��̊p�x
    float NV = dot(N, V);
    float NH = dot(N, H);
    float VH = dot(V, H);
    float NL = dot(N, L);
    float LH = dot(L, H);

	// Beckmann ���z�֐�
    const float m = 0.3f; // �r��(����0.3)
    float NH2 = NH * NH;
    float D = exp(-(1 - NH2) / (NH2 * m * m)) / (4 * m * m * NH2 * NH2);

	// �􉽌�����
    float G = min(1, min(2 * NH * NV / VH, 2 * NH * NL / VH));

	// �t���l��
    float n = 6.5f; //						 ���˂�����̗ʓI�Ȋ���(����6.5)
    float g = sqrt(n * n + LH * LH - 1);
    float gpc = g + LH;
    float gnc = g - LH;
    float cgpc = LH * gpc - 1;
    float cgnc = LH * gnc + 1;
    float F = 0.2f * gnc * gnc * (1 + cgpc * cgpc / (cgnc * cgnc)) / (gpc * gpc); //���̐擪�̒萔�Ŕ��˂̑傫���ς��

	// �����̐F
    float4 ks = { 2.0f * 0.65f, 2.0f * 0.65f, 2.0f * 0.65f, 1.0f }; //�����ŐF�ς��

    return ks * max(0, F * D * G / NV); // ���ʔ��ˌ�
}

//�s�N�Z���V�F�[�_
PS_OUTPUT PS(PS_INPUT In)
{
    PS_OUTPUT Out = (PS_OUTPUT) 0;

    Out.vPosition = In.WorldPos;                   //���W
    Out.vNormal = normalize(In.Normal);                         //�@��
	Out.vNormal.w = In.Depth.z;
	Out.vScreenSpaceSSS = g_MaterialMaskTex.Sample(g_samLinear, In.UV).r;  //SSSSS

    //�[�x�l�̔�r�̂��߂Ƀe�N�X�`��������擾
	// TODO �e�����������
    //In.LightUV /= In.LightUV.w;
    //float TexValue = g_shadowTex.Sample(g_samLinear, In.LightUV).r;     //���C�g�ʒu���猩���[�x
    //Out.vShadow = (TexValue + 0.0001f < In.LightDepth) ? 0.6f : 1.0f; //��r���đ��

	//�J���[�v�Z
	Out.vColor = g_tex.Sample(g_samLinear, In.UV);  //�`��F
	Out.vBright = Emission; //�������F

	//�����ʂ̏���
	if(g_MaterialMaskTex.Sample(g_samLinear, In.UV).g >= 0.5f){
		Out.vColor += CookTorrance(In.Normal.xyz,In.WorldPos.xyz,CameraPos.xyz);
		Out.vColor.w = 1.0f;
		//���P�x�����o
		Out.vBright += Out.vColor;
		Out.vBright -= 1.5f;		//���������߂�ƍ��P�x�������܂�
		Out.vBright *= 30.0f;		//���������߂�Ƃ͂�����Ƃ����F�ɂȂ�
		return Out;
	}

	//�����ɏƖ��v�Z�����Ă݂�
	float3 N = In.Normal.xyz;
	float3 L = -LightDir.xyz;
	float3 eye = normalize(CameraPos.xyz - In.WorldPos.xyz);
	float3 H = normalize(L + eye);

	//�����o�[�g�ŏƖ�
	 float deif = dot(In.Normal.xyz, -LightDir.xyz);
	 deif = deif * 0.5f + 0.5;
	 deif = deif * deif;

	//�u�����t�H���ŏƖ�
	//float deif = max(0.0f,dot(In.Normal.xyz,-LightDir.xyz)) + pow(max(0,dot(N,H)),10);

	// TODO �Ɩ��l���p�����[�^�����邱��
	if(deif >= 0.90f)
		Out.vColor = g_LightingHighTex.Sample(g_samLinear, In.UV);  //�n�C���C�g
	else if(deif >= 0.3f)
		Out.vColor = g_tex.Sample(g_samLinear, In.UV);  //�`��F
	else
		Out.vColor = g_LightingShadowTex.Sample(g_samLinear, In.UV);  //�V���h�E��

	//�X�N���[���X�y�[�X�T�u�T�[�t�F�X�X�L���b�^�����O�̃f�B�t���[�Y��������Bw�ɂ̓}�X�N����ꂽ�̂ł����SSSSS���̃f�B�t���[�Y�������o����
	Out.vScreenSpaceSSS.xyz = Out.vScreenSpaceSSS.w * Out.vColor.xyz;
   
    return Out;
}
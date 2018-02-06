/*=================================================
//								FILE : OutlineEmphasis.hlsl
//		�t�@�C������ :
//		�@�����Ɛ[�x�������ɗ֊s���̋������s��
//		�֊s���͊��S�ȍ��ł͂Ȃ��A���ӐF�Ɠ���ނ悤�ɏ������Ă���
//							HAL��� : ���{ �Y�V��
=================================================*/
Texture2D g_colorTex : register(t0); //�f�B�t���[�Y�e�N�X�`��
Texture2D g_NormalTex : register(t1); //�@��
SamplerState g_samLinear : register(s0);

//�g�D�[�������_�����O�̗֊s�����o�֐��ɓn���f�[�^
struct ToonEdge {
    float4 Color; //�֊s���łȂ���΂��̐F��Ԃ��F
    float2 Tex; //���߂����e�N�X�`�����W
    float offsetScall; //�𑜓x�Ŋ���l�B���l���傫���Ƃ��ꂾ�������̃e�N�Z�����܂����Ŕ�����s���̂Ő��������Ȃ�
    float DepthRange; //�֊s���Ƃ��Ď�舵���T�C�Y�B���̑����ł͂Ȃ��L���͈͂̊g��
    float NormalRange; //�֊s���Ƃ��Ď�舵���T�C�Y�B���̑����ł͂Ȃ��L���͈͂̊g��
    Texture2D normTex; //�@���e�N�X�`��
    SamplerState samp; //�T���v���[
};

cbuffer WorldMatrix : register(b0) {
    matrix g_mWVP; //���[���h����ˉe�܂ł̕ϊ��s��
};

cbuffer OutlineParam : register(b1) {
    float g_offsetScall;
	float g_DepthRange;
	float g_NormalRange;
	float g_OutlineSum;
};

//�g�D�[�������_�����O�ł̗֊s�����o��@�����Ɛ[�x����p���Ĕ�����s���֐�
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

// ���v���V�A���J�[�l��
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

	//���߂���W�𒆐S�Ɏ��͂X�}�X�̔�����s��
    for (int i = 0; i < 9; ++i)
    {
        float2 offset = Data.Tex + offsetCoord[i] * offsetScale;
		float4 NormalTexData = Data.normTex.Sample(Data.samp, clamp(offset, 0.0f, 1.0f));
        depthEdge += (NormalTexData.w + 1.0f) / 2.0f * weight[i];
        normalColor += (NormalTexData.rgb + 1.0f) / 2.0f * weight[i];
		colorData.xyz += g_colorTex.Sample(Data.samp, clamp(offset, 0.0f, 1.0f));
    }

	//�܂��@�����͈͈ȏォ�`�F�b�N����
    normalEdge = dot(abs(normalColor), tmpColor) / 3.0f;
    if(normalEdge < Data.NormalRange)
        normalEdge = 0.0f;

	//���̌�Ő[�x�Ɩ@���̂ǂ��炪�傫�����m�F���A�ŏI�I�Ȕ��f������
    float edge = 1.0f;		//�֊s���ł͂Ȃ��O��B
    if(max(abs(depthEdge), normalEdge)  > Data.DepthRange)
    {
        destColor.xyz = (colorData.xyz / 9.0f) * (g_OutlineSum);	//�F�������Â�����
    }

    return destColor;
}

//�o�[�e�b�N�X�o�b�t�@�[�o�͍\����
struct VS_OUTPUT {
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

VS_OUTPUT VS(float4 Pos : POSITION, float2 Tex : TEX)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Pos = mul(Pos, g_mWVP);
	//�e�N�X�`���[���W
    output.Tex = Tex;

    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    ToonEdge toonData;
    toonData.Color = g_colorTex.Sample(g_samLinear, input.Tex); //�֊s���łȂ���΂��̐F��Ԃ��F
    toonData.Tex = input.Tex; //���߂����e�N�X�`�����W
    toonData.offsetScall = g_offsetScall * g_NormalTex.Sample(g_samLinear, input.Tex); //�𑜓x�Ŋ���l�B���l���傫���Ƃ��ꂾ�������̃e�N�Z�����܂����Ŕ�����s���̂Ő��������Ȃ�
    toonData.DepthRange = g_DepthRange; //�֊s���Ƃ��Ď�舵���T�C�Y�B���̑����ł͂Ȃ��L���͈͂̊g��
    toonData.normTex = g_NormalTex; //�@���e�N�X�`��
    toonData.samp = g_samLinear; //�T���v���[
    toonData.NormalRange = g_NormalRange;

    return ToonEdgeWithNormalAndDepth(toonData);
}
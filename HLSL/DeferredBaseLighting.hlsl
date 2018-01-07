/*=================================================
//								FILE : DeferredBaseLighting.hlsl
//		�t�@�C������ :
//      �f�B�t�@�[�h�����_�����O�ŗ��p���郉�C�e�B���O�������s���V�F�[�_
//
//							HAL��� : ���{ �Y�V��
=================================================*/
Texture2D g_colorTex : register(t0); //�f�B�t���[�Y�e�N�X�`��
Texture2D g_shadowTex : register(t1); //�V���h�E�}�b�v
Texture2D g_positionTex : register(t2);
SamplerState g_samLinear : register(s0);

//�w���t�H�O
float4 ExpFog(float4 diffuse, float depth, float density, float4 fogColor)
{
    float d = depth; //����
    float e = 2.71828;
    float f = pow(e, -d * density); //�t�H�O�t�@�N�^�[
    f = saturate(f);
    float4 C = f * diffuse + (1 - f) * fogColor;
    return C;
}

cbuffer WorldMatrix : register(b0)
{
    matrix g_mWVP; //���[���h����ˉe�܂ł̕ϊ��s��
};

cbuffer FogData : register(b1)
{
    float4 vFogColor; //�t�H�O�̃J���[
    float vFogFactor; //�t�H�O�̋����𒲐�����
    int ShadowFlag; //�V���h�E��K�p����Ȃ�true,�g��Ȃ��Ȃ�false
}

//�o�[�e�b�N�X�o�b�t�@�[�o�͍\����
struct VS_OUTPUT
{
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
    float4 color = g_colorTex.Sample(g_samLinear, input.Tex); //�܂��͌��̐F������o��
    float4 Pos = g_positionTex.Sample(g_samLinear, input.Tex); //���W

    if (ShadowFlag)
        color *= g_shadowTex.Sample(g_samLinear, input.Tex); //�Ō�ɃV���h�E�̍������s��

    color = ExpFog(color, Pos.w, vFogFactor, vFogColor);
    
    return color;
}
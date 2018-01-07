/*=================================================
//								FILE : CrossFilter.hlsl
//		�t�@�C������ :
//		���P�x���������L�΂��N���X�t�B���^�[�𔭐�������
//
//							HAL��� : ���{ �Y�V��
=================================================*/
//�K�v�ȃf�[�^
// 4�����ɐL�΂��ۂ̂��ꂼ��̊p�x������
// ���̃����_�����O���ɗ��p����J���[�␳�l

#define MAX_SAMPLE 8

//�e�����L�΂��e�N�X�`��
Texture2D g_CrossTex1 : register(t0);
Texture2D g_CrossTex2 : register(t1);
Texture2D g_CrossTex3 : register(t2);
Texture2D g_CrossTex4 : register(t3);

SamplerState g_samLinear : register(s0);

cbuffer WorldMatrix : register(b0) {
    matrix g_mWVP; //���[���h����ˉe�܂ł̕ϊ��s��
};


cbuffer CrossParam1 : register(b1) {
    float2 g_Offset1[MAX_SAMPLE];
	float4 g_Weights1[MAX_SAMPLE];
};

cbuffer CrossParam2 : register(b2) {
	float2 g_Offset2[MAX_SAMPLE];
	float4 g_Weights2[MAX_SAMPLE];
};

cbuffer CrossParam3 : register(b3) {
    float2 g_Offset3[MAX_SAMPLE];
	float4 g_Weights3[MAX_SAMPLE];
};

cbuffer CrossParam4 : register(b4) {
    float2 g_Offset4[MAX_SAMPLE];
	float4 g_Weights4[MAX_SAMPLE];
};


//�o�[�e�b�N�X�o�b�t�@�[�o�͍\����
struct VS_OUTPUT {
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

struct PS_OUTPUT
{
    float4 vCross1 : SV_Target0;	
    float4 vCross2 : SV_Target1;
    float4 vCross3 : SV_Target2;	
    float4 vCross4 : SV_Target3;
};

VS_OUTPUT VS(float4 Pos : POSITION, float2 Tex : TEX)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Pos = mul(Pos, g_mWVP);
	//�e�N�X�`���[���W
    output.Tex = Tex;

    return output;
}


PS_OUTPUT PS(VS_OUTPUT input)
{
	 PS_OUTPUT vColor = (PS_OUTPUT)(0);
    
    // ��䊂ɂ������W�̓_���T���v�����O����
	
    for(int i = 0; i < 8; i++) {
        vColor.vCross1 += g_Weights1[i] * g_CrossTex1.Sample(g_samLinear, input.Tex + g_Offset1[i]);

		vColor.vCross2 += g_Weights2[i] * g_CrossTex2.Sample(g_samLinear, input.Tex + g_Offset2[i]);

		vColor.vCross3 += g_Weights3[i] * g_CrossTex3.Sample(g_samLinear, input.Tex + g_Offset3[i]);

		vColor.vCross4 += g_Weights4[i] * g_CrossTex4.Sample(g_samLinear, input.Tex + g_Offset4[i]);
    }
	
	/*
		vColor.vCross1 = g_CrossTex1.Sample(g_samLinear, input.Tex);

		vColor.vCross2 = g_CrossTex2.Sample(g_samLinear, input.Tex);

		vColor.vCross3 = g_CrossTex3.Sample(g_samLinear, input.Tex);

		vColor.vCross4 = g_CrossTex4.Sample(g_samLinear, input.Tex);
*/
	vColor.vCross1.a = 1.0f;
	vColor.vCross2.a = 1.0f;
	vColor.vCross3.a = 1.0f;
	vColor.vCross4.a = 1.0f;

    return vColor;
}
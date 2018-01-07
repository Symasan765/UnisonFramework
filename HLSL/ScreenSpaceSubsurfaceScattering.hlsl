/*=================================================
//								FILE : CelShading.hlsl
//		�t�@�C������ :
//		�g�D�[�������_�����O�̃|�X�g�G�t�F�N�g�������s��
//
//							HAL��� : ���{ �Y�V��
=================================================*/
Texture2D g_sssssTex : register(t0); //�f�B�t���[�Y�e�N�X�`��
Texture2D g_depthTex : register(t1); //�[�x�}�b�v
Texture2D g_diffuseTex : register(t2); //�S�̃J���[���

SamplerState g_samLinear : register(s0);
SamplerState g_samPoint : register(s1);

cbuffer WorldMatrix : register(b0) {
    matrix g_mWVP; //���[���h����ˉe�܂ł̕ϊ��s��
};

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

float4 BlurPS(float2 texcoord, uniform float2 step) : SV_TARGET {
	float correction = 5.0f;

	//���݂̃s�N�Z���̎����6�̃T���v���̃K�E�X�d��
	// -3 -2 -1 +1 +2 +3
    float w[6] = { 0.006,   0.061,   0.242,  0.242,  0.061, 0.006 };
    float o[6] = {  -1.0, -0.6667, -0.3333, 0.3333, 0.6667,   1.0 };
	
	//���݂̃s�N�Z���̐F�Ɛ[�x
    float4 colorM = g_sssssTex.Sample(g_samPoint, texcoord);
    float depthM = g_depthTex.Sample(g_samPoint, texcoord).w;
	
	//���S�T���v���ɃK�E�X�d�݂��|���ėݐς���
    float4 colorBlurred = colorM;
    colorBlurred.rgb *= 0.382;
	
	//���͂̃s�N�Z�����t�F�b�`���邽�߂ɗ��p����X�e�b�v���v�Z���A
	//"step"�͎��̒ʂ�
	//step = SSSStrength * gausssianEidth * pixelSize * dir
	//�s�N�Z�����߂��قǌ��ʂ������Ȃ���΂Ȃ�Ȃ����߁A���q1.0 / �[��M
    float2 finalStep = colorM.a * step / depthM;
	
	//���̃T���v����ݐς���
    [unroll]
    for (int i = 0; i < 6; i++) {
		// ���݂̐F�Ɛ[�x���擾����
        float2 offset = texcoord + o[i] * finalStep;
        float3 color = g_sssssTex.SampleLevel(g_samLinear, offset, 0).rgb;
        float depth = g_depthTex.SampleLevel(g_samPoint, offset, 0).w;
		
		//�[�x�̍����傫���ꍇ��colorM���ucolorM�v�ɖ߂��܂�
        float s = min(0.0125 * correction * abs(depthM - depth), 1.0);
        color = lerp(color, colorM.rgb, s);
		
		//�ݐ�
        colorBlurred.rgb += w[i] * color;
    }
	
    return colorBlurred;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
	//SSSSS�t���O������Ώ�������
	// TODO SSSSS�̃p�����[�^�����s��
	float data = 0.5f;
	if(g_sssssTex.Sample(g_samPoint, input.Tex).w > 0.5f){
		float2 dir = data * data * (float2)(1.0f / 1280.0f,1.0f / 720.0f) * data;
		return BlurPS(input.Tex,dir);
	}

	//�Ȃ���Ε��ʂɃJ���[��Ԃ�
    return g_diffuseTex.Sample(g_samPoint, input.Tex);
}
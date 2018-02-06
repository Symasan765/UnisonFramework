/*=================================================
//								FILE : ShaderCBuffer.hlsl
//		�t�@�C������ :
//
//
//							HAL��� : ���{ �Y�V��
=================================================*/
//��`
#define MAX_BONE_MATRICES 128       //�ő�{�[����
#define STATIC_MESH 128                 //�A�j���[�V�����Ȃ�

cbuffer WorldMatrix : register(b0)
{
    matrix mW; //���[���h�s��
    matrix mWVP; //���[���h����ˉe�܂ł̕ϊ������s��
    matrix mWLP; //���[���h�E�h���C�g�r���[�h�E�v���W�F�N�V�����̍���
    matrix mWLPB; //���[���h�E�h���C�g�r���[�h�E�v���W�F�N�V�����EUV�s��̍���
    matrix mDepthWVP;
}

cbuffer Material : register(b1)
{
	// ���˗�
    float4 k_a; // ambient
    float4 k_d; // diffuse
    float4 k_s; // specular
    float4 Emission; //�����F
    int4 General; //x���� = �V�F�[�_�ԍ� ���c����
}

cbuffer global_bones : register(b2) //�{�[���̃|�[�Y�s�񂪓���
{
    matrix g_mConstBoneWorld[MAX_BONE_MATRICES];
};

cbuffer global_Light:register(b3)
{	
	float4 LightDir;		//���C�g�ʒu
	float4 I_Ambient;
	float4 I_Deffuse;
};

cbuffer global_Camera:register(b4)
{	
	float4 CameraPos;
};

cbuffer global_ToonParam:register(b5)
{	
	float LightBorder;
	float ShadowBorder;
	float HighlightSub;
	float HighlightSum;
};
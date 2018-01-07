/*=================================================
//								FILE : WaveSimulation.hlsl
//		�t�@�C������ :
//      �g�̃V�~�����[�V�������s���e�b�Z���[�V�����X�e�[�W���`����
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "ShaderStruct.hlsl"
#include "PerlinNoise.hlsl"

cbuffer global_waves : register(b4) //�{�[���̃|�[�Y�s�񂪓���
{
    float g_time;
    float AMPLITUDE;			//�g�̂ӂ蕝
    float LENGTH;				//�g��
    float CYCLE;					//����
    float4 g_WavePoint; //�g�̔�����
};

struct HS_CONST
{
    float factor[3] : SV_TessFactor;
    float inner_factor : SV_InsideTessFactor;
};

struct HS_OUTPUT
{
    float4 Pos : POSITION; // ���[�J���ʒu���W
    float4 Normal : NORMAL; // �@���x�N�g��   �g��Ȃ����Ǒ��Ƃ̔ėp���̂��߂Ɏc��
    float2 Tex : TEX; //�e�N�X�`�����W
    uint4 Bones : BONE_INDEX; //�{�[���̃C���f�b�N�X
    float4 Weights : BONE_WEIGHT; //�{�[���̏d��
};

//�n���V�F�[�_�[�R���X�^���g
HS_CONST HSConstant(InputPatch<INPUT_RAW_DATA, 3> ip, uint pid : SV_PrimitiveID)
{
    HS_CONST Out = (HS_CONST)(0);

    float devide = 64;

    Out.factor[0] = devide;
    Out.factor[1] = devide;
    Out.factor[2] = devide;

    Out.inner_factor = devide;

    return Out;
}

//�n���V�F�[�_�[
[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("HSConstant")]
HS_OUTPUT HS(InputPatch<INPUT_RAW_DATA, 3> ip, uint cpid : SV_OutputControlPointID, uint pid : SV_PrimitiveID)
{
	//��{�I�Ƀn���V�F�[�_�ł͒��_�����X���[���ďo�͂���
    HS_OUTPUT Out = (HS_OUTPUT) (0);
    Out.Pos = ip[cpid].Pos;
    Out.Normal = ip[cpid].Normal;
    Out.Bones = ip[cpid].Normal;
    Out.Tex = ip[cpid].Tex;
    Out.Weights = ip[cpid].Weights;
    return Out;
}

float GetWaveHeight(float3 pos,float a,float len,float time,float cycle){
	float Len = length(pos.xy);
    return a * sin(2 * 3.1415926f * (Len / len - time / cycle));
}

//�h���C���V�F�[�_�[
[domain("tri")]
INPUT_RAW_DATA DS(HS_CONST In, float3 UV : SV_DomaInLocation, const OutputPatch<HS_OUTPUT, 3> patch)
{
	//�����ł͍��W�ϊ��ȊO�̃h���C���̌`���Ɋւ��鏈�����s��
	//�܂��͊e�����Ԃ��Đ��m�Ȉʒu������o���������K�v
    INPUT_RAW_DATA Out = (INPUT_RAW_DATA) (0);

    Out.Pos = patch[0].Pos * UV.x + patch[1].Pos * UV.y + patch[2].Pos * UV.z;
    Out.Normal = patch[0].Normal * UV.x + patch[1].Normal * UV.y + patch[2].Normal * UV.z;
    Out.Bones = patch[0].Bones * UV.x + patch[1].Bones * UV.y + patch[2].Bones * UV.z;
    Out.Tex = patch[0].Tex * UV.x + patch[1].Tex * UV.y + patch[2].Tex * UV.z;
    Out.Weights = patch[0].Weights * UV.x + patch[1].Weights * UV.y + patch[2].Weights * UV.z;

	//�ό`����
	Out.Pos.z = GetWaveHeight(Out.Pos.xyz,AMPLITUDE,LENGTH,g_time,CYCLE);
	Out.Pos.z += GetWaveHeight(Out.Pos.xyz,AMPLITUDE / 4.0f,LENGTH * 2.0f,g_time,CYCLE * 1.2f);

	float aroundh[4] = { 0, 0, 0, 0 };
    float offset = 0.01f;   // �K���ɐݒ�
	aroundh[0] += GetWaveHeight(Out.Pos.xyz + (float3)(offset,0.0f,0.0f),AMPLITUDE,LENGTH,g_time,CYCLE);
    aroundh[1] += GetWaveHeight(Out.Pos.xyz + (float3)(0.0f,-offset,0.0f),AMPLITUDE,LENGTH,g_time,CYCLE);
    aroundh[2] += GetWaveHeight(Out.Pos.xyz + (float3)(-offset,0.0f,0.0f),AMPLITUDE,LENGTH,g_time,CYCLE);
    aroundh[3] += GetWaveHeight(Out.Pos.xyz + (float3)(0.0f,offset,0.0f),AMPLITUDE,LENGTH,g_time,CYCLE);

	//float4 norm = (float4)(cross(patch[1].Pos.xyz - patch[0].Pos.xyz,patch[2].Pos.xyz - patch[0].Pos.xyz),0.0f);
	//Out.Normal = normalize(norm);


	float dx = aroundh[2] - aroundh[0];
    float dz = aroundh[1] - aroundh[3];

	Out.Normal = -normalize( float4( dx, 0.2f, dz ,1.0f) );
    //Out.Pos.z = Out.Pos.y;
    //Out.Pos.y = PerlinNoise((Out.Pos.xy + 1.0f) / 2.0f, 1.0f / 8.0f, 1.0);
    return Out;
}
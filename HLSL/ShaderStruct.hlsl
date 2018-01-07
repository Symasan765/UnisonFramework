/*=================================================
//								FILE : ShaderStruct.hlsl
//		�t�@�C������ :
//
//
//							HAL��� : ���{ �Y�V��
=================================================*/

//���_�V�F�[�_�o�͍\����
struct VS_OUTPUT
{
    float4 Pos : POSITION;
    float4 Normal : NORMAL;
    float4 Depth : WORLDDEPTH;
    float4 LightDepth : DEPTH; //�[�x�l
    float2 UV : TEXCOORD0;
    float4 LightUV : TEXCOORD1; //���C�g���W�ɕϊ������ۂ�UV���W
    float4 WorldPos : POSITION2;
};

//�s�N�Z���V�F�[�_���͍\����
struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Normal : NORMAL;
    float4 Depth : WORLDDEPTH;
    float4 LightDepth : DEPTH; //�[�x�l
    float2 UV : TEXCOORD0;
    float4 LightUV : TEXCOORD1;
    float4 WorldPos : TEXCOORD2; //���[���h���W��񂪕K�v�Ȃ̂œ]������
};

//�ŏI�����_�����O
struct PS_OUTPUT
{
    float4 vColor : SV_Target0;					//�Ɩ��v�Z�σJ���[
    float4 vPosition : SV_Target1;				//���W���
    float4 vNormal : SV_Target2;					//�@���̌��� w : �[�x
    float4 vBright : SV_Target3;					//���P�x���
    float4 vScreenSpaceSSS : SV_Target4;	//XYZ : SSSSS���J���[ W : SSSSS�}�X�N
};

//�X�L�j���O��̒��_�E�@��������
struct Skin
{
    float4 Pos;
    float4 Normal;
};

//�����������������̂܂܏o�͂���\����
//��ɒ��_�V�F�[�_�ł͂Ȃ��W�I���g���V�F�[�_�ŏ�������ꍇ�Ɏg�p����
//���_�V�F�[�_�̏o�͂ƃh���C���V�F�[�_�[�̏o�́A�W�I���g���V�F�[�_�̓��͂ɗ��p����
struct INPUT_RAW_DATA
{
    float4 Pos : POSITION; // ���[�J���ʒu���W
    float4 Normal : NORMAL; // �@���x�N�g��   �g��Ȃ����Ǒ��Ƃ̔ėp���̂��߂Ɏc��
    float2 Tex : TEX; //�e�N�X�`�����W
    uint4 Bones : BONE_INDEX; //�{�[���̃C���f�b�N�X
    float4 Weights : BONE_WEIGHT; //�{�[���̏d��
};
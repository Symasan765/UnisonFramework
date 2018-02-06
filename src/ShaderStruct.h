/*=================================================
//								FILE : ShaderStruct.h
//		�t�@�C������ :
//		�V�F�[�_�ŗ��p����萔�o�b�t�@�\���̂��`���Ă����t�@�C��
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "WinMain.h"

struct WorldData {
	DirectX::XMFLOAT4 vLightDir;	//���C�g����
	DirectX::XMFLOAT4 vEyePos;		//�J�������W
};

struct MatrixData
{
	DirectX::XMFLOAT4X4 mWVP;		//���[���h����ˉe�܂ł̕ϊ��s��
	DirectX::XMFLOAT4X4 mW;			//���[���h�ϊ��s��
	void Transpose();			//�ێ����Ă���s���]�u�s��ɕϊ�����
	void SetWorldMatrix(const DirectX::XMMATRIX& mat);
	void SetWVPMatrix(const DirectX::XMMATRIX& mat);
	void SetW_VPMatrix(const DirectX::XMFLOAT4X4& w, const DirectX::XMFLOAT4X4& vp);
};

struct LightData {
	DirectX::XMFLOAT4 I_a; // ambient
	DirectX::XMFLOAT4 I_d; // diffuse
};

struct MaterialData {
	DirectX::XMFLOAT4 k_a; // ambient
	DirectX::XMFLOAT4 k_d; // diffuse
	DirectX::XMFLOAT4 vColor; //�}�e���A���J���[
};

struct FogData {
	DirectX::XMFLOAT4 vColor;		//�t�H�O�̃J���[
	float vFactor;						//�t�H�O�̋����𒲐�������q
	int vShadowFlag;					//�V���h�E��K�p���邩�ǂ����̃t���O
	DirectX::XMFLOAT2 vDamy;		//16�{���ɒ������邽�߂̃_�~�[
};

struct DoFData {
	DirectX::XMFLOAT2 vScreenPos;	//��ʊE�[�x�̓K�p���s����ʍ��W
	float vDoFRange;						//�s���g�{�P�͈̔́B�傫���Ƃ����{�P��
	float vBlur;								//�{�P�̑傫��
};

struct RadialBlurData {
	DirectX::XMFLOAT2 vBlurPos;        //�ڂ����N�_
	float vDelta;				//�ڂ����̑傫���B-0.3���炢�����傤�ǂ���
	float vBlurNum;              //�ڂ����񐔁B�����ƂȂ߂炩�ɂȂ�
};

/// <summary>
/// �A�v���P�[�V�������ňꎞ�I�Ƀ��W�A���u���[�̏������s���\���̂ŃV�F�[�_�ɂ͓n���Ȃ�
/// </summary>
struct RadialBlurBuf {
	DirectX::XMFLOAT3 vBlurPos;        //�ڂ����N�_
	float vDelta;				//�ڂ����̑傫���B-0.3���炢�����傤�ǂ���
	float vBlurNum;              //�ڂ����񐔁B�����ƂȂ߂炩�ɂȂ�
	bool RadialBlurFlag;
};

//=======================�f�B�t�@�[�h�����_�����O�ɗ��p����=========================//
//���[���h�}�g���N�X���
struct DefeMatrix {
	DirectX::XMFLOAT4X4 mW;
	DirectX::XMFLOAT4X4 mWVP;
	DirectX::XMFLOAT4X4 mWLP;
	DirectX::XMFLOAT4X4 mWLPB;
	DirectX::XMFLOAT4X4 mDepthWVP;
};

//�}�e���A�����
struct DefeMaterial
{
	DirectX::XMFLOAT4 k_a;			//�A���r�G���g
	DirectX::XMFLOAT4 k_d;			//�f�B�t�[�Y
	DirectX::XMFLOAT4 k_s;			//�X�y�L����
	DirectX::XMFLOAT4 Emission;	//�������F
	DirectX::XMINT4 Flag;				//x���� = �V�F�[�_�ԍ� ���c����
};

//�����\����
struct DirectionalLightData {
	DirectX::XMFLOAT4 LightDir;		//���C�g�ʒu
	DirectX::XMFLOAT4 I_Ambient;
	DirectX::XMFLOAT4 I_Deffuse;
};

//�g�̃p�����[�^�Ƃ��ė��p����
struct WaveStruct {
	float vtime;
	float AMPLITUDE;			//�g�̂ӂ蕝
	float LENGTH;				//�g��
	float CYCLE;					//����
	DirectX::XMFLOAT4 vWavePoint; //�g�̔�����
};

struct ToonLightingStruct {
	float LightBorder;
	float ShadowBorder;
	float HighlightSub;
	float HighlightSum;

	ToonLightingStruct() {
		LightBorder = 0;
		ShadowBorder = 0;
		HighlightSub = 0;
		HighlightSum = 0;
	}
};

struct OutlineParam {
	float offsetScall;
	float DepthRange;
	float NormalRange;
	float OutlineSum;

	OutlineParam() {
		offsetScall = 0;
		DepthRange = 0;
		NormalRange = 0;
		OutlineSum = 0;
	}
};
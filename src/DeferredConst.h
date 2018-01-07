/*=================================================
//								FILE : DeferredConst.h
//		�t�@�C������ :
//		�f�B�t�@�[�h�����_�����O�ŗ��p���邽�߂̒萔����舵���N���X
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once

#include "DeferredConst.h"
#include "ConstantBuffer.h"
#include "ShaderStruct.h"
#include "WinMain.h"
#include "Camera.h"

/// <summary>
/// �f�B�t�@�[�h�����_�����O�ɗ��p����萔�o�b�t�@���Ǘ�����N���X
/// ��{�I�ɃA�v������g���ꍇ�A�ݒ肷�鍀�ڂ�
/// �܂��`��V�[���̍ŏ���SetCameraData�ASetLightData���ĂԁB���BaseModel�̒��ōs���Ă����
/// </summary>
class cDeferredConst{
public:
	cDeferredConst();
	~cDeferredConst();
	void SetShader(bool _mtrOnly = false);
	static void SetCameraData(const CameraData& _cameraData, ViewProj _vp);
	static void SetLightData(const DirectX::XMFLOAT3& _light);
	void SetWorldMatrix(const DirectX::XMFLOAT4X4& _w);
	void SetEmission(DirectX::XMFLOAT4 _color);
	void SetMaterial(DirectX::XMFLOAT4 _a, DirectX::XMFLOAT4 _d, DirectX::XMFLOAT4 _s);
	void SetShaderType(const int);
	const CameraData GetCameraData()const;
private:
	cConstBuf<DefeMatrix> m_MatrixConst;		//�V�F�[�_�ɓn���萔�o�b�t�@
	cConstBuf<DefeMaterial> m_MaterialConst;

	//�ݒ莞�ɕK�v�ȍ���
	static DirectX::XMFLOAT3 m_LightPos;			//���C�g�ʒu
	static DirectX::XMFLOAT3 m_Lookpt;			//�J���������_
	static DirectX::XMFLOAT3 m_CameraPos;		//�J�����ʒu
	static DirectX::XMFLOAT3 m_CameraUp;
	static DirectX::XMFLOAT4X4 m_View;			//�r���[�s��
	static DirectX::XMFLOAT4X4 m_DepthProj;			//�v���W�F�N�V�����s��
	static DirectX::XMFLOAT4X4 m_Proj;			//�v���W�F�N�V�����s��
	DirectX::XMFLOAT4X4 m_World;						//�ʂ̃��[���h�s��
	static float m_Near;
	static float m_Far;
};
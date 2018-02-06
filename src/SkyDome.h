/*=================================================
//								FILE : SkyDome.h
//		�t�@�C������ :
//		�X�J�C�h�[���̓ǂݍ��݁A�V�F�[�_�؂�ւ��A�`����s���N���X
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "DeferredBaseModel.h"
#include "PostEffects.h"
#include "Camera.h"

class cSkyDome{
public:
	cSkyDome();
	~cSkyDome();
	void DrawSkyDome(CameraData camera);
private:
	cDeferredModel* m_SkyDome;		//�X�J�C�h�[�����f����ǂݍ���
	std::shared_ptr<ShaderData> m_Shader;									//�f�B�t�@�[�h�����_�����O�V�F�[�_��ێ�
	ID3D11DepthStencilState* m_pDepthStencilState;

	float m_RotAngle;
};
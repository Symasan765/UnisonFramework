/*=================================================
//								FILE : ShadowMap.cpp
//		�t�@�C������ :
//		�V���h�E�}�b�v����舵���N���X
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "WinMain.h"
#include "ConstantBuffer.h"
#include "ShaderStruct.h"
#include "RenderTarget.h"
#include "Shader.h"

#define SHADOW_MAP_WIDTH 4096
#define SHADOW_MAP_HEIGHT 4096
#define SHADOW_MAP_NEAR 1.0f
#define SHADOW_MAP_FAR 300.0f
#define SHADOW_MAP_VIEWING_ANGLE 10.0f

/// <summary>
/// �V���h�E�}�b�v����舵���N���X
/// ���i�K�Ńe�N�X�`���P���ɂ��W���I�ȃV���h�E�}�b�v�ɑΉ�������
/// </summary>
class cShadowMap {
public:
	cShadowMap(const cShadowMap&) = delete;			//�R�s�[�R���X�g���N�^�� delete �w��
	cShadowMap& operator=(const cShadowMap&) = delete;	//�R�s�[������Z�q�� delete �w��
	cShadowMap(cShadowMap&&) = delete;			//���[�u�R���X�g���N�^�� delete �w��
	cShadowMap& operator=(cShadowMap&&) = delete;		//���[�u������Z�q�� delete �w��

	static cShadowMap& GetInstance() {
		static cShadowMap inst;			// private�ȃR���X�g���N�^���Ăяo��
		return inst;
	}
	void SetRender();		//�`�揀��
	ID3D11ShaderResourceView* GetDepthResourceView();
private:
	cShadowMap();					//�R���X�g���N�^�� private �ɒu��
	~cShadowMap();					//�f�X�g���N�^�� private �ɒu��

	//==============================================================//
	cRenderTargetTex* m_DepthTarget;			//�[�x�������݃e�N�X�`��
	std::shared_ptr<ShaderData> m_Shader;							//�[�x�}�b�v�쐬�V�F�[�_
};


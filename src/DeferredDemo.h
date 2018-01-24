/*=================================================
//								FILE : DeferredRendering.h
//		�t�@�C������ :
//		�f�B�t�@�[�h�����_�����O����舵���N���X
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "WinMain.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "GBuffer.h"

/// <summary>
/// �f�B�t�@�[�h�����_�����O���Ǘ�����N���X
/// </summary>
class cDeferredDemo {
public:
	cDeferredDemo(const cDeferredDemo&) = delete;			//�R�s�[�R���X�g���N�^�� delete �w��
	cDeferredDemo& operator=(const cDeferredDemo&) = delete;	//�R�s�[������Z�q�� delete �w��
	cDeferredDemo(cDeferredDemo&&) = delete;			//���[�u�R���X�g���N�^�� delete �w��
	cDeferredDemo& operator=(cDeferredDemo&&) = delete;		//���[�u������Z�q�� delete �w��

	static cDeferredDemo& GetInstance() {
		static cDeferredDemo inst;			// private�ȃR���X�g���N�^���Ăяo��
		return inst;
	}

	void SetDeferredRendering(ID3D11ShaderResourceView* _DepthMap);
	ID3D11ShaderResourceView* GetResourceView(int _no);
	GBuffer GetGraphicBuffer();
	void SetDeferredShader();
private:
	enum RenderName {
		DIFFUSE = 0,		//�J���[(�Ɩ��v�Z��)
		POSITION,			//���W
		NORMAL,				//�@��(w�ɐ[�x�l���i�[����)
		BRIGHT,				//���P�x���
		SSSSS,				//xyz : SSS�K�p���f�B�t���[�Y w : SSS�K�p���}�X�N
		TARGET_MAX		//�쐬�e�N�X�`����
	};
	cDeferredDemo();					//�R���X�g���N�^�� private �ɒu��
	~cDeferredDemo();					//�f�X�g���N�^�� private �ɒu��

											//===================�ϐ�======================//
	cRenderTargetTex* m_pRenderTergetTex;		//G-Buffer�Ŏg�p����e�N�X�`����ێ�����
	ID3D11SamplerState* m_pSampleLinear;			//�T���v���B�����ɂ���񂩂ȁc�H
	std::shared_ptr<ShaderData> m_Shader;									//�f�B�t�@�[�h�����_�����O�V�F�[�_��ێ�
};
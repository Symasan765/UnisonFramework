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

//�f�B�t�@�[�h�����_�����O��Ŏg�p�ł���V�F�[�_(�r�b�g���Z�ɂĕ����Ή�)
enum class ShaderType {
	Lambert = 1,				//�����o�[�g�g�U��
	BlinnPhong = 2,			//�u�����t�H��
	CookTorrance = 4,		//�N�b�N�g�����X�������˃��f��
	Edge = 8,					//�G�b�W���o
	DOF = 16,					//��ʊE�[�x
	Toon = 32					//�g�D�[���V�F�[�h
};

/// <summary>
/// �f�B�t�@�[�h�����_�����O���Ǘ�����N���X
/// </summary>
class cDeferredRendering {
public:
	cDeferredRendering(const cDeferredRendering&) = delete;			//�R�s�[�R���X�g���N�^�� delete �w��
	cDeferredRendering& operator=(const cDeferredRendering&) = delete;	//�R�s�[������Z�q�� delete �w��
	cDeferredRendering(cDeferredRendering&&) = delete;			//���[�u�R���X�g���N�^�� delete �w��
	cDeferredRendering& operator=(cDeferredRendering&&) = delete;		//���[�u������Z�q�� delete �w��

	static cDeferredRendering& GetInstance() {
		static cDeferredRendering inst;			// private�ȃR���X�g���N�^���Ăяo��
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
	cDeferredRendering();					//�R���X�g���N�^�� private �ɒu��
	~cDeferredRendering();					//�f�X�g���N�^�� private �ɒu��

	//===================�ϐ�======================//
	cRenderTargetTex* m_pRenderTergetTex;		//G-Buffer�Ŏg�p����e�N�X�`����ێ�����
	ID3D11SamplerState* m_pSampleLinear;			//�T���v���B�����ɂ���񂩂ȁc�H
	std::shared_ptr<ShaderData> m_Shader;									//�f�B�t�@�[�h�����_�����O�V�F�[�_��ێ�
};
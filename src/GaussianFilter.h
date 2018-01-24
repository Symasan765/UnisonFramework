/*=================================================
//								FILE : GaussianFilter.h
//		�t�@�C������ :
//		�K�E�X�t�B���^����舵���N���X���L�q����
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "PostEffects.h"
#include "RenderTarget.h"

class cGaussianFilter {
public:
	cGaussianFilter(const cGaussianFilter&) = delete;			//�R�s�[�R���X�g���N�^�� delete �w��
	cGaussianFilter& operator=(const cGaussianFilter&) = delete;	//�R�s�[������Z�q�� delete �w��
	cGaussianFilter(cGaussianFilter&&) = delete;			//���[�u�R���X�g���N�^�� delete �w��
	cGaussianFilter& operator=(cGaussianFilter&&) = delete;		//���[�u������Z�q�� delete �w��

	static cGaussianFilter& getInstance() {
		static cGaussianFilter inst;			// private�ȃR���X�g���N�^���Ăяo��
		return inst;
	}

	void DrawGaussianFilter(cRenderTargetTex* outputTex, ID3D11ShaderResourceView* inputTex);
private:
	cGaussianFilter();					//�R���X�g���N�^�� private �ɒu��
	~cGaussianFilter();					//�f�X�g���N�^�� private �ɒu��

	cPostEffects* m_pXBlur;		//�������̂ڂ���
	cPostEffects* m_pYBlur;		//�c�����̂ڂ���

	cRenderTargetTex* m_pBufRenderTarget;		//�ꎞ�I�ɕ`����s���e�N�X�`��

	ID3D11SamplerState* m_Clamp;
	ID3D11SamplerState* m_Wrap;
};
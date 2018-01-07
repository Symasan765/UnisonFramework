/*=================================================
//								FILE : SMAABlending.h
//		�t�@�C������ :
//		SMAA�ɂ�����ŏI�p�X�̃u�����f�B���O�v�Z���s��
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "PostEffects.h"
#include "RenderTarget.h"

class cSMAABlending : public cPostEffects {
public:
	cSMAABlending();
	~cSMAABlending();

	void DrawBlending(ID3D11ShaderResourceView * _color, ID3D11ShaderResourceView * _weight);
	ID3D11ShaderResourceView * GetResourceView();
private:
	cRenderTargetTex* m_pBufRenderTarget;		//�ꎞ�I�ɕ`����s���e�N�X�`��
};
/*=================================================
//								FILE : SMAAEdge.h
//		�t�@�C������ :
//		SMAA�ɂ�������p�X�̃G�b�W���o���s��
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "PostEffects.h"
#include "RenderTarget.h"

class cSMAAEdge : public cPostEffects {
public:
	cSMAAEdge();
	~cSMAAEdge();

	void DrawEdge(ID3D11ShaderResourceView * _color);
	ID3D11ShaderResourceView * GetResourceView();
private:
	cRenderTargetTex* m_pBufRenderTarget;		//�ꎞ�I�ɕ`����s���e�N�X�`��
};
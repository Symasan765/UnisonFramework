/*=================================================
//								FILE : SMAABlendingWeight.h
//		�t�@�C������ :
//		SMAA�̑��p�X�B���E���̏d�݂Â����s��
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "PostEffects.h"
#include "RenderTarget.h"

class cSMAABlendingWeight : public cPostEffects {
public:
	cSMAABlendingWeight();
	~cSMAABlendingWeight();

	void DrawBlendingWeight(ID3D11ShaderResourceView * _edge, ID3D11ShaderResourceView * area, ID3D11ShaderResourceView * search);
	ID3D11ShaderResourceView * GetResourceView();
private:
	cRenderTargetTex* m_pBufRenderTarget;		//�ꎞ�I�ɕ`����s���e�N�X�`��
};
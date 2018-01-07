/*=================================================
//								FILE : FXAntialiasing.h
//		�t�@�C������ :
//		FXAA����������N���X
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once

#include "PostEffects.h"
#include "RenderTarget.h"

class cFXAA : public cPostEffects {
public:
	cFXAA();
	~cFXAA();

	void DrawFXAA(ID3D11ShaderResourceView * _color);
	ID3D11ShaderResourceView * GetResourceView();
private:
	cRenderTargetTex* m_pBufRenderTarget;		//�ꎞ�I�ɕ`����s���e�N�X�`��
};
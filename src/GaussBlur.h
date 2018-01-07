/*=================================================
//								FILE : GaussBlur.h
//		�t�@�C������ :
//		�K�E�X�ڂ����������N���X�B�P�p�X�ŃK�E�X�t�B���^���s��
//		2017/12/25���݂�1280 : 720 �̂ڂ������s�����Ƃ�O��ɂ��Ă���
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "PostEffects.h"
#include "RenderTarget.h"

class cGaussBlur : public cPostEffects {
public:
	cGaussBlur();
	~cGaussBlur();

	void DrawGaussBlur(ID3D11ShaderResourceView * inData);
	ID3D11ShaderResourceView * GetResourceView();
private:
	cRenderTargetTex* m_pBufRenderTarget;		//�ڂ������摜�������_�����O����e�N�X�`��
};
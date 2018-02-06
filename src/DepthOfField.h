/*=================================================
//								FILE : DepthOfField.h
//		�t�@�C������ :
//		��ʊE�[�x��������舵��
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "PostEffects.h"
#include "RenderTarget.h"
#include "GaussBlur.h"
#include "LuaActor.h"

class cDepthOfField : public cPostEffects {
public:
	cDepthOfField();
	~cDepthOfField();

	void DrawDOF(ID3D11ShaderResourceView * color, ID3D11ShaderResourceView * _normal, const DoFData& data);
	ID3D11ShaderResourceView * GetResourceView();
private:
	cConstBuf<DoFData> m_ShaderConst;
	cRenderTargetTex* m_pBufRenderTarget;		//�ꎞ�I�ɕ`����s���e�N�X�`��
	cGaussBlur* m_GauseBlue;
	cLuaActor* m_LuaAct;
};
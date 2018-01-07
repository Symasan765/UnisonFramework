/*=================================================
//								FILE : GaussBlur.cpp
//		ファイル説明 :
//		ガウスブラーを扱うクラスを定義する
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "GaussBlur.h"

cGaussBlur::cGaussBlur() : cPostEffects("GaussBlur.hlsl")
{
	m_pBufRenderTarget = new cRenderTargetTex;
	m_pBufRenderTarget->RTCreate((float)WINDOW_SIZE_X / 2.0f, (float)WINDOW_SIZE_Y / 2.0f, { 0,0,0 });
}

cGaussBlur::~cGaussBlur()
{
	delete m_pBufRenderTarget;
}

void cGaussBlur::DrawGaussBlur(ID3D11ShaderResourceView * inData)
{
	this->SetTexSamplerState(0, LINEAR);
	m_pBufRenderTarget->SetRenderTargetTex();
	this->Draw(&inData, 1);
}

ID3D11ShaderResourceView * cGaussBlur::GetResourceView()
{
	return m_pBufRenderTarget->GetTextureResourceView();
}
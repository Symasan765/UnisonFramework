#include "FXAntialiasing.h"

cFXAA::cFXAA() : cPostEffects("FXAAShader.hlsl")
{
	m_pBufRenderTarget = new cRenderTargetTex;
	m_pBufRenderTarget->RTCreate((float)WINDOW_SIZE_X, (float)WINDOW_SIZE_Y, { 0,0,0 });
}

cFXAA::~cFXAA()
{
	delete m_pBufRenderTarget;
}

void cFXAA::DrawFXAA(ID3D11ShaderResourceView * _color)
{
	this->SetTexSamplerState(0, LINEAR);
	m_pBufRenderTarget->SetRenderTargetTex();
	this->Draw(&_color);
}

ID3D11ShaderResourceView * cFXAA::GetResourceView()
{
	return m_pBufRenderTarget->GetTextureResourceView();
}

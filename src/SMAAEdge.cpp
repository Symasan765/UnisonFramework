#include "SMAAEdge.h"

cSMAAEdge::cSMAAEdge() : cPostEffects("SMAAEdge.hlsl")
{
	m_pBufRenderTarget = new cRenderTargetTex;
	m_pBufRenderTarget->RTCreate((float)WINDOW_SIZE_X, (float)WINDOW_SIZE_Y, { 0,0,0 });
}

cSMAAEdge::~cSMAAEdge()
{
	delete m_pBufRenderTarget;
}

void cSMAAEdge::DrawEdge(ID3D11ShaderResourceView * _color)
{
	this->SetTexSamplerState(0, LINEAR);
	m_pBufRenderTarget->SetRenderTargetTex();
	this->Draw(&_color,1);
}

ID3D11ShaderResourceView * cSMAAEdge::GetResourceView()
{
	return m_pBufRenderTarget->GetTextureResourceView();
}

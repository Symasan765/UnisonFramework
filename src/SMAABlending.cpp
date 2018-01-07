#include "SMAABlending.h"

cSMAABlending::cSMAABlending() : cPostEffects("SMAABlending.hlsl")
{
	m_pBufRenderTarget = new cRenderTargetTex;
	m_pBufRenderTarget->RTCreate((float)WINDOW_SIZE_X, (float)WINDOW_SIZE_Y, { 0,0,0 });
}

cSMAABlending::~cSMAABlending()
{
	delete m_pBufRenderTarget;
}

void cSMAABlending::DrawBlending(ID3D11ShaderResourceView * _color, ID3D11ShaderResourceView * _weight)
{
	ID3D11ShaderResourceView* Tex[] = {
		_color,_weight
	};
	GetDirectX::Context()->PSSetSamplers(NULL, NULL, NULL);
	m_pBufRenderTarget->SetRenderTargetTex();
	this->Draw(Tex, 2);
}

ID3D11ShaderResourceView * cSMAABlending::GetResourceView()
{
	return m_pBufRenderTarget->GetTextureResourceView();
}

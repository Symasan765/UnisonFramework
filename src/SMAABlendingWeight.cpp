#include "SMAABlendingWeight.h"

cSMAABlendingWeight::cSMAABlendingWeight() : cPostEffects("SMAABlendingWeight.hlsl")
{
	m_pBufRenderTarget = new cRenderTargetTex;
	m_pBufRenderTarget->RTCreate((float)WINDOW_SIZE_X, (float)WINDOW_SIZE_Y, { 0,0,0 });
}

cSMAABlendingWeight::~cSMAABlendingWeight()
{
	delete m_pBufRenderTarget;
}

void cSMAABlendingWeight::DrawBlendingWeight(ID3D11ShaderResourceView * _edge, ID3D11ShaderResourceView * area, ID3D11ShaderResourceView * search)
{
	ID3D11ShaderResourceView* Tex[] = {
		_edge,area,search
	};
	GetDirectX::Context()->PSSetSamplers(NULL, NULL, NULL);
	m_pBufRenderTarget->SetRenderTargetTex();
	this->Draw(Tex, 3);
}

ID3D11ShaderResourceView * cSMAABlendingWeight::GetResourceView()
{
	return m_pBufRenderTarget->GetTextureResourceView();
}

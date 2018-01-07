#include "ScreenSpaceSSS.h"
#include "WinMain.h"

cScreenSpaceSSS::cScreenSpaceSSS() : cPostEffects("ScreenSpaceSubsurfaceScattering.hlsl")
{
	m_pBufRenderTarget = new cRenderTargetTex;
	m_pBufRenderTarget->RTCreate((float)WINDOW_SIZE_X, (float)WINDOW_SIZE_Y, { 0,0,0 });
}

cScreenSpaceSSS::~cScreenSpaceSSS()
{
	delete m_pBufRenderTarget;
}

void cScreenSpaceSSS::DrawSSS(ID3D11ShaderResourceView * _sssss, ID3D11ShaderResourceView * _normal, ID3D11ShaderResourceView * _color)
{
	ID3D11ShaderResourceView* Tex[] = {
		_sssss,_normal,_color
	};
	this->SetTexSamplerState(0, LINEAR);
	this->SetTexSamplerState(1, POINT);
	m_pBufRenderTarget->SetRenderTargetTex();
	this->Draw(Tex, 3);
}

ID3D11ShaderResourceView * cScreenSpaceSSS::GetResourceView()
{
	return m_pBufRenderTarget->GetTextureResourceView();
}
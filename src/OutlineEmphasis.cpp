#include "OutlineEmphasis.h"

cOutlineEmphasis::cOutlineEmphasis() : cPostEffects("OutlineEmphasis.hlsl")
{
	m_pBufRenderTarget = new cRenderTargetTex;
	m_pBufRenderTarget->RTCreate((float)WINDOW_SIZE_X, (float)WINDOW_SIZE_Y, { 0,0,0 });
}

cOutlineEmphasis::~cOutlineEmphasis()
{
	delete m_pBufRenderTarget;
}

void cOutlineEmphasis::DrawOutLine(ID3D11ShaderResourceView * _color, ID3D11ShaderResourceView * _normal)
{
	ID3D11ShaderResourceView* Tex[] = {
		_color,_normal
	};
	this->SetTexSamplerState(0, LINEAR);
	m_pBufRenderTarget->SetRenderTargetTex();
	this->Draw(Tex, 2);
}

ID3D11ShaderResourceView * cOutlineEmphasis::GetResourceView()
{
	return m_pBufRenderTarget->GetTextureResourceView();
}

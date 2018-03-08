#include "OutlineEmphasis.h"

/// <summary>
/// 初期化
/// </summary>
cOutlineEmphasis::cOutlineEmphasis() : cPostEffects("OutlineEmphasis.hlsl")
{
	m_pBufRenderTarget = new cRenderTargetTex;
	m_pBufRenderTarget->RTCreate((float)WINDOW_SIZE_X, (float)WINDOW_SIZE_Y, { 0,0,0 });
}

/// <summary>
/// 解放処理
/// </summary>
cOutlineEmphasis::~cOutlineEmphasis()
{
	delete m_pBufRenderTarget;
}

/// <summary>
/// 輪郭線描画
/// </summary>
/// <param name="_color"></param>
/// <param name="_normal"></param>
void cOutlineEmphasis::DrawOutLine(ID3D11ShaderResourceView * _color, ID3D11ShaderResourceView * _normal)
{
	ID3D11ShaderResourceView* Tex[] = {
		_color,_normal
	};
	this->SetTexSamplerState(0, LINEAR);
	m_pBufRenderTarget->SetRenderTargetTex();
	this->Draw(Tex, 2);
}

/// <summary>
/// リソースビューの取得
/// </summary>
/// <returns></returns>
ID3D11ShaderResourceView * cOutlineEmphasis::GetResourceView()
{
	return m_pBufRenderTarget->GetTextureResourceView();
}

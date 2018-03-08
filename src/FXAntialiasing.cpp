#include "FXAntialiasing.h"

/// <summary>
/// 初期化
/// </summary>
cFXAA::cFXAA() : cPostEffects("FXAAShader.hlsl")
{
	m_pBufRenderTarget = new cRenderTargetTex;
	m_pBufRenderTarget->RTCreate((float)WINDOW_SIZE_X, (float)WINDOW_SIZE_Y, { 0,0,0 });
}

/// <summary>
/// 解放処理
/// </summary>
cFXAA::~cFXAA()
{
	delete m_pBufRenderTarget;
}

/// <summary>
/// アンチエイリアスをかける
/// </summary>
/// <param name="_color"></param>
void cFXAA::DrawFXAA(ID3D11ShaderResourceView * _color)
{
	this->SetTexSamplerState(0, LINEAR);
	m_pBufRenderTarget->SetRenderTargetTex();
	this->Draw(&_color);
}

/// <summary>
/// アンチエイリアスをかけたテクスチャを取得
/// </summary>
/// <returns></returns>
ID3D11ShaderResourceView * cFXAA::GetResourceView()
{
	return m_pBufRenderTarget->GetTextureResourceView();
}

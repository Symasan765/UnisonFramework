#include "FXAntialiasing.h"

/// <summary>
/// ������
/// </summary>
cFXAA::cFXAA() : cPostEffects("FXAAShader.hlsl")
{
	m_pBufRenderTarget = new cRenderTargetTex;
	m_pBufRenderTarget->RTCreate((float)WINDOW_SIZE_X, (float)WINDOW_SIZE_Y, { 0,0,0 });
}

/// <summary>
/// �������
/// </summary>
cFXAA::~cFXAA()
{
	delete m_pBufRenderTarget;
}

/// <summary>
/// �A���`�G�C���A�X��������
/// </summary>
/// <param name="_color"></param>
void cFXAA::DrawFXAA(ID3D11ShaderResourceView * _color)
{
	this->SetTexSamplerState(0, LINEAR);
	m_pBufRenderTarget->SetRenderTargetTex();
	this->Draw(&_color);
}

/// <summary>
/// �A���`�G�C���A�X���������e�N�X�`�����擾
/// </summary>
/// <returns></returns>
ID3D11ShaderResourceView * cFXAA::GetResourceView()
{
	return m_pBufRenderTarget->GetTextureResourceView();
}

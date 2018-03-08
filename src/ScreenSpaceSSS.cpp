#include "ScreenSpaceSSS.h"
#include "WinMain.h"

/// <summary>
/// ������
/// </summary>
cScreenSpaceSSS::cScreenSpaceSSS() : cPostEffects("ScreenSpaceSubsurfaceScattering.hlsl")
{
	m_pBufRenderTarget = new cRenderTargetTex;
	m_pBufRenderTarget->RTCreate((float)WINDOW_SIZE_X, (float)WINDOW_SIZE_Y, { 0,0,0 });
}

/// <summary>
/// �������
/// </summary>
cScreenSpaceSSS::~cScreenSpaceSSS()
{
	delete m_pBufRenderTarget;
}

/// <summary>
/// SS-SSS�`��
/// </summary>
/// <param name="_sssss"></param>
/// <param name="_normal"></param>
/// <param name="_color"></param>
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

/// <summary>
/// �V�F�[�_���\�[�X�r���[�擾
/// </summary>
/// <returns></returns>
ID3D11ShaderResourceView * cScreenSpaceSSS::GetResourceView()
{
	return m_pBufRenderTarget->GetTextureResourceView();
}
#include "OutlineEmphasis.h"

/// <summary>
/// ������
/// </summary>
cOutlineEmphasis::cOutlineEmphasis() : cPostEffects("OutlineEmphasis.hlsl")
{
	m_pBufRenderTarget = new cRenderTargetTex;
	m_pBufRenderTarget->RTCreate((float)WINDOW_SIZE_X, (float)WINDOW_SIZE_Y, { 0,0,0 });
}

/// <summary>
/// �������
/// </summary>
cOutlineEmphasis::~cOutlineEmphasis()
{
	delete m_pBufRenderTarget;
}

/// <summary>
/// �֊s���`��
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
/// ���\�[�X�r���[�̎擾
/// </summary>
/// <returns></returns>
ID3D11ShaderResourceView * cOutlineEmphasis::GetResourceView()
{
	return m_pBufRenderTarget->GetTextureResourceView();
}

#include "GaussianFilter.h"
#include "HighlightBloom.h"

/// <summary>
/// 受け取ったリソースビューを二回ぼかして保持する
/// </summary>
/// <param name="_EmiTex"></param>
void cHighlightBloom::DrawBloom(ID3D11ShaderResourceView * _EmiTex)
{
	cGaussianFilter::getInstance().DrawGaussianFilter(&m_pBoloomTex[0], _EmiTex);		//1回目のぼかし
	cGaussianFilter::getInstance().DrawGaussianFilter(&m_pBoloomTex[1], m_pBoloomTex[0].GetTextureResourceView());	//二回目のぼかし
}

ID3D11ShaderResourceView * cHighlightBloom::GetBloomResourceView()
{
	return m_pBoloomTex[1].GetTextureResourceView();
}

cHighlightBloom::cHighlightBloom()
{
	m_pBoloomTex = new cRenderTargetTex[2];		//書き込みと読み込みを一度ずつ行うためバッファが必要
	m_pBoloomTex[0].RTCreate(WINDOW_SIZE_X / 4, WINDOW_SIZE_Y / 4, { 0,0,0 });		//ぼかすので縮小バッファでOK
	m_pBoloomTex[1].RTCreate(WINDOW_SIZE_X / 4, WINDOW_SIZE_Y / 4, { 0,0,0 });		//ぼかすので縮小バッファでOK
}

cHighlightBloom::~cHighlightBloom()
{
	delete[] m_pBoloomTex;
	m_pBoloomTex = nullptr;
}

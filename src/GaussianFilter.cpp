/*=================================================
//								FILE : GaussianFilter.cpp
//		ファイル説明 :
//		ガウスフィルタを取り扱うクラスの内容を記述する
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "GaussianFilter.h"
#include "WinMain.h"
#include "TextureManager.h"

void cGaussianFilter::DrawGaussianFilter(cRenderTargetTex* outputTex, ID3D11ShaderResourceView * inputTex)
{
	GetDirectX::Context()->PSSetSamplers(0, 1,&m_Clamp);
	//まずは内部のレンダーターゲットへ切り替えを行ってからリソースをセットし、X方向にぼかす
	m_pBufRenderTarget->SetRenderTargetTex();
	m_pXBlur->Draw(&inputTex);
	outputTex->SetRenderTargetTex();
	ID3D11ShaderResourceView* tex = m_pBufRenderTarget->GetTextureResourceView();
	m_pYBlur->Draw(&tex);
	GetDirectX::Context()->PSSetSamplers(0, 1, &m_Wrap);
}

cGaussianFilter::cGaussianFilter()
{
	cTexManager::GetInstance().CreateTexSampler(&m_Clamp, cTexManager::SampleState::CLAMP);
	cTexManager::GetInstance().CreateTexSampler(&m_Wrap, cTexManager::SampleState::WRAP);
	m_pXBlur = new cPostEffects("GaussianFilterX.hlsl");
	m_pYBlur = new cPostEffects("GaussianFilterY.hlsl");
	m_pBufRenderTarget = new cRenderTargetTex();
	//レンダーターゲットはぼかしなので縮小バッファを利用する
	m_pBufRenderTarget->RTCreate((float)WINDOW_SIZE_X / 2.0f, (float)WINDOW_SIZE_Y / 2.0f, { 0,0,0 });
}

cGaussianFilter::~cGaussianFilter()
{
	delete m_pXBlur;
	delete m_pYBlur;
	delete m_pBufRenderTarget;
	SAFE_RELEASE(m_Clamp);
	SAFE_RELEASE(m_Wrap);
}

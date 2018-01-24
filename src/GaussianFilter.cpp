/*=================================================
//								FILE : GaussianFilter.cpp
//		�t�@�C������ :
//		�K�E�X�t�B���^����舵���N���X�̓��e���L�q����
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "GaussianFilter.h"
#include "WinMain.h"
#include "TextureManager.h"

void cGaussianFilter::DrawGaussianFilter(cRenderTargetTex* outputTex, ID3D11ShaderResourceView * inputTex)
{
	GetDirectX::Context()->PSSetSamplers(0, 1,&m_Clamp);
	//�܂��͓����̃����_�[�^�[�Q�b�g�֐؂�ւ����s���Ă��烊�\�[�X���Z�b�g���AX�����ɂڂ���
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
	//�����_�[�^�[�Q�b�g�͂ڂ����Ȃ̂ŏk���o�b�t�@�𗘗p����
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

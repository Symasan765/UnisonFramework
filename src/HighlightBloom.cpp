#include "GaussianFilter.h"
#include "HighlightBloom.h"

/// <summary>
/// �󂯎�������\�[�X�r���[����ڂ����ĕێ�����
/// </summary>
/// <param name="_EmiTex"></param>
void cHighlightBloom::DrawBloom(ID3D11ShaderResourceView * _EmiTex)
{
	cGaussianFilter::getInstance().DrawGaussianFilter(&m_pBoloomTex[0], _EmiTex);		//1��ڂ̂ڂ���
	cGaussianFilter::getInstance().DrawGaussianFilter(&m_pBoloomTex[1], m_pBoloomTex[0].GetTextureResourceView());	//���ڂ̂ڂ���
}

ID3D11ShaderResourceView * cHighlightBloom::GetBloomResourceView()
{
	return m_pBoloomTex[1].GetTextureResourceView();
}

cHighlightBloom::cHighlightBloom()
{
	m_pBoloomTex = new cRenderTargetTex[2];		//�������݂Ɠǂݍ��݂���x���s�����߃o�b�t�@���K�v
	m_pBoloomTex[0].RTCreate(WINDOW_SIZE_X / 4, WINDOW_SIZE_Y / 4, { 0,0,0 });		//�ڂ����̂ŏk���o�b�t�@��OK
	m_pBoloomTex[1].RTCreate(WINDOW_SIZE_X / 4, WINDOW_SIZE_Y / 4, { 0,0,0 });		//�ڂ����̂ŏk���o�b�t�@��OK
}

cHighlightBloom::~cHighlightBloom()
{
	delete[] m_pBoloomTex;
	m_pBoloomTex = nullptr;
}

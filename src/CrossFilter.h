/*=================================================
//								FILE : CrossFilter.h
//		�t�@�C������ :
//		���P�x���������L�΂��N���X�t�B���^�[�𔭐�������
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "PostEffects.h"
#include "RenderTarget.h"
#include "WinMain.h"
#include "ConstantBuffer.h"

#define MAX_CROSSFILTER_SAMPLES 8

//�N���X�t�B���^�[�ŗ��p����萔���
struct CrossFilterParam {
	DirectX::XMFLOAT2 avSampleOffsets[MAX_CROSSFILTER_SAMPLES];
	DirectX::XMFLOAT4 avSampleWeights[MAX_CROSSFILTER_SAMPLES];
};

class cCrossFilter : public cPostEffects {
public:
	cCrossFilter();
	~cCrossFilter();

	void DrawCrossFilter(ID3D11ShaderResourceView * inData);
	ID3D11ShaderResourceView * GetResourceView();
	void TestDraw();
private:
	void ColorCorrectionCalculation();
	void LightBeamInit();
	DirectX::XMFLOAT2 cCrossFilter::OffsetCalculation(DirectX::XMFLOAT2 vtStepUV, int LineNo, int samp);
	DirectX::XMFLOAT4 cCrossFilter::ColorWeighting(float PowScale, int i, int PassesNum);
	void cCrossFilter::SetRenderTarget(int Num, ID3D11ShaderResourceView * inData);

	static const int s_maxPasses = 3;
	static const int nSamples = 8;
	static const int m_StarNum = 4;
	DirectX::XMFLOAT4 m_aaColor[s_maxPasses][nSamples];		//�S�̂̃J���[�␳����ێ����Ă���
	DirectX::XMFLOAT2 m_BeamVec[4];				//��䊂̕���

	cRenderTargetTex* m_pBufRenderTarget;		//�ڂ������摜�������_�����O����e�N�X�`��
	cConstBuf<CrossFilterParam> m_CrossParam[4];	//�����̐������쐬
};
#include "CrossFilter.h"
#include "SpriteDraw.h"

using namespace DirectX;

/// <summary>
/// ������
/// </summary>
cCrossFilter::cCrossFilter() : cPostEffects("CrossFilter.hlsl")
{
	m_pBufRenderTarget = new cRenderTargetTex[m_StarNum * 2];		// �����_�[�^�[�Q�b�g�ƃ��\�[�X�����݂ɐ؂�ւ���̂Ō����̔{����Ă���
	for (int i = 0; i < m_StarNum * 2; i++) {
		m_pBufRenderTarget[i].RTCreate((FLOAT)WINDOW_SIZE_X / 2.0f, (FLOAT)WINDOW_SIZE_Y / 2.0f, {0,0,0 }, DXGI_FORMAT_R16G16B16A16_FLOAT);
	}
	ColorCorrectionCalculation();
	LightBeamInit();
}

/// <summary>
/// �������
/// </summary>
cCrossFilter::~cCrossFilter()
{
	delete[] m_pBufRenderTarget;
}

/// <summary>
/// �`��
/// </summary>
/// <param name="inData"></param>
void cCrossFilter::DrawCrossFilter(ID3D11ShaderResourceView * inData)
{
	// for���ň����L�΂��񐔕�����������A��������(3��)
	// ���̊ԂɃV�F�[�_���ŕK�v�Ȓ萔��萔�o�b�t�@�ɓn���Ă���

	float srcW = (FLOAT)WINDOW_SIZE_X / 2.0f;
	float srcH = (FLOAT)WINDOW_SIZE_Y / 2.0f;

	float attnPowScale = (atanf(XM_PI / 4) + 0.1f) * (640.0f + 360.0f) / (srcW + srcH);

	//�N���X�t�B���^�[�쐬
	cPostEffects::SetShader();
	// TODO �����L�΂������̃o�b�t�@�쐬
	XMFLOAT2 vtStepUV[4] = { { m_BeamVec[0] },{ m_BeamVec[1]},{ m_BeamVec[2]},{ m_BeamVec[3]} };
	cPostEffects::SetShader();
	for (int extend = 0; extend < s_maxPasses; extend++) {
		for (int LineNo = 0; LineNo < m_StarNum; LineNo++) {
			for (int samp = 0; samp < nSamples; samp++) {
				// TODO �萔�o�b�t�@�쐬
				m_CrossParam[LineNo].data.avSampleWeights[samp] = ColorWeighting(attnPowScale, samp, extend);
				m_CrossParam[LineNo].data.avSampleOffsets[samp] = OffsetCalculation(vtStepUV[LineNo], LineNo, samp);
			}	//end for �T���v�����O��
			m_CrossParam[LineNo].Set(LineNo + 1);	//�V�F�[�_�萔�o�b�t�@�p�X��0�����[���h�s�񂪎g���Ă���̂�1����

			vtStepUV[LineNo].x *= nSamples;
			vtStepUV[LineNo].y *= nSamples;
		}	//end for ���̐�

		//�����Ń����_�[�^�[�Q�b�g�̃Z�b�g�ƕ`����s��
		SetRenderTarget(extend, inData);

		attnPowScale *= nSamples;
	}	//end for �����L�΂���
}

/// <summary>
/// �`��f�[�^���擾����
/// </summary>
/// <returns></returns>
ID3D11ShaderResourceView * cCrossFilter::GetResourceView()
{
	return nullptr;
}

void cCrossFilter::TestDraw()
{
	XMFLOAT2 pos = { 0.0f,0.0f };

	for (int i = 0; i < 1; i++) {
		cSprite2DDraw::GetInstance().Draw(m_pBufRenderTarget[0].GetTextureResourceView(), { 0.0f,0.0f }, { 1280.0f,720.0f });
	}
}

DirectX::XMFLOAT4 cCrossFilter::ColorWeighting(float PowScale, int Samp, int PassesNum)
{
	float lum = powf(0.95f, PowScale * Samp);	//�����L�΂��񐔂��������0.95f���|����񐔂������� == �l���������Ȃ� == �Â��Ȃ�
	XMVECTOR color = XMLoadFloat4(&m_aaColor[s_maxPasses - 1 - PassesNum][Samp]) * lum * (PassesNum + 1.0f) * 0.5f;
	XMFLOAT4 ret = { 0,0,0,0 };
	XMStoreFloat4(&ret, color);
	return ret;
}

void cCrossFilter::SetRenderTarget(int Num, ID3D11ShaderResourceView * inData)
{
	// Num(�����L�΂���)�ɂ���ē��͂Əo�͂̃o�b�t�@�����݂ɐ؂�ւ���
	int set = (Num % 2) * m_StarNum;	//1��ڂ�0 ~ 3���o�͂ɂȂ�

	ID3D11ShaderResourceView* Tex[] = {
		m_pBufRenderTarget[set + 0].GetTextureResourceView(),
		m_pBufRenderTarget[set + 1].GetTextureResourceView(),
		m_pBufRenderTarget[set + 2].GetTextureResourceView(),
		m_pBufRenderTarget[set + 3].GetTextureResourceView()
	};
	if (Num == 0) {
		for (int i = 0; i < m_StarNum; i++)
			Tex[i] = inData;
	}

	RTTex Target[m_StarNum];
	for (int i = 0; i < m_StarNum; i++) {
		//�����_�[�^�[�Q�b�g�̏����擾����
		Target[i] = m_pBufRenderTarget[(4 + i) - set].GetRenderTextureStruct();
	}
	D3D11_VIEWPORT _VP[m_StarNum];
	ID3D11RenderTargetView* _RTV[m_StarNum];
	ID3D11DepthStencilView* _DSV[m_StarNum];

	for (int i = 0; i < m_StarNum; i++) {
		_VP[i] = Target[i].m_ViewPort;
		_RTV[i] = Target[i].m_pDepthRenderTargetView;
		_DSV[i] = Target[i].m_pDepthStencilView;
	}
	//===================�`��^�[�Q�b�g��ύX����=======================//
	GetDirectX::Context()->RSSetViewports(m_StarNum, _VP);
	GetDirectX::Context()->OMSetRenderTargets(m_StarNum, _RTV, *_DSV);
	for (int i = 0; i < m_StarNum; i++) {
		GetDirectX::Context()->ClearRenderTargetView(_RTV[i], Target[i].ClereColor);//��ʃN���A
		GetDirectX::Context()->ClearDepthStencilView(_DSV[i], D3D11_CLEAR_DEPTH, 1.0f, 0);//�[�x�o�b�t�@�N���A
	}

	cPostEffects::Draw(Tex, 4);
}

void cCrossFilter::ColorCorrectionCalculation()
{
	// ��䊂̐F
	const XMVECTOR s_colorWhite{ 0.63f, 0.63f, 0.63f, 1.0f };
	const XMVECTOR s_ChromaticAberrationColor[8] = {
		XMVECTOR({0.5f, 0.5f, 0.5f,  1.0f}),	// ��
		XMVECTOR({0.8f, 0.3f, 0.3f,  1.0f}), // ��
		XMVECTOR({1.0f, 0.2f, 0.2f,  1.0f}),	// ��
		XMVECTOR({0.5f, 0.2f, 0.6f,  1.0f}), // ��
		XMVECTOR({0.2f, 0.2f, 1.0f,  1.0f}),	// ��
		XMVECTOR({0.2f, 0.3f, 0.7f,  1.0f}), // ��
		XMVECTOR({0.2f, 0.6f, 0.2f,  1.0f}),	// ��
		XMVECTOR({0.3f, 0.5f, 0.3f,  1.0f}), // ��
	};

	for (int p = 0; p < s_maxPasses; p++) {
		// ���S����̋����ɉ����Č�䊂̐F������
		float ratio = (float)(p + 1) / (float)s_maxPasses;
		// ���ꂼ��̃T���v�����O�œK���ɐF������
		for (int s = 0; s < nSamples; s++) {
			XMVECTOR chromaticAberrColor = XMVectorLerp(s_ChromaticAberrationColor[s], s_colorWhite, ratio);
			XMVECTOR LastColor = XMVectorLerp(s_colorWhite, chromaticAberrColor, 0.7f);
			XMStoreFloat4(&m_aaColor[p][s], LastColor);
		}
	}
}

void cCrossFilter::LightBeamInit()
{
	m_BeamVec[0] = { 0.000222f,0.000132f };
	m_BeamVec[1] = { 0.000176f,-0.000276f };
	m_BeamVec[2] = { -0.000222f,-0.000132f };
	m_BeamVec[1] = { -0.000176f,0.000276f };
}

DirectX::XMFLOAT2 cCrossFilter::OffsetCalculation(DirectX::XMFLOAT2 vtStepUV,int LineNo,int samp)
{
	DirectX::XMFLOAT2 ret = { 0.0f,0.0f };
	ret.x = vtStepUV.x * samp;
	ret.y = vtStepUV.y * samp;
	if (0.9f <= fabs(ret.x) ||
		0.9f <= fabs(ret.y)) {
		ret.x = 0.0f;
		ret.y = 0.0f;
		m_CrossParam[LineNo].data.avSampleWeights[samp] = { 0.0f,0.0f,0.0f,0.0f };
	}
	return ret;
}

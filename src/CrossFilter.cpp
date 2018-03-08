#include "CrossFilter.h"
#include "SpriteDraw.h"

using namespace DirectX;

/// <summary>
/// 初期化
/// </summary>
cCrossFilter::cCrossFilter() : cPostEffects("CrossFilter.hlsl")
{
	m_pBufRenderTarget = new cRenderTargetTex[m_StarNum * 2];		// レンダーターゲットとリソースを交互に切り替えるので光線の倍作っておく
	for (int i = 0; i < m_StarNum * 2; i++) {
		m_pBufRenderTarget[i].RTCreate((FLOAT)WINDOW_SIZE_X / 2.0f, (FLOAT)WINDOW_SIZE_Y / 2.0f, {0,0,0 }, DXGI_FORMAT_R16G16B16A16_FLOAT);
	}
	ColorCorrectionCalculation();
	LightBeamInit();
}

/// <summary>
/// 解放処理
/// </summary>
cCrossFilter::~cCrossFilter()
{
	delete[] m_pBufRenderTarget;
}

/// <summary>
/// 描画
/// </summary>
/// <param name="inData"></param>
void cCrossFilter::DrawCrossFilter(ID3D11ShaderResourceView * inData)
{
	// for分で引き伸ばし回数分だけ処理を連続させる(3回分)
	// その間にシェーダ側で必要な定数を定数バッファに渡していく

	float srcW = (FLOAT)WINDOW_SIZE_X / 2.0f;
	float srcH = (FLOAT)WINDOW_SIZE_Y / 2.0f;

	float attnPowScale = (atanf(XM_PI / 4) + 0.1f) * (640.0f + 360.0f) / (srcW + srcH);

	//クロスフィルター作成
	cPostEffects::SetShader();
	// TODO 引き伸ばし方向のバッファ作成
	XMFLOAT2 vtStepUV[4] = { { m_BeamVec[0] },{ m_BeamVec[1]},{ m_BeamVec[2]},{ m_BeamVec[3]} };
	cPostEffects::SetShader();
	for (int extend = 0; extend < s_maxPasses; extend++) {
		for (int LineNo = 0; LineNo < m_StarNum; LineNo++) {
			for (int samp = 0; samp < nSamples; samp++) {
				// TODO 定数バッファ作成
				m_CrossParam[LineNo].data.avSampleWeights[samp] = ColorWeighting(attnPowScale, samp, extend);
				m_CrossParam[LineNo].data.avSampleOffsets[samp] = OffsetCalculation(vtStepUV[LineNo], LineNo, samp);
			}	//end for サンプリング回数
			m_CrossParam[LineNo].Set(LineNo + 1);	//シェーダ定数バッファパスの0をワールド行列が使っているので1から

			vtStepUV[LineNo].x *= nSamples;
			vtStepUV[LineNo].y *= nSamples;
		}	//end for 線の数

		//ここでレンダーターゲットのセットと描画を行う
		SetRenderTarget(extend, inData);

		attnPowScale *= nSamples;
	}	//end for 引き伸ばし回数
}

/// <summary>
/// 描画データを取得する
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
	float lum = powf(0.95f, PowScale * Samp);	//引き伸ばし回数が増えれば0.95fを掛ける回数が増える == 値が小さくなる == 暗くなる
	XMVECTOR color = XMLoadFloat4(&m_aaColor[s_maxPasses - 1 - PassesNum][Samp]) * lum * (PassesNum + 1.0f) * 0.5f;
	XMFLOAT4 ret = { 0,0,0,0 };
	XMStoreFloat4(&ret, color);
	return ret;
}

void cCrossFilter::SetRenderTarget(int Num, ID3D11ShaderResourceView * inData)
{
	// Num(引き伸ばし回数)によって入力と出力のバッファを交互に切り替える
	int set = (Num % 2) * m_StarNum;	//1回目は0 ~ 3が出力になる

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
		//レンダーターゲットの情報を取得する
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
	//===================描画ターゲットを変更する=======================//
	GetDirectX::Context()->RSSetViewports(m_StarNum, _VP);
	GetDirectX::Context()->OMSetRenderTargets(m_StarNum, _RTV, *_DSV);
	for (int i = 0; i < m_StarNum; i++) {
		GetDirectX::Context()->ClearRenderTargetView(_RTV[i], Target[i].ClereColor);//画面クリア
		GetDirectX::Context()->ClearDepthStencilView(_DSV[i], D3D11_CLEAR_DEPTH, 1.0f, 0);//深度バッファクリア
	}

	cPostEffects::Draw(Tex, 4);
}

void cCrossFilter::ColorCorrectionCalculation()
{
	// 光芒の色
	const XMVECTOR s_colorWhite{ 0.63f, 0.63f, 0.63f, 1.0f };
	const XMVECTOR s_ChromaticAberrationColor[8] = {
		XMVECTOR({0.5f, 0.5f, 0.5f,  1.0f}),	// 白
		XMVECTOR({0.8f, 0.3f, 0.3f,  1.0f}), // 赤
		XMVECTOR({1.0f, 0.2f, 0.2f,  1.0f}),	// 赤
		XMVECTOR({0.5f, 0.2f, 0.6f,  1.0f}), // 紫
		XMVECTOR({0.2f, 0.2f, 1.0f,  1.0f}),	// 青
		XMVECTOR({0.2f, 0.3f, 0.7f,  1.0f}), // 青
		XMVECTOR({0.2f, 0.6f, 0.2f,  1.0f}),	// 緑
		XMVECTOR({0.3f, 0.5f, 0.3f,  1.0f}), // 緑
	};

	for (int p = 0; p < s_maxPasses; p++) {
		// 中心からの距離に応じて光芒の色をつける
		float ratio = (float)(p + 1) / (float)s_maxPasses;
		// それぞれのサンプリングで適当に色をつける
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

/*=================================================
//								FILE : CrossFilter.h
//		ファイル説明 :
//		高輝度部を引き伸ばしクロスフィルターを発生させる
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "PostEffects.h"
#include "RenderTarget.h"
#include "WinMain.h"
#include "ConstantBuffer.h"

#define MAX_CROSSFILTER_SAMPLES 8

//クロスフィルターで利用する定数情報
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
	DirectX::XMFLOAT4 m_aaColor[s_maxPasses][nSamples];		//全体のカラー補正情報を保持しておく
	DirectX::XMFLOAT2 m_BeamVec[4];				//光芒の方向

	cRenderTargetTex* m_pBufRenderTarget;		//ぼかした画像をレンダリングするテクスチャ
	cConstBuf<CrossFilterParam> m_CrossParam[4];	//光線の数だけ作成
};
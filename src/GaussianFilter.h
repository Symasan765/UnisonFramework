/*=================================================
//								FILE : GaussianFilter.h
//		ファイル説明 :
//		ガウスフィルタを取り扱うクラスを記述する
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "PostEffects.h"
#include "RenderTarget.h"

class cGaussianFilter {
public:
	cGaussianFilter(const cGaussianFilter&) = delete;			//コピーコンストラクタを delete 指定
	cGaussianFilter& operator=(const cGaussianFilter&) = delete;	//コピー代入演算子も delete 指定
	cGaussianFilter(cGaussianFilter&&) = delete;			//ムーブコンストラクタを delete 指定
	cGaussianFilter& operator=(cGaussianFilter&&) = delete;		//ムーブ代入演算子も delete 指定

	static cGaussianFilter& getInstance() {
		static cGaussianFilter inst;			// privateなコンストラクタを呼び出す
		return inst;
	}

	void DrawGaussianFilter(cRenderTargetTex* outputTex, ID3D11ShaderResourceView* inputTex);
private:
	cGaussianFilter();					//コンストラクタを private に置く
	~cGaussianFilter();					//デストラクタを private に置く

	cPostEffects* m_pXBlur;		//横方向のぼかし
	cPostEffects* m_pYBlur;		//縦方向のぼかし

	cRenderTargetTex* m_pBufRenderTarget;		//一時的に描画を行うテクスチャ

	ID3D11SamplerState* m_Clamp;
	ID3D11SamplerState* m_Wrap;
};
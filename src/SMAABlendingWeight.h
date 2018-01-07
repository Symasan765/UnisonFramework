/*=================================================
//								FILE : SMAABlendingWeight.h
//		ファイル説明 :
//		SMAAの第二パス。境界線の重みづけを行う
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "PostEffects.h"
#include "RenderTarget.h"

class cSMAABlendingWeight : public cPostEffects {
public:
	cSMAABlendingWeight();
	~cSMAABlendingWeight();

	void DrawBlendingWeight(ID3D11ShaderResourceView * _edge, ID3D11ShaderResourceView * area, ID3D11ShaderResourceView * search);
	ID3D11ShaderResourceView * GetResourceView();
private:
	cRenderTargetTex* m_pBufRenderTarget;		//一時的に描画を行うテクスチャ
};
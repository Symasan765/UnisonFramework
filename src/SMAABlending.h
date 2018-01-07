/*=================================================
//								FILE : SMAABlending.h
//		ファイル説明 :
//		SMAAにおける最終パスのブレンディング計算を行う
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "PostEffects.h"
#include "RenderTarget.h"

class cSMAABlending : public cPostEffects {
public:
	cSMAABlending();
	~cSMAABlending();

	void DrawBlending(ID3D11ShaderResourceView * _color, ID3D11ShaderResourceView * _weight);
	ID3D11ShaderResourceView * GetResourceView();
private:
	cRenderTargetTex* m_pBufRenderTarget;		//一時的に描画を行うテクスチャ
};
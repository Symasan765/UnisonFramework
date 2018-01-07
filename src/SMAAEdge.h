/*=================================================
//								FILE : SMAAEdge.h
//		ファイル説明 :
//		SMAAにおける第一パスのエッジ検出を行う
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "PostEffects.h"
#include "RenderTarget.h"

class cSMAAEdge : public cPostEffects {
public:
	cSMAAEdge();
	~cSMAAEdge();

	void DrawEdge(ID3D11ShaderResourceView * _color);
	ID3D11ShaderResourceView * GetResourceView();
private:
	cRenderTargetTex* m_pBufRenderTarget;		//一時的に描画を行うテクスチャ
};
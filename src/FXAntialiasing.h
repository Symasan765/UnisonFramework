/*=================================================
//								FILE : FXAntialiasing.h
//		ファイル説明 :
//		FXAAを処理するクラス
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once

#include "PostEffects.h"
#include "RenderTarget.h"

class cFXAA : public cPostEffects {
public:
	cFXAA();
	~cFXAA();

	void DrawFXAA(ID3D11ShaderResourceView * _color);
	ID3D11ShaderResourceView * GetResourceView();
private:
	cRenderTargetTex* m_pBufRenderTarget;		//一時的に描画を行うテクスチャ
};
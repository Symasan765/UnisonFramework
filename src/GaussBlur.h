/*=================================================
//								FILE : GaussBlur.h
//		ファイル説明 :
//		ガウスぼかしを扱うクラス。１パスでガウスフィルタを行う
//		2017/12/25現在で1280 : 720 のぼかしを行うことを前提にしている
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "PostEffects.h"
#include "RenderTarget.h"

class cGaussBlur : public cPostEffects {
public:
	cGaussBlur();
	~cGaussBlur();

	void DrawGaussBlur(ID3D11ShaderResourceView * inData);
	ID3D11ShaderResourceView * GetResourceView();
private:
	cRenderTargetTex* m_pBufRenderTarget;		//ぼかした画像をレンダリングするテクスチャ
};
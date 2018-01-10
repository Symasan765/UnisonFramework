/*=================================================
//								FILE : OutlineEmphasis.h
//		ファイル説明 :
//		輪郭線強調処理を行うシェーダクラス
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "PostEffects.h"
#include "RenderTarget.h"

class cOutlineEmphasis : public cPostEffects {
public:
	cOutlineEmphasis();
	~cOutlineEmphasis();

	void DrawOutLine(ID3D11ShaderResourceView * _color, ID3D11ShaderResourceView * _normal);
	ID3D11ShaderResourceView * GetResourceView();
private:
	cRenderTargetTex* m_pBufRenderTarget;		//一時的に描画を行うテクスチャ
};
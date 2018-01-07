/*=================================================
//								FILE : ScreenSpaceSSS.h
//		ファイル説明 :
//		スクリーンスペースサブサーフェススキャッタリングのポストエフェクト処理
//		を行うシェーダを保持するクラス
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "PostEffects.h"
#include "RenderTarget.h"

class cScreenSpaceSSS : public cPostEffects{
public:
	cScreenSpaceSSS();
	~cScreenSpaceSSS();

	void DrawSSS(ID3D11ShaderResourceView * _sssss, ID3D11ShaderResourceView * _normal, ID3D11ShaderResourceView * _color);
	ID3D11ShaderResourceView * GetResourceView();
private:
	cRenderTargetTex* m_pBufRenderTarget;		//一時的に描画を行うテクスチャ
};
/*=================================================
//								FILE : SMAntialiasing.h
//		ファイル説明 :
//		SMAAの処理を行うシェーダを実装する
//		http://www.iryoku.com/smaa/
//		http://hikita12312.hatenablog.com/entry/2017/12/16/160429
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "PostEffects.h"
#include "SMAAEdge.h"
#include "SMAABlendingWeight.h"
#include "SMAABlending.h"

//SMAA処理を行うクラス
class cSMAntialiasing {
public:
	cSMAntialiasing();
	~cSMAntialiasing();

	void DrawSMAA(ID3D11ShaderResourceView * _color);

	ID3D11ShaderResourceView* Get();
private:
	HRESULT CreateSMAATexture();
	cSMAAEdge* m_pEdge;
	cSMAABlendingWeight* m_pBlendingWeight;
	cSMAABlending* m_pBlending;

	ID3D11Texture2D* pAreaTex;
	ID3D11ShaderResourceView* pAreaSRV;

	ID3D11Texture2D* pSearchTex;
	ID3D11ShaderResourceView* pSearchSRV;
};
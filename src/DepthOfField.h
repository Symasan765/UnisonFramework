/*=================================================
//								FILE : DepthOfField.h
//		ファイル説明 :
//		被写界深度処理を取り扱う
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "PostEffects.h"
#include "RenderTarget.h"
#include "GaussBlur.h"
#include "LuaActor.h"

class cDepthOfField : public cPostEffects {
public:
	cDepthOfField();
	~cDepthOfField();

	void DrawDOF(ID3D11ShaderResourceView * color, ID3D11ShaderResourceView * _normal, const DoFData& data);
	ID3D11ShaderResourceView * GetResourceView();
private:
	cConstBuf<DoFData> m_ShaderConst;
	cRenderTargetTex* m_pBufRenderTarget;		//一時的に描画を行うテクスチャ
	cGaussBlur* m_GauseBlue;
	cLuaActor* m_LuaAct;
};
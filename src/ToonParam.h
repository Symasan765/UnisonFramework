/*=================================================
//								FILE : ToonParam.h
//		ファイル説明 :
//		トゥーンレンダリング用のパラメータ情報を取り扱う
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "LuaActor.h"
#include "ShaderStruct.h"
#include "ConstantBuffer.h"

class cToonParam {
public:
	cToonParam();
	~cToonParam();
	void ToonParamUpdate();
	void SetLightConstantBuffer(const int PathNo);
	void SetOutLineConstantBuffer(const int PathNo);
private:
	void ToonParamInit();
	void LightingParamUpdate();
	void DrawGuidelines();
	void OutLineUpdate();
	cConstBuf<ToonLightingStruct> m_LightParam;
	cConstBuf<OutlineParam> m_OutLineParam;
	cLuaActor* m_LuaAct;


	bool m_GuidelinesFlag;		//パラメータのガイドラインを変更するかどうかのフラグ
};
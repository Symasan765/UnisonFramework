/*=================================================
//								FILE : BackGroundModel.h
//		ファイル説明 :
//		背景モデルを描画する
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "LuaActor.h"
#include "DeferredBaseModel.h"

#define STAGE_SIZE 85	//ステージの縦横幅

class cBacGroundModel {
public:
	cBacGroundModel();
	~cBacGroundModel();
	void Draw();
	void Update();
private:
	std::string GetLuaFuncName(int no);
	cLuaActor* m_LuaAct;	//Luaの関数を実行する
	cDeferredModel* m_Models;
	int m_StageData[STAGE_SIZE * STAGE_SIZE];
};
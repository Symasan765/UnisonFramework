/*=================================================
//								FILE : MainScene.h
//		ファイル説明 :
//		ゲーム開始時のシーン情報を記述する
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "GameScene.h"
#include "DeferredBaseModel.h"

class cMainScene : public cGameScene {
public:
	cMainScene();
	~cMainScene();
	void Update();
	void MeshDraw();
	void UIDraw();
private:
	cDeferredModel* m_pModel;
};
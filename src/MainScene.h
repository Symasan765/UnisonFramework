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
#include "Player.h"
#include "BackGroundModel.h"
#include "Camera.h"
#include "CameraManager.h"

class cMainScene : public cGameScene {
public:
	cMainScene();
	~cMainScene();
	void Update();
	void MeshDraw();
	void UIDraw();
private:
	cDeferredModel* m_pGround;
	cPlayer* m_pPlayer;
	cBacGroundModel* m_BackGround;
	cCameraManager* m_CameraManager;
};
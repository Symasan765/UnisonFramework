/*=================================================
//								FILE : MainScene.h
//		�t�@�C������ :
//		�Q�[���J�n���̃V�[�������L�q����
//
//							HAL��� : ���{ �Y�V��
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
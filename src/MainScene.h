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
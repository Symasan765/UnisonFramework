/*=================================================
//								FILE : MainScene.cpp
//		ファイル説明 :
//
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "MainScene.h"

cMainScene::cMainScene()
{
	m_pGround = new cDeferredModel;
	m_pGround->LoadData("Ground.x", 1);
	//m_pGround->SetScaling(0.01f);
	m_DirectionalLight.SetLightPos(DirectX::XMFLOAT4{ 1.0f,-1.0f,0.0f,1.0f });

	m_pPlayer = new cPlayer;

	m_BackGround = new cBacGroundModel;

	m_CameraData.SetPosition({ -14.0f,1.19f,8.07f });
	m_CameraData.SetLookPoint({ -14.79f,1.16f,7.37f });

	m_CameraManager = new cCameraManager(m_pPlayer->GetPosition());
}

cMainScene::~cMainScene()
{
	delete m_pGround;
	delete m_pPlayer;
	delete m_BackGround;
	delete m_CameraManager;
}

void cMainScene::Update()
{
	m_pPlayer->SetCameraData(m_CameraData.GetViewProj(false));
	m_pPlayer->Update();
	m_BackGround->Update();

	m_CameraManager->SetPlayerPos(m_pPlayer->GetPosition());
	m_CameraManager->Update();


	//最後にカメラマネージャからカメラへ座標情報を渡す
	m_CameraManager->PopCameraPos(&m_CameraData);
}

void cMainScene::MeshDraw()
{
	m_pGround->DrawMesh();
	m_pPlayer->Draw();
	m_BackGround->Draw();
}

void cMainScene::UIDraw()
{
}

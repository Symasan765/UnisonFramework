/*=================================================
//								FILE : MainScene.cpp
//		ƒtƒ@ƒCƒ‹à–¾ :
//
//
//							HAL‘åã : ¼–{ —Y”V‰î
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
/*
	m_CameraData.SetPosition({ 0.0f,1.0f,0.0f });
	m_CameraData.SetUpVect({ 0.0f,0.0f,1.0f });
	m_CameraData.SetLookPoint({ 0.0f,0,0 });*/
}

cMainScene::~cMainScene()
{
	delete m_pGround;
	delete m_pPlayer;
}

void cMainScene::Update()
{
	m_pPlayer->SetCameraData(m_CameraData.GetViewProj(false));
	m_pPlayer->Update();
	m_BackGround->Update();

	DirectX::XMFLOAT3 pPos = m_pPlayer->GetPosition();
	m_CameraData.SetPosition({ pPos.x,pPos.y + 3.0f,pPos.z + 5.0f });
	m_CameraData.SetLookPoint({ pPos.x,pPos.y + 1.0f,pPos.z });
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

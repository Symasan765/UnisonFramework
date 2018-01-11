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
	m_pModel = new cDeferredModel;
	m_pModel->LoadData("ToonSkyDome.x", 1);
	m_DirectionalLight.SetLightPos(DirectX::XMFLOAT4{ 1.0f,0.0f,0.0f,1.0f });
	m_pModel->Rotation(-90.0f, 0.0, 0.0f);
	m_pModel->Scaling(50.0f);
	
	m_CameraData.SetPosition({ 0.0f,10.0f,-80.0f });
	m_CameraData.SetLookPoint({ 0.0f,10.0f,0.0f });
	m_CameraData.SetNear(1.0f, 10000.0f);

	m_CameraData.SetPosition({ 0.0f,0.2f,-2.0f });
	m_CameraData.SetLookPoint({ 0.0f,0.2f,0.0f });
}

cMainScene::~cMainScene()
{
	delete m_pModel;
}

void cMainScene::Update()
{
	//m_pModel->Rotation(0.0f, 1.0f, 0.0f);
}

void cMainScene::MeshDraw()
{
	static int a = 0;
	m_pModel->DrawMesh(0,a);
	a++;
}

void cMainScene::UIDraw()
{
}

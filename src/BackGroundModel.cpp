#include "BackGroundModel.h"
#include <fstream>
using namespace std;

cBacGroundModel::cBacGroundModel()
{
	m_Models = new cDeferredModel[2];
	m_LuaAct = new cLuaActor("GameObject/BackGroundModel.lua");
	//===========
	
	/*m_Models->LoadData("CityData.x", 1);
	m_Models->SetScaling(0.001f);
	m_Models->AddFunctionToLua(m_LuaAct->GetLuaState(), "City");
	m_LuaAct->ScriptCommit();*/
	
	//==========

	m_Models[1].LoadData("box.x", 0);
}

cBacGroundModel::~cBacGroundModel()
{
	delete[] m_Models;
	delete m_LuaAct;
}

void cBacGroundModel::Draw()
{
	//m_LuaAct->CallFunc("CityDraw");

	//���̒ʘH������
	m_Models[1].SetTrans({ -18.0,0,-45.0 });	//�E��
	m_Models[1].DrawMesh();
	m_Models[1].SetTrans({ -37.0,0,-45.0 });	//����
	m_Models[1].DrawMesh();
	m_Models[1].SetTrans({ -37.0,0,1.0 });		//����
	m_Models[1].DrawMesh();
	m_Models[1].SetTrans({ -18.0,0,1.0 });		//�E��
	m_Models[1].DrawMesh();

	//�L��
	m_Models[1].SetTrans({ -37.0,0,33.0 });		//����
	m_Models[1].DrawMesh();
	m_Models[1].SetTrans({ 42.0,0,33.0 });		//����
	m_Models[1].DrawMesh();
	m_Models[1].SetTrans({ 42.0,0,1.0 });		//����
	m_Models[1].DrawMesh();
}

void cBacGroundModel::Update()
{
	
}
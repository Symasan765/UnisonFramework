#include "BackGroundModel.h"
#include <fstream>
using namespace std;

/// <summary>
/// �w�i�I�u�W�F�N�g�̃��[�h���s������������
/// </summary>
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

/// <summary>
/// �������
/// </summary>
cBacGroundModel::~cBacGroundModel()
{
	delete[] m_Models;
	delete m_LuaAct;
}

/// <summary>
/// �`��
/// </summary>
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

/// <summary>
/// �X�V����
/// </summary>
void cBacGroundModel::Update()
{
	
}
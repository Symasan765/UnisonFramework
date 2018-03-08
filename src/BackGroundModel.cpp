#include "BackGroundModel.h"
#include <fstream>
using namespace std;

/// <summary>
/// 背景オブジェクトのロードを行う初期化処理
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
/// 解放処理
/// </summary>
cBacGroundModel::~cBacGroundModel()
{
	delete[] m_Models;
	delete m_LuaAct;
}

/// <summary>
/// 描画
/// </summary>
void cBacGroundModel::Draw()
{
	//m_LuaAct->CallFunc("CityDraw");

	//下の通路側から
	m_Models[1].SetTrans({ -18.0,0,-45.0 });	//右下
	m_Models[1].DrawMesh();
	m_Models[1].SetTrans({ -37.0,0,-45.0 });	//左下
	m_Models[1].DrawMesh();
	m_Models[1].SetTrans({ -37.0,0,1.0 });		//左上
	m_Models[1].DrawMesh();
	m_Models[1].SetTrans({ -18.0,0,1.0 });		//右上
	m_Models[1].DrawMesh();

	//広場
	m_Models[1].SetTrans({ -37.0,0,33.0 });		//左上
	m_Models[1].DrawMesh();
	m_Models[1].SetTrans({ 42.0,0,33.0 });		//左上
	m_Models[1].DrawMesh();
	m_Models[1].SetTrans({ 42.0,0,1.0 });		//左上
	m_Models[1].DrawMesh();
}

/// <summary>
/// 更新処理
/// </summary>
void cBacGroundModel::Update()
{
	
}
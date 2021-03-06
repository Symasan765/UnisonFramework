/*=================================================
//								FILE : BackGroundModel.h
//		t@CΰΎ :
//		wifπ`ζ·ι
//
//							HALεγ : Ό{ YVξ
=================================================*/
#pragma once
#include "LuaActor.h"
#include "DeferredBaseModel.h"

#define STAGE_SIZE 85	//Xe[WΜc‘

class cBacGroundModel {
public:
	cBacGroundModel();
	~cBacGroundModel();
	void Draw();
	void Update();
private:
	std::string GetLuaFuncName(int no);
	cLuaActor* m_LuaAct;	//LuaΜΦπΐs·ι
	cDeferredModel* m_Models;
	int m_StageData[STAGE_SIZE * STAGE_SIZE];
};
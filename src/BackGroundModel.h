/*=================================================
//								FILE : BackGroundModel.h
//		�t�@�C������ :
//		�w�i���f����`�悷��
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "LuaActor.h"
#include "DeferredBaseModel.h"

#define STAGE_SIZE 85	//�X�e�[�W�̏c����

class cBacGroundModel {
public:
	cBacGroundModel();
	~cBacGroundModel();
	void Draw();
	void Update();
private:
	std::string GetLuaFuncName(int no);
	cLuaActor* m_LuaAct;	//Lua�̊֐������s����
	cDeferredModel* m_Models;
	int m_StageData[STAGE_SIZE * STAGE_SIZE];
};
/*=================================================
//								FILE : Player.h
//		ファイル説明 :
//		プレイヤーの処理を記述する
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "DeferredBaseModel.h"
#include "LuaActor.h"

class cPlayer : public cDeferredModel{
public:
	cPlayer();
	~cPlayer();
	void Draw();
	void Update();	//これより前にカメラのアップデートをかける
	void SetCameraData(ViewProj data);
private:
	void Move();
	void PosClamp();
	void RotationCalculation(DirectX::XMFLOAT2 ProgressDir);
	DirectX::XMFLOAT3 MoveCorrection(DirectX::XMFLOAT2 ProgressDir);
	cLuaActor* m_LuaScript;
	ViewProj m_CameraData;
	int m_anmCnt;
};
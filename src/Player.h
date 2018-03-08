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

	enum MoveState {
		WALK = 0,
		DASH,
		WAIT
	};
private:
	void PlayerParamUpdate();
	void Move();
	void PosClamp();
	void InputAngleCorrection(DirectX::XMFLOAT2 inp);
	void RotationCalculation(DirectX::XMFLOAT2 ProgressDir);
	void PlayerVectChange();
	DirectX::XMFLOAT2 MoveCorrection(DirectX::XMFLOAT2 ProgressDir,const float speed);
	void MovingSpeedClamp();
	cLuaActor* m_LuaScript;
	ViewProj m_CameraData;

	//状態
	float m_ConstScal;	//モデルのサイズを変える必要があるため本番環境では不要になると思われる
	float m_anmCnt;
	float m_NowSpeed;		//移動方向ベクトルを管理する
	DirectX::XMFLOAT2 m_NowVect;			//キャラクターの方向を管理する

	//パラメータ
	float m_DashAddSpeed;
	float m_WalkAddSpeed;
	float m_MaxDashSpeed;
	float m_MaxWalkSpeed;
	float m_Switching;
	float m_Accele;
	float m_Decele;
	float m_MaxRotatAngle;

	MoveState m_MoveState;

};
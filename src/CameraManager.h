#pragma once
#include "WinMain.h"
#include "LuaActor.h"
#include "Camera.h"
#include <deque>

/// <summary>
/// プレイヤーから情報を受け取りカメラ情報の処理を行うクラス
/// </summary>
class cCameraManager {
public:
	cCameraManager(DirectX::XMFLOAT3 playerPos);
	~cCameraManager();
	DirectX::XMFLOAT3 PopCameraPos(cCamera* pCamera);
	void Update();
	void SetPlayerPos(DirectX::XMFLOAT3);
private:
	void CameraParamUpdate();
	void SpeedAdjustment();
	DirectX::XMFLOAT3 CoordinateCalculation(DirectX::XMFLOAT3 playerPos);

	std::deque<DirectX::XMFLOAT3> m_PlayerPosArry;	//プレイヤーの座標を溜めていく
	DirectX::XMFLOAT3 m_CameraOffset;
	cLuaActor* m_LuaAct;
	int m_DelayNum;		//カメラの遅延数

	//回転に関するパラメータ
	float m_CameraRotPosY;
	float m_CameraRotPosX;

	float m_NowCameraSpeedY;
	float m_NowCameraSpeedX;

	float m_MaxRotAngleY;
	float m_MaxRotAngleX;

	float m_NowRotAngY;
	float m_NowRotAngX;

	float m_InitSpeed;
	float m_Acceleration;

	float m_Deceleration;

	float m_LimitRotXAngle;
	float m_DownLimitX;
};
#include "CameraManager.h"
#include "Input.h"

using namespace DirectX;

cCameraManager::cCameraManager(DirectX::XMFLOAT3 playerPos)
{
	//まずは
	m_LuaAct = new cLuaActor("GameObject/CameraParam.lua");
	m_LuaAct->ScriptCommit();

	DirectX::XMFLOAT3 pPos = playerPos;

	for (int i = 0; i < m_DelayNum; i++) {
		m_PlayerPosArry.push_back(pPos);	// TODO ここにカメラ座標を計算して格納する
	}

	CameraParamUpdate();
	m_NowRotAngY = 0.0f;
	m_NowRotAngX = 0.0f;
	m_NowCameraSpeedY = 0.0f;
	m_NowCameraSpeedX = 0.0f;
	m_CameraRotPosY = 0.0f;
	m_CameraRotPosX = 0.0f;
}

cCameraManager::~cCameraManager()
{
	delete m_LuaAct;
}

DirectX::XMFLOAT3 cCameraManager::PopCameraPos(cCamera* pCamera)
{
	XMFLOAT3 ret = m_PlayerPosArry.front();
	ret.y += 1.0f;
	pCamera->SetLookPoint(ret);
	if (m_PlayerPosArry.size() == m_DelayNum)
		m_PlayerPosArry.pop_front();

	//カメラのパラメータをを元にしてプレイヤー座標からカメラの位置を割り出す
	ret = CoordinateCalculation(ret);
	pCamera->SetPosition(ret);
	return ret;
}

void cCameraManager::Update()
{
	//右スティックの情報を取得してカメラの回転を行う
	XMFLOAT2 _inpVec = GetGamePad->RightStick();
	CameraParamUpdate();
	XMFLOAT2 signedFlag = { 1.0f,1.0f };
	if (_inpVec.x < 0.0f)signedFlag.x = -1.0f;
	if (_inpVec.y < 0.0f)signedFlag.y = -1.0f;

	//回転速度をカメラに与えていく
	if (_inpVec.x != 0.0f) {
		m_NowCameraSpeedY += m_InitSpeed * signedFlag.x;
		m_NowCameraSpeedY *= m_Acceleration;
	}
	else {
		m_NowCameraSpeedY *= m_Deceleration;
	}

	if (_inpVec.y != 0.0f) {
		m_NowCameraSpeedX += m_InitSpeed * signedFlag.y;
		m_NowCameraSpeedX *= m_Acceleration;
	}
	else {
		m_NowCameraSpeedX *= m_Deceleration;
	}


	SpeedAdjustment();	//速度調整
}

void cCameraManager::SetPlayerPos(DirectX::XMFLOAT3 pos)
{
	m_PlayerPosArry.push_back(pos);
}

/// <summary>
/// カメラのパラメータをLuaから取得し、更新する
/// </summary>
void cCameraManager::CameraParamUpdate()
{
	// オフセット値を取得
	m_LuaAct->CallFunc("Offset", 3);
	m_CameraOffset = { m_LuaAct->m_Ret.GetNumber(0) ,m_LuaAct->m_Ret.GetNumber(1) ,m_LuaAct->m_Ret.GetNumber(2) };

	m_LuaAct->CallFunc("CameraDelay", 1);
	m_DelayNum = (int)m_LuaAct->m_Ret.GetNumber(0);

	m_LuaAct->CallFunc("MaxRotetAngle", 2);
	m_MaxRotAngleY = m_LuaAct->m_Ret.GetNumber(0);
	m_MaxRotAngleX = m_LuaAct->m_Ret.GetNumber(1);

	m_LuaAct->CallFunc("CameraSpeed", 2);
	m_InitSpeed = m_LuaAct->m_Ret.GetNumber(0);
	m_Acceleration = m_LuaAct->m_Ret.GetNumber(1);

	m_LuaAct->CallFunc("CameraDeceleration", 1);
	m_Deceleration = m_LuaAct->m_Ret.GetNumber(0);

	m_LuaAct->CallFunc("RotAngleLimit", 2);
	m_LimitRotXAngle = m_LuaAct->m_Ret.GetNumber(0);
	m_DownLimitX = m_LuaAct->m_Ret.GetNumber(1);
}

//範囲外に出た速度を元に戻す
void cCameraManager::SpeedAdjustment()
{
	//まず上限方向に出ていた場合
	if (m_NowCameraSpeedY > m_MaxRotAngleY) m_NowCameraSpeedY = m_MaxRotAngleY;
	if (m_NowCameraSpeedX > m_MaxRotAngleX) m_NowCameraSpeedX = m_MaxRotAngleX;

	if (m_NowCameraSpeedY < -m_MaxRotAngleY) m_NowCameraSpeedY = -m_MaxRotAngleY;
	if (m_NowCameraSpeedX < -m_MaxRotAngleX) m_NowCameraSpeedX = -m_MaxRotAngleX;

	//次に下限に出ていた場合
	if (fabs(m_NowCameraSpeedY) < 0.01f) m_NowCameraSpeedY = 0.0f;
	if (fabs(m_NowCameraSpeedX) < 0.01f) m_NowCameraSpeedX = 0.0f;
}

DirectX::XMFLOAT3 cCameraManager::CoordinateCalculation(DirectX::XMFLOAT3 playerPos)
{
	//オフセット値を回転情報を元に回転させたものをプレイヤー座標に足しこむ
	XMVECTOR cameraPos = XMLoadFloat3(&m_CameraOffset);
	m_CameraRotPosY += XMConvertToRadians(m_NowCameraSpeedY);
	m_CameraRotPosX += XMConvertToRadians(m_NowCameraSpeedX);

	if(m_CameraRotPosX > XMConvertToRadians(m_LimitRotXAngle))m_CameraRotPosX = XMConvertToRadians(m_LimitRotXAngle);
	if (m_CameraRotPosX <= XMConvertToRadians(m_DownLimitX)) m_CameraRotPosX = XMConvertToRadians(m_DownLimitX);

	XMMATRIX rot = XMMatrixRotationX(m_CameraRotPosX) * XMMatrixRotationY(m_CameraRotPosY);
	cameraPos = XMVector3TransformCoord(cameraPos, rot);

	XMFLOAT3 _Camera = { 0.0f,0.0f,0.0f };
	XMStoreFloat3(&_Camera, cameraPos);

	return XMFLOAT3(_Camera.x + playerPos.x, _Camera.y + playerPos.y, _Camera.z + playerPos.z);
}
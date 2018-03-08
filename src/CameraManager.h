#pragma once
#include "WinMain.h"
#include "LuaActor.h"
#include "Camera.h"
#include <deque>

/// <summary>
/// �v���C���[��������󂯎��J�������̏������s���N���X
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
	DirectX::XMFLOAT3 OffsetLinear();

	std::deque<DirectX::XMFLOAT3> m_PlayerPosArry;	//�v���C���[�̍��W�𗭂߂Ă���
	DirectX::XMFLOAT3 m_CameraOffset;
	DirectX::XMFLOAT3 m_CameraShortOffset;
	cLuaActor* m_LuaAct;
	int m_DelayNum;		//�J�����̒x����

	//��]�Ɋւ���p�����[�^
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

	float m_DistanceTime;		//����������ߋ����Ɉڂ��Ă���䗦(1.0�Ȃ牓����)
	bool m_LongDistanceFlag;	//�������t���O
};
/*=================================================
//								FILE : CameraManager.cpp
//		�t�@�C������ :
//		�J�����̐�����s���N���X
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "CameraManager.h"
#include "Input.h"

using namespace DirectX;

/// <summary>
/// �J�������̏��������s��
/// </summary>
/// <param name="playerPos"></param>
cCameraManager::cCameraManager(DirectX::XMFLOAT3 playerPos)
{
	//�܂���
	m_LuaAct = new cLuaActor("GameObject/CameraParam.lua");
	m_LuaAct->ScriptCommit();

	DirectX::XMFLOAT3 pPos = playerPos;

	for (int i = 0; i < m_DelayNum; i++) {
		m_PlayerPosArry.push_back(pPos);	// TODO �����ɃJ�������W���v�Z���Ċi�[����
	}

	CameraParamUpdate();
	m_NowRotAngY = 0.0f;
	m_NowRotAngX = 0.0f;
	m_NowCameraSpeedY = 0.0f;
	m_NowCameraSpeedX = 0.0f;
	m_CameraRotPosY = 0.0f;
	m_CameraRotPosX = 0.0f;

	m_LongDistanceFlag = true;
	m_DistanceTime = 1.0f;
}

/// <summary>
/// �������
/// </summary>
cCameraManager::~cCameraManager()
{
	delete m_LuaAct;
}

/// <summary>
/// �Â��f�[�^����|�b�v���邱�ƂŒx���J�����𐧌䂷��
/// </summary>
/// <param name="pCamera">�f�[�^���i�[����A�h���X</param>
/// <returns></returns>
DirectX::XMFLOAT3 cCameraManager::PopCameraPos(cCamera* pCamera)
{
	XMFLOAT3 ret = m_PlayerPosArry.front();
	float t = m_DistanceTime;
	t = t * t * (3 - 2 * t);
	ret.y += 1.0f + ((1.0f - t) * 0.7f);	//�ߋ����̎��͍������グ��
	pCamera->SetLookPoint(ret);
	if (m_PlayerPosArry.size() == m_DelayNum)
		m_PlayerPosArry.pop_front();

	//�J�����̃p�����[�^�������ɂ��ăv���C���[���W����J�����̈ʒu������o��
	ret = CoordinateCalculation(ret);
	pCamera->SetPosition(ret);
	return ret;
}

/// <summary>
/// �J�����A�b�v�f�[�g
/// </summary>
void cCameraManager::Update()
{
	//�E�X�e�B�b�N�̏����擾���ăJ�����̉�]���s��
	XMFLOAT2 _inpVec = GetGamePad->RightStick();
	CameraParamUpdate();
	XMFLOAT2 signedFlag = { 1.0f,1.0f };
	if (_inpVec.x < 0.0f)signedFlag.x = -1.0f;
	if (_inpVec.y < 0.0f)signedFlag.y = -1.0f;

	//��]���x���J�����ɗ^���Ă���
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


	SpeedAdjustment();	//���x����

	if (GetGamePad->Trigger(XINPUT_GAMEPAD_Y))
		m_LongDistanceFlag = !m_LongDistanceFlag;
}

/// <summary>
/// �v���C���[�̈ʒu��ݒ肷��
/// </summary>
/// <param name="pos"></param>
void cCameraManager::SetPlayerPos(DirectX::XMFLOAT3 pos)
{
	m_PlayerPosArry.push_back(pos);
}

/// <summary>
/// �J�����̃p�����[�^��Lua����擾���A�X�V����
/// </summary>
void cCameraManager::CameraParamUpdate()
{
	// �I�t�Z�b�g�l���擾
	m_LuaAct->CallFunc("Offset", 3);
	m_CameraOffset = { m_LuaAct->m_Ret.GetNumber(0) ,m_LuaAct->m_Ret.GetNumber(1) ,m_LuaAct->m_Ret.GetNumber(2) };

	m_LuaAct->CallFunc("ShortOffset", 3);
	m_CameraShortOffset = { m_LuaAct->m_Ret.GetNumber(0) ,m_LuaAct->m_Ret.GetNumber(1) ,m_LuaAct->m_Ret.GetNumber(2) };

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

//�͈͊O�ɏo�����x�����ɖ߂�
void cCameraManager::SpeedAdjustment()
{
	//�܂���������ɏo�Ă����ꍇ
	if (m_NowCameraSpeedY > m_MaxRotAngleY) m_NowCameraSpeedY = m_MaxRotAngleY;
	if (m_NowCameraSpeedX > m_MaxRotAngleX) m_NowCameraSpeedX = m_MaxRotAngleX;

	if (m_NowCameraSpeedY < -m_MaxRotAngleY) m_NowCameraSpeedY = -m_MaxRotAngleY;
	if (m_NowCameraSpeedX < -m_MaxRotAngleX) m_NowCameraSpeedX = -m_MaxRotAngleX;

	//���ɉ����ɏo�Ă����ꍇ
	if (fabs(m_NowCameraSpeedY) < 0.01f) m_NowCameraSpeedY = 0.0f;
	if (fabs(m_NowCameraSpeedX) < 0.01f) m_NowCameraSpeedX = 0.0f;
}

/// <summary>
/// �ʒu��␳����v�Z���s��
/// </summary>
/// <param name="playerPos"></param>
/// <returns></returns>
DirectX::XMFLOAT3 cCameraManager::CoordinateCalculation(DirectX::XMFLOAT3 playerPos)
{
	//�I�t�Z�b�g�l����]�������ɉ�]���������̂��v���C���[���W�ɑ�������
	XMVECTOR cameraPos = XMLoadFloat3(&OffsetLinear());
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

/// <summary>
/// �J�����̃I�t�Z�b�g�l��Ԃ��֐�
/// </summary>
/// <returns></returns>
XMFLOAT3 cCameraManager::OffsetLinear()
{
	//���݁A�J�������ߋ����Ȃ̂��������Ȃ̂��𔻒肵�Đ��`��Ԃ���

	//���݁A���������[�h�ł���
	if (m_LongDistanceFlag) {
		m_DistanceTime += 0.03f;
		if (m_DistanceTime >= 1.0f) m_DistanceTime = 1.0f;
	}
	else {
		m_DistanceTime -= 0.03f;
		if (m_DistanceTime < 0.0f) m_DistanceTime = 0.0f;
	}

	float t = m_DistanceTime;
	t = t * t * (3 - 2 * t);

	XMVECTOR v1 = XMLoadFloat3(&m_CameraShortOffset);
	XMVECTOR v2 = XMLoadFloat3(&m_CameraOffset);

	XMVECTOR dest = XMVectorLerp(v1, v2, t);
	XMFLOAT3 ret;

	XMStoreFloat3(&ret, dest);

	return ret;
}

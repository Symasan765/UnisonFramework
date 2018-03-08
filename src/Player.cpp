#include "Player.h"
#include "Input.h"
#include <algorithm>
#include <math.h>
using namespace DirectX;

/// <summary>
/// �v���C���[���̏�����
/// </summary>
cPlayer::cPlayer()
{
	m_ConstScal = 0.025f;
	LoadData("Player.x", 1);
	m_anmCnt = 0;
	SetScaling(m_ConstScal);
	SetTrans({ -17.3f,0.0f,6.4f });

	m_LuaScript = new cLuaActor("GameActor/PlayerMove.lua");
	m_LuaScript->ScriptCommit();

	m_MoveState = WAIT;

	m_MaxDashSpeed = 0.0f;
	m_MaxWalkSpeed = 0.0f;
	m_Accele = 0.0f;
	m_Decele = 0.0f;
	m_NowSpeed = 0.0f;
	m_NowVect = { 0.0f,1.0f };		//���������x�N�g��
	m_MaxRotatAngle = 0.0f;
}

/// <summary>
/// �������
/// </summary>
cPlayer::~cPlayer()
{
	delete m_LuaScript;
}

/// <summary>
/// �`��
/// </summary>
void cPlayer::Draw()
{
	int anmNo = 0;
	if (m_NowSpeed > 0.02) {
		anmNo = 1;
	}
	DrawMesh(anmNo, m_anmCnt);
}

/// <summary>
/// �X�V
/// </summary>
void cPlayer::Update()
{
	PlayerParamUpdate();
	Move();

	if (m_NowSpeed > 0.04) {
		m_anmCnt += (m_NowSpeed / m_MaxDashSpeed);	//�A�j���[�V�����̃J�E���g��i�߂Ă���
	}
	else
		m_anmCnt += 1.0f;	//�A�j���[�V�����̃J�E���g��i�߂Ă���

	PlayerVectChange();
}

/// <summary>
/// �J�����f�[�^���擾����
/// </summary>
/// <param name="data">�J�������</param>
void cPlayer::SetCameraData(ViewProj data)
{
	m_CameraData = data;
}

/// <summary>
/// �v���C���[�̃p�����[�^�f�[�^���X�V����
/// </summary>
void cPlayer::PlayerParamUpdate()
{
	m_LuaScript->CallFunc("RetMoveSpeed", 2);
	m_WalkAddSpeed = m_LuaScript->m_Ret.GetNumber(0);
	m_DashAddSpeed = m_LuaScript->m_Ret.GetNumber(1);

	m_LuaScript->CallFunc("Switching", 1);
	m_Switching = m_LuaScript->m_Ret.GetNumber(0);

	m_LuaScript->CallFunc("MaxSpeed", 4);
	m_MaxDashSpeed = m_LuaScript->m_Ret.GetNumber(0);
	m_MaxWalkSpeed = m_LuaScript->m_Ret.GetNumber(1);
	m_Accele = m_LuaScript->m_Ret.GetNumber(2);
	m_Decele = m_LuaScript->m_Ret.GetNumber(3);

	m_LuaScript->CallFunc("MaxRotatAngle", 1);
	m_MaxRotatAngle = m_LuaScript->m_Ret.GetNumber(0);
}

/// <summary>
/// �L�[�����擾���ăL�����N�^�[���ړ�������
/// </summary>
void cPlayer::Move()
{
	//���X�e�B�b�N�̏����擾����
	XMFLOAT2 moveVec = GetGamePad->LeftStick();

	XMVECTOR vec = XMLoadFloat2(&moveVec);
	vec = XMVector2Length(vec);
	float speed = 0.0f;
	//�ړ��x�N�g�����Ȃ���Ώ����̕K�v�Ȃ�
	if ((*vec.m128_f32) == 0.0f) {
		m_NowSpeed *= m_Decele;
	}
	else {
		InputAngleCorrection(MoveCorrection(moveVec, (*vec.m128_f32)));
		if ((*vec.m128_f32) > m_Switching) {
			m_MoveState = DASH;
			speed = m_DashAddSpeed;
		}	// end �_�b�V�����̑��x
		else {
			m_MoveState = WALK;
			speed = m_WalkAddSpeed;
		}	// end �������̑��x
		m_NowSpeed += speed;
	}	//end else �ړ��x�N�g���Ȃ�

	MovingSpeedClamp();
	moveVec = XMFLOAT2{ m_NowVect.x * m_NowSpeed,m_NowVect.y * m_NowSpeed };


	//��]�̕�Ԃ��s��
	RotationCalculation(moveVec);

	this->Translation({ moveVec.x,0.0f,moveVec.y });
	XMFLOAT3 NaNCheck = GetPosition();
	//��]�̏�����ǉ�����


	//�ړ��͈͂����肳����
	PosClamp();
}

/// <summary>
/// �L�����N�^�[�̈ړ��͈͂����肳����
/// </summary>
void cPlayer::PosClamp()
{
	XMFLOAT3 pos = GetPosition();
	//�X�e�[�W�͒ʘH�ƍL��̓�̎l�p�`�͈͂ō\������Ă���̂�
	//�ǂ���ɂ��邩�𔻒f���č��W�𒲐�����
	bool PlazaFlag = false;	//�L��ɂ��邩�H
	if (pos.z >= 1.0f || pos.x > -18.0f) PlazaFlag = true;

	if (PlazaFlag) {
		//�L��̏���
		m_LuaScript->CallFunc("RetPlazaClamp", 4);	//�ړ��͈͂��擾
		const XMFLOAT2 LeftDown = { m_LuaScript->m_Ret.GetNumber(0),m_LuaScript->m_Ret.GetNumber(1) };
		const XMFLOAT2 RightUp = { m_LuaScript->m_Ret.GetNumber(2),m_LuaScript->m_Ret.GetNumber(3) };

		//clamp����
		if (pos.x < LeftDown.x) pos.x = LeftDown.x;
		if (pos.x > RightUp.x) pos.x = RightUp.x;
		if (pos.z < LeftDown.y) pos.z = LeftDown.y;
		if (pos.z > RightUp.y) pos.z = RightUp.y;
	}
	else {
		//�ʘH�̏���
		//�L��̏���
		m_LuaScript->CallFunc("RetAisleClamp", 4);	//�ړ��͈͂��擾
		const XMFLOAT2 LeftDown = { m_LuaScript->m_Ret.GetNumber(0),m_LuaScript->m_Ret.GetNumber(1) };
		const XMFLOAT2 RightUp = { m_LuaScript->m_Ret.GetNumber(2),m_LuaScript->m_Ret.GetNumber(3) };

		//clamp����
		if (pos.x < LeftDown.x) pos.x = LeftDown.x;
		if (pos.x > RightUp.x) pos.x = RightUp.x;
		if (pos.z < LeftDown.y) pos.z = LeftDown.y;
		if (pos.z > RightUp.y) pos.z = RightUp.y;
	}
	SetTrans(pos);
}

/// <summary>
/// �L�����N�^�[�̓��͌���
/// </summary>
/// <param name="inp"></param>
void cPlayer::InputAngleCorrection(DirectX::XMFLOAT2 inp)
{
	// �ړ��x�N�g���ƕ����x�N�g���̊p�x�������߂�
	//�p�x�����傫�����
	XMVECTOR Vec = XMLoadFloat2(&m_NowVect);
	XMVECTOR Inp = XMLoadFloat2(&inp);

	Vec = XMVector2Normalize(Vec);
	Inp = XMVector2Normalize(Inp);

	XMVECTOR dot = XMVector2Dot(Vec, Inp);
	XMVECTOR cross = XMVector3Cross(Vec, Inp);	//���͂��E�����Ȃ������x�N�g�����ς���
	float XVec = cross.m128_f32[2];
	float si = *(dot.m128_f32);
	// NaN�΍�Ŕ͈͂�-1 ~ 1��
	if (si > 1.0f) si = 1.0f;
	if (si < -1.0f) si = -1.0f;
	float ang = XMConvertToDegrees(acosf(si));			//���ς̒l����p�x�����߂�
	const float MaxAng = m_MaxRotatAngle;
	float RotAng = ang;	//�����ɉ�]�p������

	//�p�x���ő��]�p�������Ă���΍ő��]�p�ɕύX����
	if (ang > MaxAng)
		RotAng = MaxAng;

	//���͂��^���ɋ߂��ꍇ�͋}���񂳂���
	if (si < -0.9f) {
		RotAng = MaxAng * 30.0f;
		if (XVec == 0.0f) {
			XVec = 1.0f;
		}
	}

	//�����x�N�g������]������
	Vec = XMVector2TransformCoord(Vec, XMMatrixRotationZ(XMConvertToRadians(RotAng * XVec)));
	Vec = XMVector2Normalize(Vec);
	XMStoreFloat2(&m_NowVect, Vec);
}

/// <summary>
/// �L�����N�^�[���ړ�������ۂ̃��f���̉�]�𐧌䂷��
/// </summary>
/// <param name="ProgressDir"></param>
void cPlayer::RotationCalculation(XMFLOAT2 ProgressDir)
{
	//�i�s����
	XMVECTOR buf = { ProgressDir.x,0.0f,ProgressDir.y };
	//���݌����Ă������
	XMVECTOR vec = XMLoadFloat3(&(GetAxisZ()));
	XMVECTOR prog = buf * vec;

	//���K��
	prog = XMVector3Normalize(prog);
	vec = XMVector3Normalize(-vec);

	float deg = *XMVector3Dot(prog, vec).m128_f32;	//��̂Ȃ��p�x�𒲂ׂ�
	float maxRot = 0.0f;
	m_LuaScript->CallFunc("RetRotateData", 1);
	maxRot = m_LuaScript->m_Ret.GetNumber(0);	//�ő��]�p�x���擾

	if (deg > maxRot) deg = maxRot;	//�ő�l�␳

	Rotation(0.0f, deg, 0.0f);
}

/// <summary>
/// �v���C���[�̐i�s�������烂�f���̌������v�Z���A�ύX����֐�
/// </summary>
void cPlayer::PlayerVectChange()
{
	//�ړ������Ə�x�N�g�����������Ă���̂ŊO�ʂ�X�x�N�g�����쐬����
	XMVECTOR vecZ = { m_NowVect.x ,0.0f ,m_NowVect.y };
	vecZ = XMVector3Normalize(vecZ);
	XMVECTOR vecY = { 0.0f,1.0f,0.0f };	//������͏�ɐ^��
	XMVECTOR vecX = XMVector3Cross(vecY, vecZ);
	//vecX = XMVector3Normalize(vecX);

	XMFLOAT3 vX = {};
	XMFLOAT3 vY = {};
	XMFLOAT3 vZ = {};
	XMStoreFloat3(&vX, vecX);
	XMStoreFloat3(&vY, vecY);
	XMStoreFloat3(&vZ, vecZ);

	XMFLOAT4X4 mat;
	mat._11 = vX.x;
	mat._12 = vX.y;
	mat._13 = vX.z;
	mat._14 = 0.0f;

	mat._21 = vY.x;
	mat._22 = vY.y;
	mat._23 = vY.z;
	mat._24 = 0.0f;

	mat._31 = vZ.x;
	mat._32 = vZ.y;
	mat._33 = vZ.z;
	mat._34 = 0.0f;

	SetRotateMatrix(mat);

	//�Ō�Ɋg�k����
	Scaling(m_ConstScal);
}

/// <summary>
/// �v���C���[�̈ړ��͈͂𐧌�����
/// </summary>
/// <param name="ProgressDir"></param>
/// <param name="speed"></param>
/// <returns></returns>
DirectX::XMFLOAT2 cPlayer::MoveCorrection(DirectX::XMFLOAT2 ProgressDir, const float speed)
{
	//�J�������猩�đO���Ƃ��擾����
	XMVECTOR vecZ = { m_CameraData.vView._13 ,0.0f ,m_CameraData.vView._33 };
	vecZ = XMVector3Normalize(vecZ);
	XMVECTOR vecY = { 0.0f,1.0f,0.0f };	//������͏�ɐ^��
	XMVECTOR vecX = XMVector3Cross(vecY, vecZ);
	vecZ *= ProgressDir.y;
	vecX *= ProgressDir.x;

	XMVECTOR v = vecX + vecZ;
	v = XMVector3Normalize(v);
	v *= speed;
	XMFLOAT3 buf = {};
	XMStoreFloat3(&buf, v);

	return XMFLOAT2{ buf.x, buf.z };
}

/// <summary>
/// �ړ����x�͈̔͒������s��
/// </summary>
void cPlayer::MovingSpeedClamp()
{
	if (m_NowSpeed < 0.001f) {
		m_NowSpeed = 0.0f;
		return;
	}

	float maxSpeed = 0.0f;
	switch (m_MoveState)
	{
	case cPlayer::WALK:
		maxSpeed = m_MaxWalkSpeed;
		break;
	case cPlayer::DASH:
		maxSpeed = m_MaxDashSpeed;
		break;
	}

	if (m_NowSpeed > maxSpeed) {
		m_NowSpeed = maxSpeed;
		return;
	}
}
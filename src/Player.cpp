#include "Player.h"
#include "Input.h"
#include <algorithm>
using namespace DirectX;

cPlayer::cPlayer()
{
	LoadData("Link.x", 1);
	m_anmCnt = 0;
	SetTrans({ -17.3f,0.0f,6.4f });
	SetScaling(0.1f);

	m_LuaScript = new cLuaActor("GameActor/PlayerMove.lua");
	m_LuaScript->ScriptCommit();
}

cPlayer::~cPlayer()
{
	delete m_LuaScript;
}

void cPlayer::Draw()
{
	
	DrawMesh(0, m_anmCnt);
}

void cPlayer::Update()
{
	Move();
	m_anmCnt++;	//�A�j���[�V�����̃J�E���g��i�߂Ă���
}

void cPlayer::SetCameraData(ViewProj data)
{
	m_CameraData = data;
}

/// <summary>
/// �L�[�����擾���ăL�����N�^�[���ړ�������
/// </summary>
void cPlayer::Move()
{
	//�ړ��Ɋւ���p�����[�^���擾
	m_LuaScript->CallFunc("RetMoveSpeed", 1);
	const float speed = m_LuaScript->m_Ret.GetNumber(0);

	//���X�e�B�b�N�̏����擾����
	XMFLOAT2 moveVec = GetGamePad->LeftStick();
	moveVec = XMFLOAT2{ moveVec.x * speed,moveVec.y * speed };
	

	//��]�̕�Ԃ��s��
	RotationCalculation(moveVec);

	this->Translation({ MoveCorrection(moveVec) });
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

DirectX::XMFLOAT3 cPlayer::MoveCorrection(DirectX::XMFLOAT2 ProgressDir)
{
	//�J�������猩�đO���ƉE�������擾����
	XMVECTOR vecX = { m_CameraData.vView._11 ,0.0f ,m_CameraData.vView._31 };
	XMVECTOR vecZ = { m_CameraData.vView._13 ,0.0f ,m_CameraData.vView._33 };

	vecX = XMVector3Normalize(vecX);
	vecZ = XMVector3Normalize(vecZ);

	vecX *= ProgressDir.x;
	vecZ *= ProgressDir.y;

	vecX += vecZ;

	XMFLOAT3 ret{};
	XMStoreFloat3(&ret, vecX);
	return ret;
}

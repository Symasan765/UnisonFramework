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
	m_anmCnt++;	//アニメーションのカウントを進めておく
}

void cPlayer::SetCameraData(ViewProj data)
{
	m_CameraData = data;
}

/// <summary>
/// キー情報を取得してキャラクターを移動させる
/// </summary>
void cPlayer::Move()
{
	//移動に関するパラメータを取得
	m_LuaScript->CallFunc("RetMoveSpeed", 1);
	const float speed = m_LuaScript->m_Ret.GetNumber(0);

	//左スティックの情報を取得する
	XMFLOAT2 moveVec = GetGamePad->LeftStick();
	moveVec = XMFLOAT2{ moveVec.x * speed,moveVec.y * speed };
	

	//回転の補間を行う
	RotationCalculation(moveVec);

	this->Translation({ MoveCorrection(moveVec) });
	//回転の処理を追加する


	//移動範囲を限定させる
	PosClamp();
}

/// <summary>
/// キャラクターの移動範囲を限定させる
/// </summary>
void cPlayer::PosClamp()
{
	XMFLOAT3 pos = GetPosition();
	//ステージは通路と広場の二つの四角形範囲で構成されているので
	//どちらにいるかを判断して座標を調整する
	bool PlazaFlag = false;	//広場にいるか？
	if (pos.z >= 1.0f || pos.x > -18.0f) PlazaFlag = true;

	if (PlazaFlag) {
		//広場の処理
		m_LuaScript->CallFunc("RetPlazaClamp", 4);	//移動範囲を取得
		const XMFLOAT2 LeftDown = { m_LuaScript->m_Ret.GetNumber(0),m_LuaScript->m_Ret.GetNumber(1) };
		const XMFLOAT2 RightUp = { m_LuaScript->m_Ret.GetNumber(2),m_LuaScript->m_Ret.GetNumber(3) };

		//clamp処理
		if (pos.x < LeftDown.x) pos.x = LeftDown.x;
		if (pos.x > RightUp.x) pos.x = RightUp.x;
		if (pos.z < LeftDown.y) pos.z = LeftDown.y;
		if (pos.z > RightUp.y) pos.z = RightUp.y;
	}
	else {
		//通路の処理
		//広場の処理
		m_LuaScript->CallFunc("RetAisleClamp", 4);	//移動範囲を取得
		const XMFLOAT2 LeftDown = { m_LuaScript->m_Ret.GetNumber(0),m_LuaScript->m_Ret.GetNumber(1) };
		const XMFLOAT2 RightUp = { m_LuaScript->m_Ret.GetNumber(2),m_LuaScript->m_Ret.GetNumber(3) };

		//clamp処理
		if (pos.x < LeftDown.x) pos.x = LeftDown.x;
		if (pos.x > RightUp.x) pos.x = RightUp.x;
		if (pos.z < LeftDown.y) pos.z = LeftDown.y;
		if (pos.z > RightUp.y) pos.z = RightUp.y;
	}
	SetTrans(pos);
}

/// <summary>
/// キャラクターが移動をする際のモデルの回転を制御する
/// </summary>
/// <param name="ProgressDir"></param>
void cPlayer::RotationCalculation(XMFLOAT2 ProgressDir)
{
	//進行方向
	XMVECTOR buf = { ProgressDir.x,0.0f,ProgressDir.y };
	//現在向いている方向
	XMVECTOR vec = XMLoadFloat3(&(GetAxisZ()));
	XMVECTOR prog = buf * vec;

	//正規化
	prog = XMVector3Normalize(prog);
	vec = XMVector3Normalize(-vec);

	float deg = *XMVector3Dot(prog, vec).m128_f32;	//二つのなす角度を調べる
	float maxRot = 0.0f;
	m_LuaScript->CallFunc("RetRotateData", 1);
	maxRot = m_LuaScript->m_Ret.GetNumber(0);	//最大回転角度を取得

	if (deg > maxRot) deg = maxRot;	//最大値補正

	Rotation(0.0f, deg, 0.0f);
}

DirectX::XMFLOAT3 cPlayer::MoveCorrection(DirectX::XMFLOAT2 ProgressDir)
{
	//カメラから見て前方と右方向を取得する
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

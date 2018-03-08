#include "Player.h"
#include "Input.h"
#include <algorithm>
#include <math.h>
using namespace DirectX;

/// <summary>
/// プレイヤー情報の初期化
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
	m_NowVect = { 0.0f,1.0f };		//初期方向ベクトル
	m_MaxRotatAngle = 0.0f;
}

/// <summary>
/// 解放処理
/// </summary>
cPlayer::~cPlayer()
{
	delete m_LuaScript;
}

/// <summary>
/// 描画
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
/// 更新
/// </summary>
void cPlayer::Update()
{
	PlayerParamUpdate();
	Move();

	if (m_NowSpeed > 0.04) {
		m_anmCnt += (m_NowSpeed / m_MaxDashSpeed);	//アニメーションのカウントを進めておく
	}
	else
		m_anmCnt += 1.0f;	//アニメーションのカウントを進めておく

	PlayerVectChange();
}

/// <summary>
/// カメラデータを取得する
/// </summary>
/// <param name="data">カメラ情報</param>
void cPlayer::SetCameraData(ViewProj data)
{
	m_CameraData = data;
}

/// <summary>
/// プレイヤーのパラメータデータを更新する
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
/// キー情報を取得してキャラクターを移動させる
/// </summary>
void cPlayer::Move()
{
	//左スティックの情報を取得する
	XMFLOAT2 moveVec = GetGamePad->LeftStick();

	XMVECTOR vec = XMLoadFloat2(&moveVec);
	vec = XMVector2Length(vec);
	float speed = 0.0f;
	//移動ベクトルがなければ処理の必要なし
	if ((*vec.m128_f32) == 0.0f) {
		m_NowSpeed *= m_Decele;
	}
	else {
		InputAngleCorrection(MoveCorrection(moveVec, (*vec.m128_f32)));
		if ((*vec.m128_f32) > m_Switching) {
			m_MoveState = DASH;
			speed = m_DashAddSpeed;
		}	// end ダッシュ時の速度
		else {
			m_MoveState = WALK;
			speed = m_WalkAddSpeed;
		}	// end 歩き時の速度
		m_NowSpeed += speed;
	}	//end else 移動ベクトルなし

	MovingSpeedClamp();
	moveVec = XMFLOAT2{ m_NowVect.x * m_NowSpeed,m_NowVect.y * m_NowSpeed };


	//回転の補間を行う
	RotationCalculation(moveVec);

	this->Translation({ moveVec.x,0.0f,moveVec.y });
	XMFLOAT3 NaNCheck = GetPosition();
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
/// キャラクターの入力件数
/// </summary>
/// <param name="inp"></param>
void cPlayer::InputAngleCorrection(DirectX::XMFLOAT2 inp)
{
	// 移動ベクトルと方向ベクトルの角度差を求めて
	//角度差が大きければ
	XMVECTOR Vec = XMLoadFloat2(&m_NowVect);
	XMVECTOR Inp = XMLoadFloat2(&inp);

	Vec = XMVector2Normalize(Vec);
	Inp = XMVector2Normalize(Inp);

	XMVECTOR dot = XMVector2Dot(Vec, Inp);
	XMVECTOR cross = XMVector3Cross(Vec, Inp);	//入力が右方向なら上方向ベクトルが変える
	float XVec = cross.m128_f32[2];
	float si = *(dot.m128_f32);
	// NaN対策で範囲を-1 ~ 1に
	if (si > 1.0f) si = 1.0f;
	if (si < -1.0f) si = -1.0f;
	float ang = XMConvertToDegrees(acosf(si));			//内積の値から角度を求めた
	const float MaxAng = m_MaxRotatAngle;
	float RotAng = ang;	//ここに回転角を入れる

	//角度が最大回転角を上回っていれば最大回転角に変更する
	if (ang > MaxAng)
		RotAng = MaxAng;

	//入力が真後ろに近い場合は急旋回させる
	if (si < -0.9f) {
		RotAng = MaxAng * 30.0f;
		if (XVec == 0.0f) {
			XVec = 1.0f;
		}
	}

	//方向ベクトルを回転させる
	Vec = XMVector2TransformCoord(Vec, XMMatrixRotationZ(XMConvertToRadians(RotAng * XVec)));
	Vec = XMVector2Normalize(Vec);
	XMStoreFloat2(&m_NowVect, Vec);
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

/// <summary>
/// プレイヤーの進行方向からモデルの向きを計算し、変更する関数
/// </summary>
void cPlayer::PlayerVectChange()
{
	//移動方向と上ベクトルが判明しているので外戚でXベクトルを作成する
	XMVECTOR vecZ = { m_NowVect.x ,0.0f ,m_NowVect.y };
	vecZ = XMVector3Normalize(vecZ);
	XMVECTOR vecY = { 0.0f,1.0f,0.0f };	//上方向は常に真上
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

	//最後に拡縮調整
	Scaling(m_ConstScal);
}

/// <summary>
/// プレイヤーの移動範囲を制限する
/// </summary>
/// <param name="ProgressDir"></param>
/// <param name="speed"></param>
/// <returns></returns>
DirectX::XMFLOAT2 cPlayer::MoveCorrection(DirectX::XMFLOAT2 ProgressDir, const float speed)
{
	//カメラから見て前方とを取得する
	XMVECTOR vecZ = { m_CameraData.vView._13 ,0.0f ,m_CameraData.vView._33 };
	vecZ = XMVector3Normalize(vecZ);
	XMVECTOR vecY = { 0.0f,1.0f,0.0f };	//上方向は常に真上
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
/// 移動速度の範囲調整を行う
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
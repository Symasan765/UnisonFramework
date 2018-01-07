/*=================================================
//								FILE : MatrixCoord.cpp
//		ファイル説明 :
//		行列計算を行うクラスを定義する。
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "MatrixCoord.h"
#include "LuaConvert.h"
using namespace LuaConv;
using namespace DirectX;

cMatrix::cMatrix() {
	//行列を単位行列に変換
	XMMATRIX mat = XMMatrixIdentity();
	XMStoreFloat4x4(&mtxDate.mtxWorld, mat);

	m_ScaleRate = { 1.0f,1.0f,1.0f };	//初期拡縮率は100%
}

void cMatrix::Scaling(const float _rate) {
	XMVECTOR s = XMLoadFloat3(&m_ScaleRate);
	s *= _rate;		//拡縮率を上げる
	XMStoreFloat3(&m_ScaleRate, s);		//格納

	//行列を更新する
	XMMATRIX rot = XMMatrixScaling(_rate, _rate, _rate);		//回転行列を作成
	XMMATRIX mat = XMLoadFloat4x4(&mtxDate.mtxWorld);
	rot *= mat;
	XMStoreFloat4x4(&mtxDate.mtxWorld, rot);
}

void cMatrix::Scaling(const float _rateX, const float _rateY, const float _rateZ)
{
	//行列を更新する
	XMMATRIX rot = XMMatrixScaling(_rateX, _rateY, _rateZ);		//回転行列を作成
	XMMATRIX mat = XMLoadFloat4x4(&mtxDate.mtxWorld);
	rot *= mat;
	XMStoreFloat4x4(&mtxDate.mtxWorld, rot);
}

void cMatrix::Translation(const DirectX::XMFLOAT3 _moveVec) {
	XMMATRIX mat = XMLoadFloat4x4(&mtxDate.mtxWorld);
	XMMATRIX trans = XMMatrixIdentity();
	trans = XMMatrixTranslation(_moveVec.x, _moveVec.y, _moveVec.z);
	mat *= trans;
	XMStoreFloat4x4(&mtxDate.mtxWorld, mat);
}

void cMatrix::Rotation(const float _rotX, const float _rotY, const float _rotZ) {
	//まず現在の移動量を保持しておき、移動量を一時的に0にしておくことでローカル回転を行う。
	XMFLOAT3 pos = { mtxDate.mtxWorld._41,mtxDate.mtxWorld._42,mtxDate.mtxWorld._43 };
	SetTrans({ 0.0f, 0.0f, 0.0f });
	//回転処理スタート
	XMMATRIX mat = XMLoadFloat4x4(&mtxDate.mtxWorld);
	XMMATRIX rot = XMMatrixIdentity();
	//ローカルの回転軸を取得する
	XMVECTOR axis;

	//各回転を行う
	if (_rotX != 0.0f) {
		axis = XMLoadFloat3(&GetAxisX());
		axis = XMVector3Normalize(axis);
		rot *= XMMatrixRotationAxis(axis, XMConvertToRadians(_rotX));
	}
	if (_rotY != 0.0f) {
		axis = XMLoadFloat3(&GetAxisY());
		axis = XMVector3Normalize(axis);
		rot *= XMMatrixRotationAxis(axis, XMConvertToRadians(_rotY));
	}
	if (_rotZ != 0.0f) {
		axis = XMLoadFloat3(&GetAxisZ());
		axis = XMVector3Normalize(axis);
		rot *= XMMatrixRotationAxis(axis, XMConvertToRadians(_rotZ));
	}
	mat *= rot;
	XMStoreFloat4x4(&mtxDate.mtxWorld, mat);

	//最後に元の座標位置に移動させる
	SetTrans(pos);
}

void cMatrix::SetScaling(const float _deg) {
	auto p = this;
	//拡縮率を元に戻す
	mtxDate.mtxWorld._11 = mtxDate.mtxWorld._11 / m_ScaleRate.x;
	mtxDate.mtxWorld._22 = mtxDate.mtxWorld._22 / m_ScaleRate.x;
	mtxDate.mtxWorld._33 = mtxDate.mtxWorld._33 / m_ScaleRate.x;

	//縮小率を初期化
	m_ScaleRate = { _deg,_deg,_deg };

	//新しい拡縮率に変更
	mtxDate.mtxWorld._11 = mtxDate.mtxWorld._11 * m_ScaleRate.x;
	mtxDate.mtxWorld._22 = mtxDate.mtxWorld._22 * m_ScaleRate.x;
	mtxDate.mtxWorld._33 = mtxDate.mtxWorld._33 * m_ScaleRate.x;
}

void cMatrix::SetRotate(const float _rotX, const float _rotY, const float _rotZ) {
	//回転処理スタート
	XMMATRIX rot = XMMatrixIdentity();
	//ローカルの回転軸を取得する
	XMVECTOR axis;


	//各回転を行う
	{
		XMFLOAT3 Vect = { 1.0f,0.0f,0.0f };
		axis = XMLoadFloat3(&Vect);
		rot *= XMMatrixRotationAxis(axis, XMConvertToRadians(_rotX));
	}
	{
		XMFLOAT3 Vect = { 0.0f,1.0f,0.0f };
		axis = XMLoadFloat3(&Vect);
		rot *= XMMatrixRotationAxis(axis, XMConvertToRadians(_rotY));
	}
	{
		XMFLOAT3 Vect = { 0.0f,0.0f,1.0f };
		axis = XMLoadFloat3(&Vect);
		rot *= XMMatrixRotationAxis(axis, XMConvertToRadians(_rotZ));
	}

	XMFLOAT3 pos(mtxDate.mtxWorld._41, mtxDate.mtxWorld._42, mtxDate.mtxWorld._43);
	XMStoreFloat4x4(&mtxDate.mtxWorld, rot);
	mtxDate.mtxWorld._41 = pos.x;
	mtxDate.mtxWorld._42 = pos.y;
	mtxDate.mtxWorld._43 = pos.z;
}

void cMatrix::SetTrans(const DirectX::XMFLOAT3 pos) {
	mtxDate.mtxWorld._41 = pos.x;
	mtxDate.mtxWorld._42 = pos.y;
	mtxDate.mtxWorld._43 = pos.z;
}

void cMatrix::SetMatrix(const DirectX::XMFLOAT4X4 _m)
{
	mtxDate.mtxWorld = _m;
}

void cMatrix::VectMove(const float _moveVec) {
	//まず現在のZ軸方向ベクトルを取得する
	XMFLOAT3 vec = GetAxisZ();

	mtxDate.mtxWorld._41 += vec.x * _moveVec;
	mtxDate.mtxWorld._42 += vec.y * _moveVec;
	mtxDate.mtxWorld._43 += vec.z * _moveVec;
}

void cMatrix::WorldRotation(const float _rotX, const float _rotY, const float _rotZ)
{
	//回転処理スタート
	XMMATRIX mat = XMLoadFloat4x4(&mtxDate.mtxWorld);
	XMMATRIX rot = XMMatrixIdentity();
	//ローカルの回転軸を取得する
	XMVECTOR axis;

	//各回転を行う
	if (_rotX != 0.0f) {
		axis = XMLoadFloat3(&GetAxisX());
		axis = XMVector3Normalize(axis);
		rot *= XMMatrixRotationAxis(axis, XMConvertToRadians(_rotX));
	}
	if (_rotY != 0.0f) {
		axis = XMLoadFloat3(&GetAxisY());
		axis = XMVector3Normalize(axis);
		rot *= XMMatrixRotationAxis(axis, XMConvertToRadians(_rotY));
	}
	if (_rotZ != 0.0f) {
		axis = XMLoadFloat3(&GetAxisZ());
		axis = XMVector3Normalize(axis);
		rot *= XMMatrixRotationAxis(axis, XMConvertToRadians(_rotZ));
	}
	mat *= rot;
	XMStoreFloat4x4(&mtxDate.mtxWorld, mat);
}

DirectX::XMFLOAT3 cMatrix::GetPosition()const {
	XMFLOAT3 _pos = { mtxDate.mtxWorld._41,mtxDate.mtxWorld._42 ,mtxDate.mtxWorld._43 };
	return _pos;
}

DirectX::XMFLOAT4X4 cMatrix::GetWorldMatrix() const {
	return mtxDate.mtxWorld;
}

DirectX::XMFLOAT3 cMatrix::GetAxisX()const {
	//軸ベクトルを正規化して返却する
	XMFLOAT3 axis = { mtxDate.mtxWorld._11,mtxDate.mtxWorld._12,mtxDate.mtxWorld._13 };
	XMVECTOR axisVec = XMLoadFloat3(&axis);
	axisVec = XMVector3Normalize(axisVec);
	XMStoreFloat3(&axis, axisVec);
	return axis;
}

DirectX::XMFLOAT3 cMatrix::GetAxisY()const {
	//軸ベクトルを正規化して返却する
	XMFLOAT3 axis = { mtxDate.mtxWorld._21,mtxDate.mtxWorld._22,mtxDate.mtxWorld._23 };
	XMVECTOR axisVec = XMLoadFloat3(&axis);
	axisVec = XMVector3Normalize(axisVec);
	XMStoreFloat3(&axis, axisVec);
	return axis;
}

DirectX::XMFLOAT3 cMatrix::GetAxisZ()const {
	//軸ベクトルを正規化して返却する
	XMFLOAT3 axis = { mtxDate.mtxWorld._31,mtxDate.mtxWorld._32,mtxDate.mtxWorld._33 };
	XMVECTOR axisVec = XMLoadFloat3(&axis);
	axisVec = XMVector3Normalize(axisVec);
	XMStoreFloat3(&axis, axisVec);
	return axis;
}

/// <summary>
/// Luaへグルー関数を追加して指定したLuaステート上からコントロールできるようにする
/// </summary>
/// <param name="L"></param>
/// <param name="LuaVarName"></param>
void cMatrix::AddFunctionToLua(lua_State * L, std::string LuaVarName)
{
	const luaL_Reg funcs[] = {
		{ "Scaling", cMatrix::ScalingGlue },
		{ "Translation", cMatrix::TranslationGlue },
		{ "Rotation", cMatrix::RotationGlue },
		{ "SetScaling", cMatrix::SetScalingGlue },
		{ "SetRotate", cMatrix::SetRotateGlue },
		{ "SetTrans", cMatrix::SetTransGlue },
		{ "VectMove", cMatrix::VectMoveGlue },
		{ "GetPosition", cMatrix::GetPositionGlue },
		{ NULL, NULL }
	};

	SetLuaGlobalObjct<cMatrix>(L, LuaVarName.c_str(), funcs, this);
}

/// <summary>
/// transform:Scaling(rate)
/// </summary>
/// <param name="L"></param>
/// <returns></returns>
int cMatrix::ScalingGlue(lua_State * L)
{
	//テーブルからアドレスを持ってくる
	auto p = toPointer<cMatrix>(L);

	//引数を取得していく
	float rate = (float)lua_tonumber(L, 2);

	//関数実行
	p->Scaling(rate);
	lua_settop(L, 0);	//関数呼び出し用スタックは戻り値では戻り値以外は削除して問題ない

	return 0;		//戻り値
}

/// <summary>
/// transform:Translation(x,y,z)
/// </summary>
/// <param name="L"></param>
/// <returns></returns>
int cMatrix::TranslationGlue(lua_State * L)
{
	//テーブルからアドレスを持ってくる
	auto p = toPointer<cMatrix>(L);

	//引数を取得していく
	float x = (float)lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);
	float z = (float)lua_tonumber(L, 4);

	//関数実行
	p->Translation(XMFLOAT3{ x,y,z });
	lua_settop(L, 0);	//関数呼び出し用スタックは戻り値では戻り値以外は削除して問題ない

	return 0;		//戻り値
}

/// <summary>
/// transform:Rotation(x,y,z)
/// </summary>
/// <param name="L"></param>
/// <returns></returns>
int cMatrix::RotationGlue(lua_State * L)
{
	//テーブルからアドレスを持ってくる
	auto p = toPointer<cMatrix>(L);

	//引数を取得していく
	float x = (float)lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);
	float z = (float)lua_tonumber(L, 4);

	//関数実行
	p->Rotation(x, y, z);
	lua_settop(L, 0);	//関数呼び出し用スタックは戻り値では戻り値以外は削除して問題ない

	return 0;		//戻り値
}

/// <summary>
/// transform:SetScaling(rate)
/// </summary>
/// <param name="L"></param>
/// <returns></returns>
int cMatrix::SetScalingGlue(lua_State * L)
{
	//テーブルからアドレスを持ってくる
	auto p = toPointer<cMatrix>(L);

	//引数を取得していく
	float rate = (float)lua_tonumber(L, 2);

	//関数実行
	p->SetScaling(rate);
	lua_settop(L, 0);	//関数呼び出し用スタックは戻り値では戻り値以外は削除して問題ない

	return 0;		//戻り値
}

/// <summary>
/// transform:SetRotate(x,y,z)
/// </summary>
/// <param name="L"></param>
/// <returns></returns>
int cMatrix::SetRotateGlue(lua_State * L)
{
	//テーブルからアドレスを持ってくる
	auto p = toPointer<cMatrix>(L);

	//引数を取得していく
	float x = (float)lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);
	float z = (float)lua_tonumber(L, 4);

	//関数実行
	p->SetRotate(x, y, z);
	lua_settop(L, 0);	//関数呼び出し用スタックは戻り値では戻り値以外は削除して問題ない

	return 0;		//戻り値
}

/// <summary>
/// transform:SetTrans(x,y,z)
/// </summary>
/// <param name="L"></param>
/// <returns></returns>
int cMatrix::SetTransGlue(lua_State * L)
{
	//テーブルからアドレスを持ってくる
	auto p = toPointer<cMatrix>(L);

	//引数を取得していく
	float x = (float)lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);
	float z = (float)lua_tonumber(L, 4);

	//関数実行
	p->SetTrans(XMFLOAT3{ x,y,z });
	lua_settop(L, 0);	//関数呼び出し用スタックは戻り値では戻り値以外は削除して問題ない

	return 0;		//戻り値
}

/// <summary>
/// transform:VectMove(scalar)
/// </summary>
/// <param name="L"></param>
/// <returns></returns>
int cMatrix::VectMoveGlue(lua_State * L)
{
	//テーブルからアドレスを持ってくる
	auto p = toPointer<cMatrix>(L);

	//引数を取得していく
	float scalar = (float)lua_tonumber(L, 2);

	//関数実行
	p->VectMove(scalar);
	lua_settop(L, 0);	//関数呼び出し用スタックは戻り値では戻り値以外は削除して問題ない

	return 0;		//戻り値
}

int cMatrix::GetPositionGlue(lua_State * L)
{
	//テーブルからアドレスを持ってくる
	auto p = toPointer<cMatrix>(L);

	//関数実行
	XMFLOAT3 pos = p->GetPosition();
	lua_settop(L, 0);	//関数呼び出し用スタックは戻り値では戻り値以外は削除して問題ない

	//戻り値を格納
	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);
	lua_pushnumber(L, pos.z);

	return 3;		//戻り値
}

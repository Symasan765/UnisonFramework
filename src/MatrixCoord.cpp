/*=================================================
//								FILE : MatrixCoord.cpp
//		�t�@�C������ :
//		�s��v�Z���s���N���X���`����B
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "MatrixCoord.h"
#include "LuaConvert.h"
using namespace LuaConv;
using namespace DirectX;

cMatrix::cMatrix() {
	//�s���P�ʍs��ɕϊ�
	XMMATRIX mat = XMMatrixIdentity();
	XMStoreFloat4x4(&mtxDate.mtxWorld, mat);

	m_ScaleRate = { 1.0f,1.0f,1.0f };	//�����g�k����100%
}

void cMatrix::Scaling(const float _rate) {
	XMVECTOR s = XMLoadFloat3(&m_ScaleRate);
	s *= _rate;		//�g�k�����グ��
	XMStoreFloat3(&m_ScaleRate, s);		//�i�[

	//�s����X�V����
	XMMATRIX rot = XMMatrixScaling(_rate, _rate, _rate);		//��]�s����쐬
	XMMATRIX mat = XMLoadFloat4x4(&mtxDate.mtxWorld);
	rot *= mat;
	XMStoreFloat4x4(&mtxDate.mtxWorld, rot);
}

void cMatrix::Scaling(const float _rateX, const float _rateY, const float _rateZ)
{
	//�s����X�V����
	XMMATRIX rot = XMMatrixScaling(_rateX, _rateY, _rateZ);		//��]�s����쐬
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
	//�܂����݂̈ړ��ʂ�ێ����Ă����A�ړ��ʂ��ꎞ�I��0�ɂ��Ă������ƂŃ��[�J����]���s���B
	XMFLOAT3 pos = { mtxDate.mtxWorld._41,mtxDate.mtxWorld._42,mtxDate.mtxWorld._43 };
	SetTrans({ 0.0f, 0.0f, 0.0f });
	//��]�����X�^�[�g
	XMMATRIX mat = XMLoadFloat4x4(&mtxDate.mtxWorld);
	XMMATRIX rot = XMMatrixIdentity();
	//���[�J���̉�]�����擾����
	XMVECTOR axis;

	//�e��]���s��
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

	//�Ō�Ɍ��̍��W�ʒu�Ɉړ�������
	SetTrans(pos);
}

void cMatrix::SetScaling(const float _deg) {
	auto p = this;
	//�g�k�������ɖ߂�
	mtxDate.mtxWorld._11 = mtxDate.mtxWorld._11 / m_ScaleRate.x;
	mtxDate.mtxWorld._22 = mtxDate.mtxWorld._22 / m_ScaleRate.x;
	mtxDate.mtxWorld._33 = mtxDate.mtxWorld._33 / m_ScaleRate.x;

	//�k������������
	m_ScaleRate = { _deg,_deg,_deg };

	//�V�����g�k���ɕύX
	mtxDate.mtxWorld._11 = mtxDate.mtxWorld._11 * m_ScaleRate.x;
	mtxDate.mtxWorld._22 = mtxDate.mtxWorld._22 * m_ScaleRate.x;
	mtxDate.mtxWorld._33 = mtxDate.mtxWorld._33 * m_ScaleRate.x;
}

void cMatrix::SetRotate(const float _rotX, const float _rotY, const float _rotZ) {
	//��]�����X�^�[�g
	XMMATRIX rot = XMMatrixIdentity();
	//���[�J���̉�]�����擾����
	XMVECTOR axis;


	//�e��]���s��
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
	//�܂����݂�Z�������x�N�g�����擾����
	XMFLOAT3 vec = GetAxisZ();

	mtxDate.mtxWorld._41 += vec.x * _moveVec;
	mtxDate.mtxWorld._42 += vec.y * _moveVec;
	mtxDate.mtxWorld._43 += vec.z * _moveVec;
}

void cMatrix::WorldRotation(const float _rotX, const float _rotY, const float _rotZ)
{
	//��]�����X�^�[�g
	XMMATRIX mat = XMLoadFloat4x4(&mtxDate.mtxWorld);
	XMMATRIX rot = XMMatrixIdentity();
	//���[�J���̉�]�����擾����
	XMVECTOR axis;

	//�e��]���s��
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
	//���x�N�g���𐳋K�����ĕԋp����
	XMFLOAT3 axis = { mtxDate.mtxWorld._11,mtxDate.mtxWorld._12,mtxDate.mtxWorld._13 };
	XMVECTOR axisVec = XMLoadFloat3(&axis);
	axisVec = XMVector3Normalize(axisVec);
	XMStoreFloat3(&axis, axisVec);
	return axis;
}

DirectX::XMFLOAT3 cMatrix::GetAxisY()const {
	//���x�N�g���𐳋K�����ĕԋp����
	XMFLOAT3 axis = { mtxDate.mtxWorld._21,mtxDate.mtxWorld._22,mtxDate.mtxWorld._23 };
	XMVECTOR axisVec = XMLoadFloat3(&axis);
	axisVec = XMVector3Normalize(axisVec);
	XMStoreFloat3(&axis, axisVec);
	return axis;
}

DirectX::XMFLOAT3 cMatrix::GetAxisZ()const {
	//���x�N�g���𐳋K�����ĕԋp����
	XMFLOAT3 axis = { mtxDate.mtxWorld._31,mtxDate.mtxWorld._32,mtxDate.mtxWorld._33 };
	XMVECTOR axisVec = XMLoadFloat3(&axis);
	axisVec = XMVector3Normalize(axisVec);
	XMStoreFloat3(&axis, axisVec);
	return axis;
}

/// <summary>
/// Lua�փO���[�֐���ǉ����Ďw�肵��Lua�X�e�[�g�ォ��R���g���[���ł���悤�ɂ���
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
	//�e�[�u������A�h���X�������Ă���
	auto p = toPointer<cMatrix>(L);

	//�������擾���Ă���
	float rate = (float)lua_tonumber(L, 2);

	//�֐����s
	p->Scaling(rate);
	lua_settop(L, 0);	//�֐��Ăяo���p�X�^�b�N�͖߂�l�ł͖߂�l�ȊO�͍폜���Ė��Ȃ�

	return 0;		//�߂�l
}

/// <summary>
/// transform:Translation(x,y,z)
/// </summary>
/// <param name="L"></param>
/// <returns></returns>
int cMatrix::TranslationGlue(lua_State * L)
{
	//�e�[�u������A�h���X�������Ă���
	auto p = toPointer<cMatrix>(L);

	//�������擾���Ă���
	float x = (float)lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);
	float z = (float)lua_tonumber(L, 4);

	//�֐����s
	p->Translation(XMFLOAT3{ x,y,z });
	lua_settop(L, 0);	//�֐��Ăяo���p�X�^�b�N�͖߂�l�ł͖߂�l�ȊO�͍폜���Ė��Ȃ�

	return 0;		//�߂�l
}

/// <summary>
/// transform:Rotation(x,y,z)
/// </summary>
/// <param name="L"></param>
/// <returns></returns>
int cMatrix::RotationGlue(lua_State * L)
{
	//�e�[�u������A�h���X�������Ă���
	auto p = toPointer<cMatrix>(L);

	//�������擾���Ă���
	float x = (float)lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);
	float z = (float)lua_tonumber(L, 4);

	//�֐����s
	p->Rotation(x, y, z);
	lua_settop(L, 0);	//�֐��Ăяo���p�X�^�b�N�͖߂�l�ł͖߂�l�ȊO�͍폜���Ė��Ȃ�

	return 0;		//�߂�l
}

/// <summary>
/// transform:SetScaling(rate)
/// </summary>
/// <param name="L"></param>
/// <returns></returns>
int cMatrix::SetScalingGlue(lua_State * L)
{
	//�e�[�u������A�h���X�������Ă���
	auto p = toPointer<cMatrix>(L);

	//�������擾���Ă���
	float rate = (float)lua_tonumber(L, 2);

	//�֐����s
	p->SetScaling(rate);
	lua_settop(L, 0);	//�֐��Ăяo���p�X�^�b�N�͖߂�l�ł͖߂�l�ȊO�͍폜���Ė��Ȃ�

	return 0;		//�߂�l
}

/// <summary>
/// transform:SetRotate(x,y,z)
/// </summary>
/// <param name="L"></param>
/// <returns></returns>
int cMatrix::SetRotateGlue(lua_State * L)
{
	//�e�[�u������A�h���X�������Ă���
	auto p = toPointer<cMatrix>(L);

	//�������擾���Ă���
	float x = (float)lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);
	float z = (float)lua_tonumber(L, 4);

	//�֐����s
	p->SetRotate(x, y, z);
	lua_settop(L, 0);	//�֐��Ăяo���p�X�^�b�N�͖߂�l�ł͖߂�l�ȊO�͍폜���Ė��Ȃ�

	return 0;		//�߂�l
}

/// <summary>
/// transform:SetTrans(x,y,z)
/// </summary>
/// <param name="L"></param>
/// <returns></returns>
int cMatrix::SetTransGlue(lua_State * L)
{
	//�e�[�u������A�h���X�������Ă���
	auto p = toPointer<cMatrix>(L);

	//�������擾���Ă���
	float x = (float)lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);
	float z = (float)lua_tonumber(L, 4);

	//�֐����s
	p->SetTrans(XMFLOAT3{ x,y,z });
	lua_settop(L, 0);	//�֐��Ăяo���p�X�^�b�N�͖߂�l�ł͖߂�l�ȊO�͍폜���Ė��Ȃ�

	return 0;		//�߂�l
}

/// <summary>
/// transform:VectMove(scalar)
/// </summary>
/// <param name="L"></param>
/// <returns></returns>
int cMatrix::VectMoveGlue(lua_State * L)
{
	//�e�[�u������A�h���X�������Ă���
	auto p = toPointer<cMatrix>(L);

	//�������擾���Ă���
	float scalar = (float)lua_tonumber(L, 2);

	//�֐����s
	p->VectMove(scalar);
	lua_settop(L, 0);	//�֐��Ăяo���p�X�^�b�N�͖߂�l�ł͖߂�l�ȊO�͍폜���Ė��Ȃ�

	return 0;		//�߂�l
}

int cMatrix::GetPositionGlue(lua_State * L)
{
	//�e�[�u������A�h���X�������Ă���
	auto p = toPointer<cMatrix>(L);

	//�֐����s
	XMFLOAT3 pos = p->GetPosition();
	lua_settop(L, 0);	//�֐��Ăяo���p�X�^�b�N�͖߂�l�ł͖߂�l�ȊO�͍폜���Ė��Ȃ�

	//�߂�l���i�[
	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);
	lua_pushnumber(L, pos.z);

	return 3;		//�߂�l
}

/*=================================================
//								FILE : Camera.cpp
//		�t�@�C������ :
//		�Q�[���V�[�����Ŏ�舵�����C���J��������������N���X
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "Camera.h"
#include "Input.h"			//�t���[�J�����̑���p�ɗ��p
#include "LuaConvert.h"

using namespace LuaConv;
using namespace DirectX;

/// <summary>
/// �J�����̃R���X�g���N�^
/// </summary>
cCamera::cCamera()
{
	//�J�����̏����l��ݒ肷��
	SetCameraAllData({ {0.0f,10.0f,-15.0f},{0.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},0.2f,200.0f });
	m_ShaderConst.data = { m_CameraData.vPos.x,m_CameraData.vPos.y,m_CameraData.vPos.z,1.0f };
}

cCamera::~cCamera()
{
}

/// <summary>
/// �ݒ肳��Ă���������Ƀr���[�A�v���W�F�N�V�������쐬����
/// </summary>
/// <returns></returns>
const ViewProj& cCamera::Projection(bool _FreeCameraFlag)
{
	XMMATRIX View = XMMatrixIdentity();
	XMMATRIX Proj = XMMatrixIdentity();
	XMFLOAT3 pos, look, up;
	float Near, Far;
	//�t���[�J�������ǂ����Őݒ��ς���
	if (!_FreeCameraFlag) {
		//�ʏ탌���_�����O
		pos = m_CameraData.vPos;
		look = m_CameraData.vLook;
		up = m_CameraData.vUp;
		Near = m_CameraData.vNear;
		Far = m_CameraData.vFar;
	}
	else {
		//�t���[�J���������_�����O
		pos = m_DebugCameraData.vPos;
		look = m_DebugCameraData.vLook;
		up = m_DebugCameraData.vUp;
		Near = m_DebugCameraData.vNear;
		Far = m_DebugCameraData.vFar;
	}

	//���W�ʒu,������x�N�g��,�����_�x�N�g�����擾
	XMVECTOR eyePosition = XMLoadFloat3(&pos);
	XMVECTOR upDirection = XMLoadFloat3(&up);
	XMVECTOR focusPosition = XMLoadFloat3(&look);

	View = DirectX::XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);
	Proj = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(45.0f),		// ����p
		(float)WINDOW_SIZE_X / (float)WINDOW_SIZE_Y,	// �A�X�y�N�g��
		Near,							// �O�����e�ʂ܂ł̋���
		Far);							// ������e�ʂ܂ł̋���


	//�t���[�J�����t���O�ɂ���ēn������ς���
	if (!_FreeCameraFlag) {
		//�ʏ탌���_�����O
		XMStoreFloat4x4(&m_CameraVP.vProj, Proj);
		XMStoreFloat4x4(&m_CameraVP.vView, View);
		XMMATRIX mat = View * Proj;		//�r���[,�v���W�F�N�V�����s��v�Z
		XMStoreFloat4x4(&m_CameraVP.vVP, mat);
		return m_CameraVP;
	}

	//��L�ɓ���Ȃ����� = �t���[�J����
	XMStoreFloat4x4(&m_DebugVP.vProj, Proj);
	XMStoreFloat4x4(&m_DebugVP.vView, View);
	XMMATRIX mat = View * Proj;		//�r���[,�v���W�F�N�V�����s��v�Z
	XMStoreFloat4x4(&m_DebugVP.vVP, mat);
	return m_DebugVP;
}

/// <summary>
/// �t���[�J�����̑�����L�q����
/// </summary>
void cCamera::FreeCameraOperation()
{
#if defined(DEBUG) || defined(_DEBUG)
	//�G���^�[�L�[�������ꂽ��ʏ�J�����̈ʒu�ɖ߂�
	if (GetKey->Trigger(DIK_RETURN)) {
		m_DebugCameraData = m_CameraData;
	}

	//�ړ��v�Z
	if (true) {
		//�ړ��x�N�g��
		DirectX::XMFLOAT3 _VecZ =
		{ m_DebugCameraData.vLook.x - m_DebugCameraData.vPos.x,
			m_DebugCameraData.vLook.y - m_DebugCameraData.vPos.y,
			m_DebugCameraData.vLook.z - m_DebugCameraData.vPos.z };
		DirectX::XMVECTOR Vec = DirectX::XMLoadFloat3(&_VecZ);
		Vec = DirectX::XMVector3Normalize(Vec); 		//�ړ��ʂ𐳋K��

		XMFLOAT3 _VecX = { 0,1.0f,0 };
		XMVECTOR VecX = XMLoadFloat3(&_VecX);			//�������ړ����������ɂ���90�x��]������
		XMMATRIX m = XMMatrixRotationAxis(Vec, XMConvertToRadians(-90.0f));
		VecX = XMVector3TransformCoord(VecX, m);
		VecX *= 0.1f;
		XMStoreFloat3(&_VecX, VecX);

		Vec *= 0.1f;							//�ړ���
		XMStoreFloat3(&_VecZ, Vec);

		//�ړ��̏���
		if (GetKey->Press(DIK_W)) {
			//�O�i
			m_DebugCameraData.vPos.x += _VecZ.x;
			m_DebugCameraData.vPos.y += _VecZ.y;
			m_DebugCameraData.vPos.z += _VecZ.z;

			m_DebugCameraData.vLook.x += _VecZ.x;
			m_DebugCameraData.vLook.y += _VecZ.y;
			m_DebugCameraData.vLook.z += _VecZ.z;
		}
		else if (GetKey->Press(DIK_S)) {
			//���
			m_DebugCameraData.vPos.x -= _VecZ.x;
			m_DebugCameraData.vPos.y -= _VecZ.y;
			m_DebugCameraData.vPos.z -= _VecZ.z;

			m_DebugCameraData.vLook.x -= _VecZ.x;
			m_DebugCameraData.vLook.y -= _VecZ.y;
			m_DebugCameraData.vLook.z -= _VecZ.z;
		}

		if (GetKey->Press(DIK_D)) {
			//�E�i
			m_DebugCameraData.vPos.x += _VecX.x;
			m_DebugCameraData.vPos.y += _VecX.y;
			m_DebugCameraData.vPos.z += _VecX.z;

			m_DebugCameraData.vLook.x += _VecX.x;
			m_DebugCameraData.vLook.y += _VecX.y;
			m_DebugCameraData.vLook.z += _VecX.z;
		}
		else if (GetKey->Press(DIK_A)) {
			//���i
			m_DebugCameraData.vPos.x -= _VecX.x;
			m_DebugCameraData.vPos.y -= _VecX.y;
			m_DebugCameraData.vPos.z -= _VecX.z;

			m_DebugCameraData.vLook.x -= _VecX.x;
			m_DebugCameraData.vLook.y -= _VecX.y;
			m_DebugCameraData.vLook.z -= _VecX.z;
		}
	}

	//��]�v�Z
	if (true) {
		//��]�͒����_��ύX���邱�Ƃōs��
		XMFLOAT3 LookVec =
		{ m_DebugCameraData.vLook.x - m_DebugCameraData.vPos.x,
			m_DebugCameraData.vLook.y - m_DebugCameraData.vPos.y,
			m_DebugCameraData.vLook.z - m_DebugCameraData.vPos.z };

		XMVECTOR Vec = XMLoadFloat3(&LookVec);		//���ꂪ��{�̉�]��
		XMVECTOR Pos = XMLoadFloat3(&m_DebugCameraData.vPos);
		XMFLOAT3 _SideVec = { 0,1.0f,0 };		//�������]�����ĉ����Ƃ���
		XMFLOAT3 _UpVec = { 0,0,1.0f };			//�������]�����ďc���Ƃ���
		XMVECTOR SideVec = XMLoadFloat3(&_SideVec);
		XMVECTOR UpVec = XMLoadFloat3(&_UpVec);

		//���������߂�
		XMMATRIX m = XMMatrixRotationAxis(Vec, XMConvertToRadians(-90.0f));
		SideVec = XMVector3TransformCoord(SideVec, m);

		m = XMMatrixRotationX(XMConvertToRadians(-90.0f));
		UpVec = XMVector3TransformCoord(UpVec, m);

		XMStoreFloat3(&_UpVec, UpVec);
		XMStoreFloat3(&_SideVec, SideVec);

		//�オ�����ꂽ
		if (GetKey->Press(DIK_UP)) {
			XMMATRIX Mat = XMMatrixRotationAxis(SideVec, XMConvertToRadians(-0.5f));
			Vec = XMVector3TransformCoord(Vec, Mat);
		}
		//���������ꂽ
		else if (GetKey->Press(DIK_DOWN)) {
			XMMATRIX Mat = XMMatrixRotationAxis(SideVec, XMConvertToRadians(0.5f));
			Vec = XMVector3TransformCoord(Vec, Mat);
		}
		//�E�������ꂽ
		else if (GetKey->Press(DIK_RIGHT)) {
			XMMATRIX Mat = XMMatrixRotationAxis(UpVec, XMConvertToRadians(0.5f));
			Vec = XMVector3TransformCoord(Vec, Mat);
		}
		//���������ꂽ
		else if (GetKey->Press(DIK_LEFT)) {
			XMMATRIX Mat = XMMatrixRotationAxis(UpVec, XMConvertToRadians(-0.5f));
			Vec = XMVector3TransformCoord(Vec, Mat);
		}

		Vec += Pos;
		XMStoreFloat3(&m_DebugCameraData.vLook, Vec);
	}

	//�`��͈͕ύX
	if (true) {
		if (GetKey->Press(DIK_Q)) {
			m_DebugCameraData.vNear += 1.0f;
			//�t�@�[�ʂ𒴂�����߂�
			if (m_DebugCameraData.vNear >= m_DebugCameraData.vFar) {
				m_DebugCameraData.vNear -= 1.0f;
			}
		}
		else if (GetKey->Press(DIK_Z)) {
			m_DebugCameraData.vNear -= 1.0f;
			//0.1��菬�����Ȃ�����␳
			if (m_DebugCameraData.vNear <= 0.1f) {
				m_DebugCameraData.vNear = 0.1f;
			}
		}
		else if (GetKey->Press(DIK_E)) {
			m_DebugCameraData.vFar += 1.0f;
			//50�����ő�͈͂Ƃ���
			if (m_DebugCameraData.vFar >= 500000.0f) {
				m_DebugCameraData.vFar = 500000.0f;
			}
		}
		else if (GetKey->Press(DIK_C)) {
			m_DebugCameraData.vFar -= 1.0f;
			//�j�A��菬�����Ȃ���������
			if (m_DebugCameraData.vFar <= m_DebugCameraData.vNear) {
				m_DebugCameraData.vFar += 0.1f;
			}
		}
	}

	//��]�̏���
#endif
}


// =================�J������Lua����g�p���邽�߂�Glue�֐��Q==================== //
int cCamera::VectMoveGlue(lua_State * L)
{
	//�e�[�u������A�h���X�������Ă���
	auto p = toPointer<cCamera>(L);

	//�������擾���Ă���
	float scalar = (float)lua_tonumber(L, 2);

	//�֐����s
	p->VectMove(scalar);
	lua_settop(L, 0);	//�֐��Ăяo���p�X�^�b�N�͖߂�l�ł͖߂�l�ȊO�͍폜���Ė��Ȃ�

	return 0;		//�߂�l
}

int cCamera::LookRotGlue(lua_State * L)
{
	//�e�[�u������A�h���X�������Ă���
	auto p = toPointer<cCamera>(L);

	//�������擾���Ă���
	float x = (float)lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);
	float z = (float)lua_tonumber(L, 4);

	//�֐����s
	p->LookPointRotate( x,y,z );
	lua_settop(L, 0);	//�֐��Ăяo���p�X�^�b�N�͖߂�l�ł͖߂�l�ȊO�͍폜���Ė��Ȃ�

	return 0;		//�߂�l
}

int cCamera::SetPosGlue(lua_State * L)
{
	//�e�[�u������A�h���X�������Ă���
	auto p = toPointer<cCamera>(L);

	//�������擾���Ă���
	float x = (float)lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);
	float z = (float)lua_tonumber(L, 4);

	//�֐����s
	p->SetPosition(XMFLOAT3{ x, y, z });
	lua_settop(L, 0);	//�֐��Ăяo���p�X�^�b�N�͖߂�l�ł͖߂�l�ȊO�͍폜���Ė��Ȃ�

	return 0;		//�߂�l
}

int cCamera::SetLookGlue(lua_State * L)
{
	//�e�[�u������A�h���X�������Ă���
	auto p = toPointer<cCamera>(L);

	//�������擾���Ă���
	float x = (float)lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);
	float z = (float)lua_tonumber(L, 4);

	//�֐����s
	p->SetLookPoint(XMFLOAT3{ x, y, z });
	lua_settop(L, 0);	//�֐��Ăяo���p�X�^�b�N�͖߂�l�ł͖߂�l�ȊO�͍폜���Ė��Ȃ�

	return 0;		//�߂�l
}

int cCamera::SetUpGlue(lua_State * L)
{
	//�e�[�u������A�h���X�������Ă���
	auto p = toPointer<cCamera>(L);

	//�������擾���Ă���
	float x = (float)lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);
	float z = (float)lua_tonumber(L, 4);

	//�֐����s
	p->SetUpVect(XMFLOAT3{ x, y, z });
	lua_settop(L, 0);	//�֐��Ăяo���p�X�^�b�N�͖߂�l�ł͖߂�l�ȊO�͍폜���Ė��Ȃ�

	return 0;		//�߂�l
}

/// <summary>
/// �J�����̍��W�݂̂�ݒ肷��B�f�o�b�O�J�����̈ʒu���ς��
/// </summary>
/// <param name="_pos"></param>
void cCamera::SetPosition(const DirectX::XMFLOAT3 & _pos)
{
	m_CameraData.vPos = _pos;
	m_DebugCameraData.vPos = _pos;
}

/// <summary>
/// �J�����̏���������ς���B���܂�g��Ȃ�����
/// </summary>
/// <param name="_Up"></param>
void cCamera::SetUpVect(const DirectX::XMFLOAT3 & _Up)
{
	m_CameraData.vUp = _Up;
	m_DebugCameraData.vUp = _Up;
}

/// <summary>
/// �����_������ύX����
/// </summary>
/// <param name="_look"></param>
void cCamera::SetLookPoint(const DirectX::XMFLOAT3 & _look)
{
	m_CameraData.vLook = _look;
	m_DebugCameraData.vLook = _look;
}

/// <summary>
/// ���ׂẴf�[�^���\���̂ł��炤���ƂŐݒ肷��B�����ݒ�Ɏg�����Ƃ�z�肵�Ă�
/// </summary>
/// <param name="_data"></param>
void cCamera::SetCameraAllData(const CameraData & _data)
{
	m_CameraData = _data;
	m_DebugCameraData = _data;
}

/// <summary>
/// Near�AFar����ݒ肷��
/// </summary>
/// <param name="Near"></param>
/// <param name="Far"></param>
void cCamera::SetNear(const float Near, const float Far)
{
	m_CameraData.vNear = Near;
	m_DebugCameraData.vNear = Near;

	m_CameraData.vFar = Far;
	m_DebugCameraData.vFar = Far;
}

/// <summary>
/// �萔�����V�F�[�_�ɃZ�b�g����
/// </summary>
/// <param name="pass"></param>
/// <param name="debugFlag"></param>
void cCamera::SetConstBuffer(const int pass, bool debugFlag)
{
	if (debugFlag) {
		m_ShaderConst.data = { m_DebugCameraData.vPos.x,m_DebugCameraData.vPos.y,m_DebugCameraData.vPos.z,1.0f };
	}
	else {
		m_ShaderConst.data = { m_CameraData.vPos.x,m_CameraData.vPos.y,m_CameraData.vPos.z,1.0f };
	}
	m_ShaderConst.Set(pass);
}

/// <summary>
/// �J�����f�[�^���擾����
/// </summary>
/// <param name="_FreeCameraFlag"></param>
/// <returns></returns>
CameraData cCamera::GetCameraData(bool _FreeCameraFlag)
{
	if (!_FreeCameraFlag) {
		return m_CameraData;
	}
	else {
		return m_DebugCameraData;
	}
}

/// <summary>
/// �r���[�v���W�F�N�V���������擾����
/// </summary>
/// <param name="_FreeCameraFlag"></param>
/// <returns></returns>
ViewProj cCamera::GetViewProj(bool _FreeCameraFlag)
{
	if (!_FreeCameraFlag) {
		return m_CameraVP;
	}
		return m_DebugVP;
}

/// <summary>
/// �����Ă�������Ɉړ�������
/// </summary>
/// <param name="rate">�ړ���</param>
void cCamera::VectMove(const float rate)
{
	const XMFLOAT3& _VecZ = GetAxisZ();

	//�ړ��ʂ𑫂�����
	m_CameraData.vPos.x += _VecZ.x * rate;
	m_CameraData.vPos.y += _VecZ.y * rate;
	m_CameraData.vPos.z += _VecZ.z * rate;

	m_CameraData.vLook.x += _VecZ.x * rate;
	m_CameraData.vLook.y += _VecZ.y * rate;
	m_CameraData.vLook.z += _VecZ.z * rate;
}

/// <summary>
/// ���Ă���ʒu����]������
/// </summary>
/// <param name="rotX"></param>
/// <param name="rotY"></param>
/// <param name="rotZ"></param>
void cCamera::LookPointRotate(const float rotX, const float rotY, const float rotZ)
{
	//��]�͒����_��ύX���邱�Ƃōs��
	XMFLOAT3 LookVec = GetAxisZ();

	XMVECTOR Vec = XMLoadFloat3(&LookVec);		//���ꂪ��{�̉�]��
	XMVECTOR Pos = XMLoadFloat3(&m_CameraData.vPos);
	XMFLOAT3 _VectX = { 0,1.0f,0 };		//�������]�����ĉ����Ƃ���
	XMFLOAT3 _VectY = { 0,0,1.0f };			//�������]�����ďc���Ƃ���
	XMVECTOR VectX = XMLoadFloat3(&_VectX);
	XMVECTOR VectY = XMLoadFloat3(&_VectY);

	//���������߂�
	XMMATRIX m = XMMatrixRotationAxis(Vec, XMConvertToRadians(-90.0f));
	VectX = XMVector3TransformCoord(VectX, m);

	m = XMMatrixRotationX(XMConvertToRadians(-90.0f));
	VectY = XMVector3TransformCoord(VectY, m);

	XMStoreFloat3(&_VectY, VectY);
	XMStoreFloat3(&_VectX, VectX);

	if (rotX != 0) {
		XMMATRIX Mat = XMMatrixRotationAxis(VectX, XMConvertToRadians(rotX));
		Vec = XMVector3TransformCoord(Vec, Mat);
	}
	if (rotY != 0) {
		XMMATRIX Mat = XMMatrixRotationAxis(VectY, XMConvertToRadians(rotY));
		Vec = XMVector3TransformCoord(Vec, Mat);
	}
	if (rotZ != 0) {
		XMVECTOR AxisZ = XMLoadFloat3(&GetAxisZ());
		XMMATRIX Mat = XMMatrixRotationAxis(AxisZ, XMConvertToRadians(rotZ));
		
		XMVECTOR UpVect = XMVector3TransformCoord(VectY, Mat);
		XMStoreFloat3(&m_CameraData.vUp, Vec);
	}

	Vec += Pos;
	XMStoreFloat3(&m_CameraData.vLook, Vec);
}

/// <summary>
/// �J�����������Ă���������擾����
/// </summary>
/// <returns></returns>
const DirectX::XMFLOAT3 cCamera::GetAxisZ() const
{
	//�����x�N�g�����Z�o����
	DirectX::XMFLOAT3 _VecZ =
	{ m_CameraData.vLook.x - m_CameraData.vPos.x,
		m_CameraData.vLook.y - m_CameraData.vPos.y,
		m_CameraData.vLook.z - m_CameraData.vPos.z };
	DirectX::XMVECTOR Vec = DirectX::XMLoadFloat3(&_VecZ);
	Vec = DirectX::XMVector3Normalize(Vec); 		//�ړ��ʂ𐳋K��

	XMStoreFloat3(&_VecZ, Vec);

	return _VecZ;
}

/// <summary>
/// Lua�ɃJ��������t�^����
/// </summary>
/// <param name="L"></param>
/// <param name="LuaVarName"></param>
void cCamera::AddFunctionToLua(lua_State * L, std::string LuaVarName)
{
	const luaL_Reg funcs[] = {
		{ "VectMove", cCamera::VectMoveGlue },
		{ "LookRot", cCamera::LookRotGlue },
		{ "SetLook", cCamera::SetLookGlue },
		{ "SetPos", cCamera::SetPosGlue },
		{ "SetUp", cCamera::SetUpGlue },
		{ NULL, NULL }
	};

	SetLuaGlobalObjct<cCamera>(L, LuaVarName.c_str(), funcs, this);
}
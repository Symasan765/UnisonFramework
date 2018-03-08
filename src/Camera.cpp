/*=================================================
//								FILE : Camera.cpp
//		ファイル説明 :
//		ゲームシーン内で取り扱うメインカメラを実装するクラス
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "Camera.h"
#include "Input.h"			//フリーカメラの操作用に利用
#include "LuaConvert.h"

using namespace LuaConv;
using namespace DirectX;

/// <summary>
/// カメラのコンストラクタ
/// </summary>
cCamera::cCamera()
{
	//カメラの初期値を設定する
	SetCameraAllData({ {0.0f,10.0f,-15.0f},{0.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},0.2f,200.0f });
	m_ShaderConst.data = { m_CameraData.vPos.x,m_CameraData.vPos.y,m_CameraData.vPos.z,1.0f };
}

cCamera::~cCamera()
{
}

/// <summary>
/// 設定されている情報を元にビュー、プロジェクションを作成する
/// </summary>
/// <returns></returns>
const ViewProj& cCamera::Projection(bool _FreeCameraFlag)
{
	XMMATRIX View = XMMatrixIdentity();
	XMMATRIX Proj = XMMatrixIdentity();
	XMFLOAT3 pos, look, up;
	float Near, Far;
	//フリーカメラかどうかで設定を変える
	if (!_FreeCameraFlag) {
		//通常レンダリング
		pos = m_CameraData.vPos;
		look = m_CameraData.vLook;
		up = m_CameraData.vUp;
		Near = m_CameraData.vNear;
		Far = m_CameraData.vFar;
	}
	else {
		//フリーカメラレンダリング
		pos = m_DebugCameraData.vPos;
		look = m_DebugCameraData.vLook;
		up = m_DebugCameraData.vUp;
		Near = m_DebugCameraData.vNear;
		Far = m_DebugCameraData.vFar;
	}

	//座標位置,上方向ベクトル,注視点ベクトルを取得
	XMVECTOR eyePosition = XMLoadFloat3(&pos);
	XMVECTOR upDirection = XMLoadFloat3(&up);
	XMVECTOR focusPosition = XMLoadFloat3(&look);

	View = DirectX::XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);
	Proj = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(45.0f),		// 視野角
		(float)WINDOW_SIZE_X / (float)WINDOW_SIZE_Y,	// アスペクト比
		Near,							// 前方投影面までの距離
		Far);							// 後方投影面までの距離


	//フリーカメラフラグによって渡す情報を変える
	if (!_FreeCameraFlag) {
		//通常レンダリング
		XMStoreFloat4x4(&m_CameraVP.vProj, Proj);
		XMStoreFloat4x4(&m_CameraVP.vView, View);
		XMMATRIX mat = View * Proj;		//ビュー,プロジェクション行列計算
		XMStoreFloat4x4(&m_CameraVP.vVP, mat);
		return m_CameraVP;
	}

	//上記に入らなかった = フリーカメラ
	XMStoreFloat4x4(&m_DebugVP.vProj, Proj);
	XMStoreFloat4x4(&m_DebugVP.vView, View);
	XMMATRIX mat = View * Proj;		//ビュー,プロジェクション行列計算
	XMStoreFloat4x4(&m_DebugVP.vVP, mat);
	return m_DebugVP;
}

/// <summary>
/// フリーカメラの操作を記述する
/// </summary>
void cCamera::FreeCameraOperation()
{
#if defined(DEBUG) || defined(_DEBUG)
	//エンターキーが押されたら通常カメラの位置に戻す
	if (GetKey->Trigger(DIK_RETURN)) {
		m_DebugCameraData = m_CameraData;
	}

	//移動計算
	if (true) {
		//移動ベクトル
		DirectX::XMFLOAT3 _VecZ =
		{ m_DebugCameraData.vLook.x - m_DebugCameraData.vPos.x,
			m_DebugCameraData.vLook.y - m_DebugCameraData.vPos.y,
			m_DebugCameraData.vLook.z - m_DebugCameraData.vPos.z };
		DirectX::XMVECTOR Vec = DirectX::XMLoadFloat3(&_VecZ);
		Vec = DirectX::XMVector3Normalize(Vec); 		//移動量を正規化

		XMFLOAT3 _VecX = { 0,1.0f,0 };
		XMVECTOR VecX = XMLoadFloat3(&_VecX);			//こいつを移動方向を軸にして90度回転させる
		XMMATRIX m = XMMatrixRotationAxis(Vec, XMConvertToRadians(-90.0f));
		VecX = XMVector3TransformCoord(VecX, m);
		VecX *= 0.1f;
		XMStoreFloat3(&_VecX, VecX);

		Vec *= 0.1f;							//移動量
		XMStoreFloat3(&_VecZ, Vec);

		//移動の処理
		if (GetKey->Press(DIK_W)) {
			//前進
			m_DebugCameraData.vPos.x += _VecZ.x;
			m_DebugCameraData.vPos.y += _VecZ.y;
			m_DebugCameraData.vPos.z += _VecZ.z;

			m_DebugCameraData.vLook.x += _VecZ.x;
			m_DebugCameraData.vLook.y += _VecZ.y;
			m_DebugCameraData.vLook.z += _VecZ.z;
		}
		else if (GetKey->Press(DIK_S)) {
			//後退
			m_DebugCameraData.vPos.x -= _VecZ.x;
			m_DebugCameraData.vPos.y -= _VecZ.y;
			m_DebugCameraData.vPos.z -= _VecZ.z;

			m_DebugCameraData.vLook.x -= _VecZ.x;
			m_DebugCameraData.vLook.y -= _VecZ.y;
			m_DebugCameraData.vLook.z -= _VecZ.z;
		}

		if (GetKey->Press(DIK_D)) {
			//右進
			m_DebugCameraData.vPos.x += _VecX.x;
			m_DebugCameraData.vPos.y += _VecX.y;
			m_DebugCameraData.vPos.z += _VecX.z;

			m_DebugCameraData.vLook.x += _VecX.x;
			m_DebugCameraData.vLook.y += _VecX.y;
			m_DebugCameraData.vLook.z += _VecX.z;
		}
		else if (GetKey->Press(DIK_A)) {
			//左進
			m_DebugCameraData.vPos.x -= _VecX.x;
			m_DebugCameraData.vPos.y -= _VecX.y;
			m_DebugCameraData.vPos.z -= _VecX.z;

			m_DebugCameraData.vLook.x -= _VecX.x;
			m_DebugCameraData.vLook.y -= _VecX.y;
			m_DebugCameraData.vLook.z -= _VecX.z;
		}
	}

	//回転計算
	if (true) {
		//回転は注視点を変更することで行う
		XMFLOAT3 LookVec =
		{ m_DebugCameraData.vLook.x - m_DebugCameraData.vPos.x,
			m_DebugCameraData.vLook.y - m_DebugCameraData.vPos.y,
			m_DebugCameraData.vLook.z - m_DebugCameraData.vPos.z };

		XMVECTOR Vec = XMLoadFloat3(&LookVec);		//これが基本の回転軸
		XMVECTOR Pos = XMLoadFloat3(&m_DebugCameraData.vPos);
		XMFLOAT3 _SideVec = { 0,1.0f,0 };		//これを回転させて横軸とする
		XMFLOAT3 _UpVec = { 0,0,1.0f };			//これを回転させて縦軸とする
		XMVECTOR SideVec = XMLoadFloat3(&_SideVec);
		XMVECTOR UpVec = XMLoadFloat3(&_UpVec);

		//横軸を求める
		XMMATRIX m = XMMatrixRotationAxis(Vec, XMConvertToRadians(-90.0f));
		SideVec = XMVector3TransformCoord(SideVec, m);

		m = XMMatrixRotationX(XMConvertToRadians(-90.0f));
		UpVec = XMVector3TransformCoord(UpVec, m);

		XMStoreFloat3(&_UpVec, UpVec);
		XMStoreFloat3(&_SideVec, SideVec);

		//上が押された
		if (GetKey->Press(DIK_UP)) {
			XMMATRIX Mat = XMMatrixRotationAxis(SideVec, XMConvertToRadians(-0.5f));
			Vec = XMVector3TransformCoord(Vec, Mat);
		}
		//下が押された
		else if (GetKey->Press(DIK_DOWN)) {
			XMMATRIX Mat = XMMatrixRotationAxis(SideVec, XMConvertToRadians(0.5f));
			Vec = XMVector3TransformCoord(Vec, Mat);
		}
		//右が押された
		else if (GetKey->Press(DIK_RIGHT)) {
			XMMATRIX Mat = XMMatrixRotationAxis(UpVec, XMConvertToRadians(0.5f));
			Vec = XMVector3TransformCoord(Vec, Mat);
		}
		//左が押された
		else if (GetKey->Press(DIK_LEFT)) {
			XMMATRIX Mat = XMMatrixRotationAxis(UpVec, XMConvertToRadians(-0.5f));
			Vec = XMVector3TransformCoord(Vec, Mat);
		}

		Vec += Pos;
		XMStoreFloat3(&m_DebugCameraData.vLook, Vec);
	}

	//描画範囲変更
	if (true) {
		if (GetKey->Press(DIK_Q)) {
			m_DebugCameraData.vNear += 1.0f;
			//ファー面を超えたら戻す
			if (m_DebugCameraData.vNear >= m_DebugCameraData.vFar) {
				m_DebugCameraData.vNear -= 1.0f;
			}
		}
		else if (GetKey->Press(DIK_Z)) {
			m_DebugCameraData.vNear -= 1.0f;
			//0.1より小さくなったら補正
			if (m_DebugCameraData.vNear <= 0.1f) {
				m_DebugCameraData.vNear = 0.1f;
			}
		}
		else if (GetKey->Press(DIK_E)) {
			m_DebugCameraData.vFar += 1.0f;
			//50万を最大範囲とする
			if (m_DebugCameraData.vFar >= 500000.0f) {
				m_DebugCameraData.vFar = 500000.0f;
			}
		}
		else if (GetKey->Press(DIK_C)) {
			m_DebugCameraData.vFar -= 1.0f;
			//ニアより小さくなったら取りやめ
			if (m_DebugCameraData.vFar <= m_DebugCameraData.vNear) {
				m_DebugCameraData.vFar += 0.1f;
			}
		}
	}

	//回転の処理
#endif
}


// =================カメラをLuaから使用するためのGlue関数群==================== //
int cCamera::VectMoveGlue(lua_State * L)
{
	//テーブルからアドレスを持ってくる
	auto p = toPointer<cCamera>(L);

	//引数を取得していく
	float scalar = (float)lua_tonumber(L, 2);

	//関数実行
	p->VectMove(scalar);
	lua_settop(L, 0);	//関数呼び出し用スタックは戻り値では戻り値以外は削除して問題ない

	return 0;		//戻り値
}

int cCamera::LookRotGlue(lua_State * L)
{
	//テーブルからアドレスを持ってくる
	auto p = toPointer<cCamera>(L);

	//引数を取得していく
	float x = (float)lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);
	float z = (float)lua_tonumber(L, 4);

	//関数実行
	p->LookPointRotate( x,y,z );
	lua_settop(L, 0);	//関数呼び出し用スタックは戻り値では戻り値以外は削除して問題ない

	return 0;		//戻り値
}

int cCamera::SetPosGlue(lua_State * L)
{
	//テーブルからアドレスを持ってくる
	auto p = toPointer<cCamera>(L);

	//引数を取得していく
	float x = (float)lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);
	float z = (float)lua_tonumber(L, 4);

	//関数実行
	p->SetPosition(XMFLOAT3{ x, y, z });
	lua_settop(L, 0);	//関数呼び出し用スタックは戻り値では戻り値以外は削除して問題ない

	return 0;		//戻り値
}

int cCamera::SetLookGlue(lua_State * L)
{
	//テーブルからアドレスを持ってくる
	auto p = toPointer<cCamera>(L);

	//引数を取得していく
	float x = (float)lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);
	float z = (float)lua_tonumber(L, 4);

	//関数実行
	p->SetLookPoint(XMFLOAT3{ x, y, z });
	lua_settop(L, 0);	//関数呼び出し用スタックは戻り値では戻り値以外は削除して問題ない

	return 0;		//戻り値
}

int cCamera::SetUpGlue(lua_State * L)
{
	//テーブルからアドレスを持ってくる
	auto p = toPointer<cCamera>(L);

	//引数を取得していく
	float x = (float)lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);
	float z = (float)lua_tonumber(L, 4);

	//関数実行
	p->SetUpVect(XMFLOAT3{ x, y, z });
	lua_settop(L, 0);	//関数呼び出し用スタックは戻り値では戻り値以外は削除して問題ない

	return 0;		//戻り値
}

/// <summary>
/// カメラの座標のみを設定する。デバッグカメラの位置も変わる
/// </summary>
/// <param name="_pos"></param>
void cCamera::SetPosition(const DirectX::XMFLOAT3 & _pos)
{
	m_CameraData.vPos = _pos;
	m_DebugCameraData.vPos = _pos;
}

/// <summary>
/// カメラの上方向だけ変える。あまり使わないかも
/// </summary>
/// <param name="_Up"></param>
void cCamera::SetUpVect(const DirectX::XMFLOAT3 & _Up)
{
	m_CameraData.vUp = _Up;
	m_DebugCameraData.vUp = _Up;
}

/// <summary>
/// 注視点だけを変更する
/// </summary>
/// <param name="_look"></param>
void cCamera::SetLookPoint(const DirectX::XMFLOAT3 & _look)
{
	m_CameraData.vLook = _look;
	m_DebugCameraData.vLook = _look;
}

/// <summary>
/// すべてのデータを構造体でもらうことで設定する。初期設定に使うことを想定してい
/// </summary>
/// <param name="_data"></param>
void cCamera::SetCameraAllData(const CameraData & _data)
{
	m_CameraData = _data;
	m_DebugCameraData = _data;
}

/// <summary>
/// Near、Far情報を設定する
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
/// 定数情報をシェーダにセットする
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
/// カメラデータを取得する
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
/// ビュープロジェクション情報を取得する
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
/// 向いている方向に移動させる
/// </summary>
/// <param name="rate">移動量</param>
void cCamera::VectMove(const float rate)
{
	const XMFLOAT3& _VecZ = GetAxisZ();

	//移動量を足しこむ
	m_CameraData.vPos.x += _VecZ.x * rate;
	m_CameraData.vPos.y += _VecZ.y * rate;
	m_CameraData.vPos.z += _VecZ.z * rate;

	m_CameraData.vLook.x += _VecZ.x * rate;
	m_CameraData.vLook.y += _VecZ.y * rate;
	m_CameraData.vLook.z += _VecZ.z * rate;
}

/// <summary>
/// 見ている位置を回転させる
/// </summary>
/// <param name="rotX"></param>
/// <param name="rotY"></param>
/// <param name="rotZ"></param>
void cCamera::LookPointRotate(const float rotX, const float rotY, const float rotZ)
{
	//回転は注視点を変更することで行う
	XMFLOAT3 LookVec = GetAxisZ();

	XMVECTOR Vec = XMLoadFloat3(&LookVec);		//これが基本の回転軸
	XMVECTOR Pos = XMLoadFloat3(&m_CameraData.vPos);
	XMFLOAT3 _VectX = { 0,1.0f,0 };		//これを回転させて横軸とする
	XMFLOAT3 _VectY = { 0,0,1.0f };			//これを回転させて縦軸とする
	XMVECTOR VectX = XMLoadFloat3(&_VectX);
	XMVECTOR VectY = XMLoadFloat3(&_VectY);

	//横軸を求める
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
/// カメラが向いている方向を取得する
/// </summary>
/// <returns></returns>
const DirectX::XMFLOAT3 cCamera::GetAxisZ() const
{
	//方向ベクトルを算出する
	DirectX::XMFLOAT3 _VecZ =
	{ m_CameraData.vLook.x - m_CameraData.vPos.x,
		m_CameraData.vLook.y - m_CameraData.vPos.y,
		m_CameraData.vLook.z - m_CameraData.vPos.z };
	DirectX::XMVECTOR Vec = DirectX::XMLoadFloat3(&_VecZ);
	Vec = DirectX::XMVector3Normalize(Vec); 		//移動量を正規化

	XMStoreFloat3(&_VecZ, Vec);

	return _VecZ;
}

/// <summary>
/// Luaにカメラ情報を付与する
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
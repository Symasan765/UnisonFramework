/*=================================================
//								FILE : DeferredConst.cpp
//		ファイル説明 :
//		ディファードレンダリングで利用する定数を保持するクラス
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "DeferredConst.h"
#include "DirectX11.h"
#include "ShadowMap.h"
using namespace DirectX;


DirectX::XMFLOAT3 cDeferredConst::m_LightPos = {0.0f,1.0f,0.0f};			//ライト位置
DirectX::XMFLOAT3 cDeferredConst::m_Lookpt = {0.0f,0.0f,0.0f};			//カメラ注視点
DirectX::XMFLOAT3 cDeferredConst::m_CameraPos = {0,0,0};		//カメラ位置
DirectX::XMFLOAT3 cDeferredConst::m_CameraUp = {0,1,0};
DirectX::XMFLOAT4X4 cDeferredConst::m_View;			//ビュー行列
DirectX::XMFLOAT4X4 cDeferredConst::m_DepthProj;			//プロジェクション行列
DirectX::XMFLOAT4X4 cDeferredConst::m_Proj;			//プロジェクション行列
float cDeferredConst::m_Near;
float cDeferredConst::m_Far;

cDeferredConst::cDeferredConst()
{
	m_MaterialConst.data.Flag = { 0,0,0,0 };
	m_MaterialConst.data.k_a = { 0,0,0,0 };
	m_MaterialConst.data.k_d = { 0,0,0,0 };
	m_MaterialConst.data.k_s = { 0,0,0,0 };
	m_MaterialConst.data.Emission = { 0,0,0,0 };

	//行列はすべて単位行列で初期化しておく
	XMStoreFloat4x4(&m_World, XMMatrixIdentity());
	m_MatrixConst.data.mW = m_MatrixConst.data.mWLP = m_MatrixConst.data.mWLPB = m_MatrixConst.data.mWVP = m_World;

	m_Near = 1.0f;
	m_Far = 100.0f;
}

cDeferredConst::~cDeferredConst()
{
}

void cDeferredConst::SetShader(bool _mtrOnly)
{
	//マテリアルの設定のみでいい場合は飛ばす
	if (!_mtrOnly) {
		//まず各行列を作っていく
		XMMATRIX _w = XMLoadFloat4x4(&m_World);
		XMMATRIX _wvp = _w * XMLoadFloat4x4(&m_View) * XMLoadFloat4x4(&m_Proj);
		XMVECTOR _lightVec = XMLoadFloat3(&m_LightPos);
		_lightVec = XMVector3Normalize(_lightVec);		//正規化
		_lightVec *= m_Far * 0.95f;									//ファーの半分で
		_lightVec += XMLoadFloat3(&m_Lookpt);
		XMMATRIX _l = XMMatrixLookAtLH(_lightVec, XMLoadFloat3(&m_Lookpt), { 0,1,0 });		//ライト位置からのビュー

		//テクスチャ座標に変換する行列
		DirectX::XMFLOAT4X4 b = {
			0.5f,0.0f,0.0f,0.0f,
			0.0f,-0.5f,0.0f,0.0f,
			0.0f,0.0f,1.0f,0.0f,
			0.5f,0.5f,0.0f,1.0f
		};
		XMMATRIX _b = DirectX::XMLoadFloat4x4(&b);

		//それぞれの行列を格納していく
		XMStoreFloat4x4(&m_MatrixConst.data.mW, XMMatrixTranspose(_w));
		XMStoreFloat4x4(&m_MatrixConst.data.mWVP, XMMatrixTranspose(_wvp));
		XMStoreFloat4x4(&m_MatrixConst.data.mWLP, XMMatrixTranspose(_w * _l * XMLoadFloat4x4(&m_DepthProj)));
		XMStoreFloat4x4(&m_MatrixConst.data.mWLPB, XMMatrixTranspose(_w * _l * XMLoadFloat4x4(&m_DepthProj) * _b));
		XMStoreFloat4x4(&m_MatrixConst.data.mDepthWVP, XMMatrixTranspose(_w * XMLoadFloat4x4(&m_View) * XMLoadFloat4x4(&m_DepthProj)));

		//シェーダをセット
		m_MatrixConst.Set(0);
	}
	m_MaterialConst.Set(1);		//マテリアルはすでに情報が設定されているものとする
}

void cDeferredConst::SetCameraData(const CameraData& _cameraData, ViewProj _vp)
{
	//カメラ情報セット
	m_Lookpt = _cameraData.vLook;
	m_CameraPos = _cameraData.vPos;
	m_CameraUp = _cameraData.vUp;
	m_View = _vp.vView;
	m_Proj = _vp.vProj;

	XMMATRIX Proj;
	XMFLOAT4X4 ProjBuf;
	Proj = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(SHADOW_MAP_VIEWING_ANGLE),		// 視野角
		(float)SHADOW_MAP_WIDTH / (float)SHADOW_MAP_HEIGHT,	// アスペクト比
		SHADOW_MAP_NEAR,							// 前方投影面までの距離
		SHADOW_MAP_FAR);							// 後方投影面までの距離
	XMStoreFloat4x4(&ProjBuf, Proj);
	//深度値の線形補間(参考 : http://marupeke296.com/DXG_No71_depthbuffer.html)
	float _33 = 1.0f / (SHADOW_MAP_FAR - SHADOW_MAP_NEAR);
	float _43 = -SHADOW_MAP_NEAR / (SHADOW_MAP_FAR - SHADOW_MAP_NEAR);
	ProjBuf._33 = _33;
	ProjBuf._43 = _43;
	m_DepthProj = ProjBuf;
	m_Near = SHADOW_MAP_NEAR;
	m_Far = SHADOW_MAP_FAR;
}

void cDeferredConst::SetLightData(const DirectX::XMFLOAT3 & _light)
{
	m_LightPos = _light;
}

void cDeferredConst::SetWorldMatrix(const DirectX::XMFLOAT4X4 & _w)
{
	m_World = _w;
}

void cDeferredConst::SetEmission(DirectX::XMFLOAT4 _color)
{
	m_MaterialConst.data.Emission = _color;
}

void cDeferredConst::SetMaterial(DirectX::XMFLOAT4 _a, DirectX::XMFLOAT4 _d, DirectX::XMFLOAT4 _s)
{
	m_MaterialConst.data.k_a = _a;
	m_MaterialConst.data.k_d = _d;
	m_MaterialConst.data.k_s = _s;
}

void cDeferredConst::SetShaderType(const int _type)
{
	m_MaterialConst.data.Flag.x = _type;
}

const CameraData cDeferredConst::GetCameraData() const
{
	CameraData data;
	data.vFar = m_Far;
	data.vLook = m_Lookpt;
	data.vNear = m_Near;
	data.vPos = m_CameraPos;
	data.vUp = m_CameraUp;

	return data;
}

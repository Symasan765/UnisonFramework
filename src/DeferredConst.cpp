/*=================================================
//								FILE : DeferredConst.cpp
//		�t�@�C������ :
//		�f�B�t�@�[�h�����_�����O�ŗ��p����萔��ێ�����N���X
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "DeferredConst.h"
#include "DirectX11.h"
#include "ShadowMap.h"
using namespace DirectX;


DirectX::XMFLOAT3 cDeferredConst::m_LightPos = {0.0f,1.0f,0.0f};			//���C�g�ʒu
DirectX::XMFLOAT3 cDeferredConst::m_Lookpt = {0.0f,0.0f,0.0f};			//�J���������_
DirectX::XMFLOAT3 cDeferredConst::m_CameraPos = {0,0,0};		//�J�����ʒu
DirectX::XMFLOAT3 cDeferredConst::m_CameraUp = {0,1,0};
DirectX::XMFLOAT4X4 cDeferredConst::m_View;			//�r���[�s��
DirectX::XMFLOAT4X4 cDeferredConst::m_DepthProj;			//�v���W�F�N�V�����s��
DirectX::XMFLOAT4X4 cDeferredConst::m_Proj;			//�v���W�F�N�V�����s��
float cDeferredConst::m_Near;
float cDeferredConst::m_Far;

cDeferredConst::cDeferredConst()
{
	m_MaterialConst.data.Flag = { 0,0,0,0 };
	m_MaterialConst.data.k_a = { 0,0,0,0 };
	m_MaterialConst.data.k_d = { 0,0,0,0 };
	m_MaterialConst.data.k_s = { 0,0,0,0 };
	m_MaterialConst.data.Emission = { 0,0,0,0 };

	//�s��͂��ׂĒP�ʍs��ŏ��������Ă���
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
	//�}�e���A���̐ݒ�݂̂ł����ꍇ�͔�΂�
	if (!_mtrOnly) {
		//�܂��e�s�������Ă���
		XMMATRIX _w = XMLoadFloat4x4(&m_World);
		XMMATRIX _wvp = _w * XMLoadFloat4x4(&m_View) * XMLoadFloat4x4(&m_Proj);
		XMVECTOR _lightVec = XMLoadFloat3(&m_LightPos);
		_lightVec = XMVector3Normalize(_lightVec);		//���K��
		_lightVec *= m_Far * 0.95f;									//�t�@�[�̔�����
		_lightVec += XMLoadFloat3(&m_Lookpt);
		XMMATRIX _l = XMMatrixLookAtLH(_lightVec, XMLoadFloat3(&m_Lookpt), { 0,1,0 });		//���C�g�ʒu����̃r���[

		//�e�N�X�`�����W�ɕϊ�����s��
		DirectX::XMFLOAT4X4 b = {
			0.5f,0.0f,0.0f,0.0f,
			0.0f,-0.5f,0.0f,0.0f,
			0.0f,0.0f,1.0f,0.0f,
			0.5f,0.5f,0.0f,1.0f
		};
		XMMATRIX _b = DirectX::XMLoadFloat4x4(&b);

		//���ꂼ��̍s����i�[���Ă���
		XMStoreFloat4x4(&m_MatrixConst.data.mW, XMMatrixTranspose(_w));
		XMStoreFloat4x4(&m_MatrixConst.data.mWVP, XMMatrixTranspose(_wvp));
		XMStoreFloat4x4(&m_MatrixConst.data.mWLP, XMMatrixTranspose(_w * _l * XMLoadFloat4x4(&m_DepthProj)));
		XMStoreFloat4x4(&m_MatrixConst.data.mWLPB, XMMatrixTranspose(_w * _l * XMLoadFloat4x4(&m_DepthProj) * _b));
		XMStoreFloat4x4(&m_MatrixConst.data.mDepthWVP, XMMatrixTranspose(_w * XMLoadFloat4x4(&m_View) * XMLoadFloat4x4(&m_DepthProj)));

		//�V�F�[�_���Z�b�g
		m_MatrixConst.Set(0);
	}
	m_MaterialConst.Set(1);		//�}�e���A���͂��łɏ�񂪐ݒ肳��Ă�����̂Ƃ���
}

void cDeferredConst::SetCameraData(const CameraData& _cameraData, ViewProj _vp)
{
	//�J�������Z�b�g
	m_Lookpt = _cameraData.vLook;
	m_CameraPos = _cameraData.vPos;
	m_CameraUp = _cameraData.vUp;
	m_View = _vp.vView;
	m_Proj = _vp.vProj;

	XMMATRIX Proj;
	XMFLOAT4X4 ProjBuf;
	Proj = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(SHADOW_MAP_VIEWING_ANGLE),		// ����p
		(float)SHADOW_MAP_WIDTH / (float)SHADOW_MAP_HEIGHT,	// �A�X�y�N�g��
		SHADOW_MAP_NEAR,							// �O�����e�ʂ܂ł̋���
		SHADOW_MAP_FAR);							// ������e�ʂ܂ł̋���
	XMStoreFloat4x4(&ProjBuf, Proj);
	//�[�x�l�̐��`���(�Q�l : http://marupeke296.com/DXG_No71_depthbuffer.html)
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

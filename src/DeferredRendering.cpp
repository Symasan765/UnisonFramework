/*=================================================
//								FILE : DeferredRendering.cpp
//		�t�@�C������ :
//		�f�B�t�@�[�h�����_�����O���Ǘ�����V���O���g���N���X
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "DeferredRendering.h"
#include "DirectX11.h"

using namespace DirectX;
void cDeferredRendering::SetDeferredRendering(ID3D11ShaderResourceView* _DepthMap)
{
	RTTex Target[TARGET_MAX];
	for (int i = 0; i < TARGET_MAX; i++) {
		//�����_�[�^�[�Q�b�g�̏����擾����
		Target[i] = m_pRenderTergetTex[i].GetRenderTextureStruct();
	}
	D3D11_VIEWPORT _VP[TARGET_MAX];
	ID3D11ShaderResourceView* _SRV[TARGET_MAX];
	ID3D11RenderTargetView* _RTV[TARGET_MAX];
	ID3D11DepthStencilView* _DSV[TARGET_MAX];

	for (int i = 0; i < TARGET_MAX; i++) {
		_VP[i] = Target[i].m_ViewPort;
		_SRV[i] = Target[i].m_pTextureView;
		_RTV[i] = Target[i].m_pDepthRenderTargetView;
		_DSV[i] = Target[i].m_pDepthStencilView;
	}

	//===================�`��^�[�Q�b�g��ύX����=======================//
	GetDirectX::Context()->RSSetViewports(TARGET_MAX, _VP);
	GetDirectX::Context()->OMSetRenderTargets(TARGET_MAX, _RTV,*_DSV);
	for (int i = 0; i < TARGET_MAX; i++) {
		GetDirectX::Context()->ClearRenderTargetView(_RTV[i], Target[i].ClereColor);//��ʃN���A
		GetDirectX::Context()->ClearDepthStencilView(_DSV[i], D3D11_CLEAR_DEPTH, 1.0f, 0);//�[�x�o�b�t�@�N���A
	}
	GetDirectX::Context()->PSSetSamplers(0, 1, &m_pSampleLinear);
	GetDirectX::Context()->VSSetSamplers(0, 1, &m_pSampleLinear);

	GetDirectX::Context()->PSSetShaderResources(4, 1, &_DepthMap);
}

ID3D11ShaderResourceView * cDeferredRendering::GetResourceView(int _no)
{
	return m_pRenderTergetTex[_no].GetTextureResourceView();
}

GBuffer cDeferredRendering::GetGraphicBuffer()
{
	GetDirectX::Context()->PSSetShaderResources(0, NULL, NULL);
	GetDirectX::Context()->VSSetShaderResources(0, NULL, NULL);
	GBuffer buf = {
		GetResourceView(0),
		GetResourceView(1),
		GetResourceView(2),
		GetResourceView(3),
		GetResourceView(4),
	};
	return buf;
}

void cDeferredRendering::SetDeferredShader()
{
	m_Shader->Set();
}

/*
DIFFUSE = 0,		//�J���[
POSITION,			//���W(w�ɐ[�x�l���i�[����)
NORMAL,				//�@��
AMBIENT,			//�g�U��
SPECULER,			//���ˌ�
EMISSION,			//������
SHADOW_MAP,		//�V���h�E�}�b�v
TARGET_MAX		//�쐬�e�N�X�`����
*/
cDeferredRendering::cDeferredRendering()
{
	m_pRenderTergetTex = new cRenderTargetTex[TARGET_MAX];
	XMUINT3 color[TARGET_MAX] = {
		{	0,	0,	0},		//�J���[
		{ 0,	0,	0 },		//���W
		{ 0,	0,	0 },		//�@��
		{ 0,	0,	0 },		//���P�x���
		{ 0,	0,	0 },		//SSS
	};
	DXGI_FORMAT form[TARGET_MAX] = {
		DXGI_FORMAT_R16G16B16A16_FLOAT,		//�J���[
		DXGI_FORMAT_R32G32B32A32_FLOAT,		//���W
		DXGI_FORMAT_R32G32B32A32_FLOAT,		//�@��
		DXGI_FORMAT_R16G16B16A16_FLOAT,		//���P�x���
		DXGI_FORMAT_R16G16B16A16_FLOAT,		//SSS
	};

	//������
	for (int i = 0; i < TARGET_MAX; i++) {
		m_pRenderTergetTex[i].RTCreate(1280.0f, 720.0f, color[i], form[i]);
	}

	//�e�N�X�`���[�p�T���v���[�쐬
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));

	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	GetDirectX::Device()->CreateSamplerState(&SamDesc, &m_pSampleLinear);

	//�V�F�[�_�f�[�^���[�h
	m_Shader = SHADER::GetInstance()->LoadShaderFile("Deferred1.hlsl", inPOSITION | inNORMAL | inTEX_UV | inBone | inWeight,true);
}

cDeferredRendering::~cDeferredRendering()
{
	delete[] m_pRenderTergetTex;
	m_pRenderTergetTex = nullptr;
	SAFE_RELEASE(m_pSampleLinear);
}

/*=================================================
//								FILE : DeferredRendering.cpp
//		ファイル説明 :
//		ディファードレンダリングを管理するシングルトンクラス
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "DeferredRendering.h"
#include "DirectX11.h"

using namespace DirectX;
void cDeferredRendering::SetDeferredRendering(ID3D11ShaderResourceView* _DepthMap)
{
	RTTex Target[TARGET_MAX];
	for (int i = 0; i < TARGET_MAX; i++) {
		//レンダーターゲットの情報を取得する
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

	//===================描画ターゲットを変更する=======================//
	GetDirectX::Context()->RSSetViewports(TARGET_MAX, _VP);
	GetDirectX::Context()->OMSetRenderTargets(TARGET_MAX, _RTV,*_DSV);
	for (int i = 0; i < TARGET_MAX; i++) {
		GetDirectX::Context()->ClearRenderTargetView(_RTV[i], Target[i].ClereColor);//画面クリア
		GetDirectX::Context()->ClearDepthStencilView(_DSV[i], D3D11_CLEAR_DEPTH, 1.0f, 0);//深度バッファクリア
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
DIFFUSE = 0,		//カラー
POSITION,			//座標(wに深度値を格納する)
NORMAL,				//法線
AMBIENT,			//拡散光
SPECULER,			//反射光
EMISSION,			//自発光
SHADOW_MAP,		//シャドウマップ
TARGET_MAX		//作成テクスチャ数
*/
cDeferredRendering::cDeferredRendering()
{
	m_pRenderTergetTex = new cRenderTargetTex[TARGET_MAX];
	XMUINT3 color[TARGET_MAX] = {
		{	0,	0,	0},		//カラー
		{ 0,	0,	0 },		//座標
		{ 0,	0,	0 },		//法線
		{ 0,	0,	0 },		//高輝度情報
		{ 0,	0,	0 },		//SSS
	};
	DXGI_FORMAT form[TARGET_MAX] = {
		DXGI_FORMAT_R16G16B16A16_FLOAT,		//カラー
		DXGI_FORMAT_R32G32B32A32_FLOAT,		//座標
		DXGI_FORMAT_R32G32B32A32_FLOAT,		//法線
		DXGI_FORMAT_R16G16B16A16_FLOAT,		//高輝度情報
		DXGI_FORMAT_R16G16B16A16_FLOAT,		//SSS
	};

	//初期化
	for (int i = 0; i < TARGET_MAX; i++) {
		m_pRenderTergetTex[i].RTCreate(1280.0f, 720.0f, color[i], form[i]);
	}

	//テクスチャー用サンプラー作成
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));

	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	GetDirectX::Device()->CreateSamplerState(&SamDesc, &m_pSampleLinear);

	//シェーダデータロード
	m_Shader = SHADER::GetInstance()->LoadShaderFile("Deferred1.hlsl", inPOSITION | inNORMAL | inTEX_UV | inBone | inWeight,true);
}

cDeferredRendering::~cDeferredRendering()
{
	delete[] m_pRenderTergetTex;
	m_pRenderTergetTex = nullptr;
	SAFE_RELEASE(m_pSampleLinear);
}

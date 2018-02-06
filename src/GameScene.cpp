/*=================================================
//								FILE :GameScene.cpp
//		ファイル説明 :
//		ゲームシーンで利用する関数を記述する
//		ゲームシーンが管理するべきものを追加していく予定
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "GameScene.h"

DirectXData cGameScene::m_DirectXData;
DirectXDrawData cGameScene::m_TexDrawData;

cGameScene::cGameScene()
{
	//フォグの初期値
	m_Fog.vColor = { 0.7f,0.7f,0.7f,1.0f };
	m_Fog.vFactor = 2.0f;
	m_Fog.vShadowFlag = true;

	//被写界深度の初期値
	m_DoF.vBlur = 5.0f;
	m_DoF.vDoFRange = -10.3f;
	m_DoF.vScreenPos = { 0.5f,0.5f };

	m_Blur.RadialBlurFlag = false;
}

cGameScene::~cGameScene()
{
}

void cGameScene::SetDefaultRender()
{
	GetDirectX::InitOMSetRenderTargets();

	// RSにビューポートを設定
	GetDirectX::Context()->RSSetViewports(1, &m_DirectXData.v_ViewPort);
	// RSにラスタライザ・ステート・オブジェクトを設定
	GetDirectX::Context()->RSSetState(m_DirectXData.v_pCullNoneRasterizerState);

	// OMに描画ターゲット ビューと深度/ステンシル・ビューを設定
	GetDirectX::Context()->OMSetRenderTargets(1, &m_TexDrawData.v_pRenderTargetView, m_TexDrawData.v_pDepthStencilView);
	// OMにブレンド・ステート・オブジェクトを設定
	FLOAT BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	GetDirectX::Context()->OMSetBlendState(m_DirectXData.v_pBlendState, BlendFactor, 0xffffffff);
	// OMに深度/ステンシル・ステート・オブジェクトを設定
	GetDirectX::Context()->OMSetDepthStencilState(m_DirectXData.v_pDepthStencilState, 0);
}

void cGameScene::SetBlendState(BrendStateNo _OnFlag)
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	switch (_OnFlag)
	{
	case BrendStateNo::NONE :
		GetDirectX::Context()->OMSetBlendState(m_DirectXData.v_pBlendState, blendFactor, 0xffffffff);
		break;
	case BrendStateNo::ALPHA :
		GetDirectX::Context()->OMSetBlendState(m_DirectXData.v_pAlphaState, blendFactor, 0xffffffff);
		break;
	case BrendStateNo::ADD:
		GetDirectX::Context()->OMSetBlendState(m_DirectXData.v_pAddState, blendFactor, 0xffffffff);
		break;
	}
}

void cGameScene::SetFogColor(const DirectX::XMFLOAT4 & _Color)
{
	m_Fog.vColor = _Color;
}

void cGameScene::SetFogFactor(const float _f)
{
	m_Fog.vFactor = _f;
}

void cGameScene::SetShadowFlag(bool _Flag)
{
	m_Fog.vShadowFlag = _Flag;
}

void cGameScene::SetRasterizerState(CullMode mode)
{
	// OMにブレンド・ステート・オブジェクトを設定
	FLOAT BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	switch (mode)
	{
	case CullMode::NONE:
		GetDirectX::Context()->RSSetState(m_DirectXData.v_pCullNoneRasterizerState);
		break;
	case CullMode::LEFT:
		GetDirectX::Context()->RSSetState(m_DirectXData.v_pCullLeftRasterizerState);
		break;
	case CullMode::RIGHT:
		GetDirectX::Context()->RSSetState(m_DirectXData.v_pCullRightRasterizerState);
		break;
	case CullMode::WIRE:
		GetDirectX::Context()->RSSetState(m_DirectXData.v_pWIREFRAMERasterizerState);
		break;
	}
}

void cGameScene::SetRendBuffer()
{
	this->SetDefaultRender();
	//レンダリング先を画面に切り替え
	GetDirectX::Context()->OMSetRenderTargets(1, &m_DirectXData.v_pRenderTargetView, true ? m_DirectXData.v_pDepthStencilView : NULL);
}

void cGameScene::SetRadialBlur(DirectX::XMFLOAT3 _Pos, float _Delta, int _Num)
{
	m_Blur.vBlurPos = _Pos;
	m_Blur.vDelta = _Delta;
	m_Blur.vBlurNum = _Num;
	m_Blur.RadialBlurFlag = true;		//次描画せよ
}

void cGameScene::SetDepthStencilState()
{
	// OMに深度/ステンシル・ステート・オブジェクトを設定
	GetDirectX::Context()->OMSetDepthStencilState(m_DirectXData.v_pDepthStencilState, 0);
}

/// <summary>
/// 現在,
/// ラジアルブラーを照射するかどうか？
/// </summary>
bool cGameScene::IsRadialBlur()
{
	//放射回数があればtrue
	if (m_Blur.RadialBlurFlag){
		return true;
	}
	
	return false;
}

RadialBlurData cGameScene::GetRadialBlur(const DirectX::XMFLOAT4X4& _VP)
{
	//ラジアルブラーでぼかす位置を計算して渡す
	RadialBlurData data;
	data.vBlurNum = m_Blur.vBlurNum;
	data.vBlurPos = { 0,0 };
	data.vDelta = m_Blur.vDelta;
	//まずはぼかすワールド座標位置を射影座標に変換する
	DirectX::XMVECTOR WorldPos = DirectX::XMLoadFloat3(&m_Blur.vBlurPos);
	DirectX::XMVECTOR ViewPos = DirectX::XMVector3TransformCoord(WorldPos, DirectX::XMLoadFloat4x4(&_VP));
	DirectX::XMStoreFloat2(&data.vBlurPos, ViewPos);
	data.vBlurPos.x = (data.vBlurPos.x + 1.0f) / 2.0f;
	data.vBlurPos.y = ((-data.vBlurPos.y + 1.0f) / 2.0f);
	m_Blur.RadialBlurFlag = false;		//更新がなければ次は描画しない
	return data;
}

void cGameScene::SetDirectXData(DirectXData _data)
{
	m_DirectXData = _data;
}

/*=================================================
//								FILE :GameScene.cpp
//		�t�@�C������ :
//		�Q�[���V�[���ŗ��p����֐����L�q����
//		�Q�[���V�[�����Ǘ�����ׂ����̂�ǉ����Ă����\��
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "GameScene.h"

DirectXData cGameScene::m_DirectXData;
DirectXDrawData cGameScene::m_TexDrawData;

cGameScene::cGameScene()
{
	//�t�H�O�̏����l
	m_Fog.vColor = { 0.7f,0.7f,0.7f,1.0f };
	m_Fog.vFactor = 2.0f;
	m_Fog.vShadowFlag = true;

	//��ʊE�[�x�̏����l
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

	// RS�Ƀr���[�|�[�g��ݒ�
	GetDirectX::Context()->RSSetViewports(1, &m_DirectXData.v_ViewPort);
	// RS�Ƀ��X�^���C�U�E�X�e�[�g�E�I�u�W�F�N�g��ݒ�
	GetDirectX::Context()->RSSetState(m_DirectXData.v_pCullNoneRasterizerState);

	// OM�ɕ`��^�[�Q�b�g �r���[�Ɛ[�x/�X�e���V���E�r���[��ݒ�
	GetDirectX::Context()->OMSetRenderTargets(1, &m_TexDrawData.v_pRenderTargetView, m_TexDrawData.v_pDepthStencilView);
	// OM�Ƀu�����h�E�X�e�[�g�E�I�u�W�F�N�g��ݒ�
	FLOAT BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	GetDirectX::Context()->OMSetBlendState(m_DirectXData.v_pBlendState, BlendFactor, 0xffffffff);
	// OM�ɐ[�x/�X�e���V���E�X�e�[�g�E�I�u�W�F�N�g��ݒ�
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
	// OM�Ƀu�����h�E�X�e�[�g�E�I�u�W�F�N�g��ݒ�
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
	//�����_�����O�����ʂɐ؂�ւ�
	GetDirectX::Context()->OMSetRenderTargets(1, &m_DirectXData.v_pRenderTargetView, true ? m_DirectXData.v_pDepthStencilView : NULL);
}

void cGameScene::SetRadialBlur(DirectX::XMFLOAT3 _Pos, float _Delta, int _Num)
{
	m_Blur.vBlurPos = _Pos;
	m_Blur.vDelta = _Delta;
	m_Blur.vBlurNum = _Num;
	m_Blur.RadialBlurFlag = true;		//���`�悹��
}

void cGameScene::SetDepthStencilState()
{
	// OM�ɐ[�x/�X�e���V���E�X�e�[�g�E�I�u�W�F�N�g��ݒ�
	GetDirectX::Context()->OMSetDepthStencilState(m_DirectXData.v_pDepthStencilState, 0);
}

/// <summary>
/// ����,
/// ���W�A���u���[���Ǝ˂��邩�ǂ����H
/// </summary>
bool cGameScene::IsRadialBlur()
{
	//���ˉ񐔂������true
	if (m_Blur.RadialBlurFlag){
		return true;
	}
	
	return false;
}

RadialBlurData cGameScene::GetRadialBlur(const DirectX::XMFLOAT4X4& _VP)
{
	//���W�A���u���[�łڂ����ʒu���v�Z���ēn��
	RadialBlurData data;
	data.vBlurNum = m_Blur.vBlurNum;
	data.vBlurPos = { 0,0 };
	data.vDelta = m_Blur.vDelta;
	//�܂��͂ڂ������[���h���W�ʒu���ˉe���W�ɕϊ�����
	DirectX::XMVECTOR WorldPos = DirectX::XMLoadFloat3(&m_Blur.vBlurPos);
	DirectX::XMVECTOR ViewPos = DirectX::XMVector3TransformCoord(WorldPos, DirectX::XMLoadFloat4x4(&_VP));
	DirectX::XMStoreFloat2(&data.vBlurPos, ViewPos);
	data.vBlurPos.x = (data.vBlurPos.x + 1.0f) / 2.0f;
	data.vBlurPos.y = ((-data.vBlurPos.y + 1.0f) / 2.0f);
	m_Blur.RadialBlurFlag = false;		//�X�V���Ȃ���Ύ��͕`�悵�Ȃ�
	return data;
}

void cGameScene::SetDirectXData(DirectXData _data)
{
	m_DirectXData = _data;
}

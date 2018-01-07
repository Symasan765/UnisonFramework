/*=================================================
//								FILE : ShadowMap.cpp
//		�t�@�C������ :
//		�V���h�E�}�b�v����舵���N���X
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "ShadowMap.h"
#include "WinMain.h"
namespace {
	const int DepthX = SHADOW_MAP_WIDTH;
	const int DepthY = SHADOW_MAP_HEIGHT;
}

void cShadowMap::SetRender()
{
	m_Shader->Set();
	m_DepthTarget->SetRenderTargetTex();
}

ID3D11ShaderResourceView* cShadowMap::GetDepthResourceView()
{
	return m_DepthTarget->GetTextureResourceView();
}

cShadowMap::cShadowMap()
{
	//�������݃e�N�X�`���쐬
	m_DepthTarget = new cRenderTargetTex();
	m_DepthTarget->RTCreate(DepthX, DepthY, { 255,255,255 }, DXGI_FORMAT_R32_FLOAT);	//	�[�x�l����������R32��OK

	//�V�F�[�_�f�[�^���[�h
	m_Shader = SHADER::GetInstance()->LoadShaderFile("ShadowMap.hlsl", inPOSITION | inNORMAL | inTEX_UV | inBone | inWeight,true);
}

cShadowMap::~cShadowMap()
{
	delete m_DepthTarget;
	m_DepthTarget = nullptr;
}
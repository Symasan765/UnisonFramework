/*=================================================
//								FILE : ShadowMap.cpp
//		ファイル説明 :
//		シャドウマップを取り扱うクラス
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "ShadowMap.h"
#include "WinMain.h"
namespace {
	const int DepthX = SHADOW_MAP_WIDTH;
	const int DepthY = SHADOW_MAP_HEIGHT;
}

/// <summary>
/// 描画前にデータとともにセットする
/// </summary>
void cShadowMap::SetRender()
{
	m_Shader->Set();
	m_DepthTarget->SetRenderTargetTex();
}

/// <summary>
/// 深度マップを取得する
/// </summary>
/// <returns></returns>
ID3D11ShaderResourceView* cShadowMap::GetDepthResourceView()
{
	return m_DepthTarget->GetTextureResourceView();
}

/// <summary>
/// 初期化
/// </summary>
cShadowMap::cShadowMap()
{
	//書き込みテクスチャ作成
	m_DepthTarget = new cRenderTargetTex();
	m_DepthTarget->RTCreate(DepthX, DepthY, { 255,255,255 }, DXGI_FORMAT_R32_FLOAT);	//	深度値だけだからR32でOK

	//シェーダデータロード
	m_Shader = SHADER::GetInstance()->LoadShaderFile("ShadowMap.hlsl", inPOSITION | inNORMAL | inTEX_UV | inBone | inWeight,true);
}

/// <summary>
/// 解放処理
/// </summary>
cShadowMap::~cShadowMap()
{
	delete m_DepthTarget;
	m_DepthTarget = nullptr;
}
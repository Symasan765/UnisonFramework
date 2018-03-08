#include "SkyDome.h"
#include "WinMain.h"

/// <summary>
/// スカイドーム情報を初期化する
/// </summary>
cSkyDome::cSkyDome()
{
	m_SkyDome = new cDeferredModel;
	m_SkyDome->LoadData("ToonSkyDome.x", 1);
	m_SkyDome->Rotation(-90.0f, 0.0, 0.0f);
	m_SkyDome->Scaling(5.0f);
	m_RotAngle = 0.0f;

	//シェーダデータロード。IA情報などはすべてディファードレンダリング扱いでOK
	m_Shader = SHADER::GetInstance()->LoadShaderFile("DeferredSkyDraw.hlsl", inPOSITION | inNORMAL | inTEX_UV | inBone | inWeight, true);

	// 深度/ステンシル・ステート・オブジェクトの作成
	D3D11_DEPTH_STENCIL_DESC DepthStencil;
	DepthStencil.DepthEnable = false; // 深度テストあり
	DepthStencil.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // 書き込む
	DepthStencil.DepthFunc = D3D11_COMPARISON_LESS; // 手前の物体を描画
	DepthStencil.StencilEnable = FALSE; // ステンシル・テストなし
	DepthStencil.StencilReadMask = 0;     // ステンシル読み込みマスク。
	DepthStencil.StencilWriteMask = 0;     // ステンシル書き込みマスク。
										   // 面が表を向いている場合のステンシル・テストの設定
	DepthStencil.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;  // 維持
	DepthStencil.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;  // 維持
	DepthStencil.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;  // 維持
	DepthStencil.FrontFace.StencilFunc = D3D11_COMPARISON_NEVER; // 常に失敗
																 // 面が裏を向いている場合のステンシル・テストの設定
	DepthStencil.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;   // 維持
	DepthStencil.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;   // 維持
	DepthStencil.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;   // 維持
	DepthStencil.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS; // 常に成功
	HRESULT hr = GetDirectX::Device()->CreateDepthStencilState(&DepthStencil,
		&m_pDepthStencilState);
}

/// <summary>
/// 解放処理
/// </summary>
cSkyDome::~cSkyDome()
{
	delete m_SkyDome;
	SAFE_RELEASE(m_pDepthStencilState);
}

/// <summary>
/// スカイドームを描画する
/// </summary>
/// <param name="camera"></param>
void cSkyDome::DrawSkyDome(CameraData camera)
{
	m_SkyDome->SetTrans(camera.vPos);
	m_SkyDome->SetRotate(-90.0f, m_RotAngle,0.0f);
	m_RotAngle += 0.02f;
	m_Shader->Set();

	//スカイドームは必ず無限円でいいのでZテストをOFFにする
	GetDirectX::Context()->OMSetDepthStencilState(m_pDepthStencilState, 1);
	m_SkyDome->DrawMesh();
}
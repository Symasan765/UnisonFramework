/*=================================================
//								FILE : SceneManager.cpp
//		ファイル説明 :
//		ゲーム内でシーンを切り替えるためのゲームシーンマネージャの
//		処理内容を記述する。
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "FontDraw.h"
#include "SceneManager.h"
#include "DeferredRendering.h"
#include "DeferredConst.h"
#include "ShadowMap.h"
#include "SpriteDraw.h"
#include "MemoryMonitor.h"
#include "DeferredBaseModel.h"

cSceneManager::cSceneManager()
{
	//変数の初期化処理
	m_pOldScene = nullptr;
	m_pSwapChain = nullptr;
	m_pImmediateContext = nullptr;
	m_pRenderTargetView = nullptr;
	m_pDepthStencilView = nullptr;
	m_ScenePopFlag = false;
	m_pDefaultRnedTex = new cRenderTargetTex();
	m_pDefaultRnedTex->RTCreate((float)WINDOW_SIZE_X, (float)WINDOW_SIZE_Y, { 0,0,0 });
	m_SSSSS = new cScreenSpaceSSS;
	m_CrossFilter = new cCrossFilter;
	m_FXAA = new cFXAA;
	m_OutlineEmphasis = new cOutlineEmphasis;

#if defined(DEBUG) || defined(_DEBUG)
	if (FAILED(D3DX11CreateShaderResourceViewFromFileA(GetDirectX::Device(), "asset/Texture/Debug/CPURateBar.png", NULL, NULL, &m_pCPURateBar, NULL))) {
		MessageBox(0, "CPU利用率画像取得失敗", NULL, MB_OK);
	}
	if (FAILED(D3DX11CreateShaderResourceViewFromFileA(GetDirectX::Device(), "asset/Texture/Debug/MemoryRateBar.png", NULL, NULL, &m_pMemoryRateBar, NULL))) {
		MessageBox(0, "メモリー利用率画像取得失敗", NULL, MB_OK);
	}
	if (FAILED(D3DX11CreateShaderResourceViewFromFileA(GetDirectX::Device(), "asset/Texture/Debug/BarBase.png", NULL, NULL, &m_pBaseBar, NULL))) {
		MessageBox(0, "情報バー画像取得失敗", NULL, MB_OK);
	}
#endif
}

cSceneManager::~cSceneManager()
{
	SAFE_RELEASE(m_pCPURateBar);
	SAFE_RELEASE(m_pMemoryRateBar);
	SAFE_RELEASE(m_pBaseBar);
	delete m_pDefaultRnedTex;
	m_pDefaultRnedTex = nullptr;
	delete m_SSSSS;
	delete m_CrossFilter;
	delete m_FXAA;
	delete m_OutlineEmphasis;
}

/// <summary>
/// シーンのメイン更新処理を行う
/// シーンは更新途中で別のシーンへ行くこともあるので
/// 変わった場合は次の描画を飛ばすことになる。
/// </summary>
void cSceneManager::Update() {
#if defined(DEBUG) || defined(_DEBUG)
	m_TimeUpdate.TimerStart();		//デバッグモード中は処理時間を計測を行う
#endif

	//まずは更新処理
	m_pOldScene = m_pSceneData[0].get();		//更新処理中にシーン切り替えがある場合があるので現在のアドレスを保持する
	m_pSceneData[0]->Update();						//更新する時はもっとも先頭のものだけを更新する

#if defined(DEBUG) || defined(_DEBUG)
	m_TimeUpdate.TimerEnd();			//時間計測終了
#endif
}

/// <summary>
/// シーンの描画を行う。
/// 基本的にディファードレンダリングベースで作成する予定なので
/// 描画のフェーズをG-Buffer作成とその後のポストエフェクトとで分けている
/// </summary>
void cSceneManager::Draw(bool _DebugFlag, bool _FreeCameraFlag, bool _GBufferDrawFlag) {
	//Updateの中でシーンの更新があった場合は描画を飛ばす
	if (m_pSceneData[0].get() != m_pOldScene) return;

#if defined(DEBUG) || defined(_DEBUG)
	m_TimeDraw.TimerStart();		//デバッグモード中は処理時間を計測を行う
#endif

	DrawStart();		//描画スタート。ここで画面クリアされる=============================

	RenderingStandby(_FreeCameraFlag);

	RenderingShadowMap();

	DeferredRenderingPass();

	GBuffer GraphicBuffer = cDeferredRendering::GetInstance().GetGraphicBuffer();

	ID3D11ShaderResourceView * LastData = PostEffectPath(GraphicBuffer);

	BackBufferRendering(LastData);

	//=======================デバッグ情報の描画=================================
#if defined(DEBUG) || defined(_DEBUG)
	m_pSceneData[0]->SetRendBuffer();		//標準的なレンダリング設定をONにする
	cFontDraw::getInstance()->Draw();		//ゲームシーン中に設定された文字をまとめて描画する(2017/)
	if (_DebugFlag)
		ProcessRateDraw();
	if (_GBufferDrawFlag)
		GBufferDraw(cDeferredRendering::GetInstance().GetGraphicBuffer());	//G-Buffer表示

	m_TimeDraw.TimerEnd();			//垂直同期前に時間計測終了
#endif

	DrawEnd();			//描画終了。ここで画面が切り替わる================================
}

void cSceneManager::RenderingStandby(bool _FreeCameraFlag)
{
	//まずはカメラのプロジェクションを行う。プロジェクションのデータをもらっておく
	const ViewProj& ViewPorjData = m_pSceneData[0]->m_CameraData.Projection(_FreeCameraFlag);
	cDeferredConst::SetCameraData(m_pSceneData[0]->m_CameraData.GetCameraData(_FreeCameraFlag), ViewPorjData);
	cDeferredConst::SetLightData(m_pSceneData[0]->m_DirectionalLight.GetLightPos());
	m_pSceneData[0]->SetDefaultRender();		//標準的なレンダリング設定をONにする
	m_pSceneData[0]->m_DirectionalLight.SetConstantBuffer(3);
	m_pSceneData[0]->m_CameraData.SetConstBuffer(4, _FreeCameraFlag);
}

void cSceneManager::RenderingShadowMap()
{
	//深度マップを作成する
	cDeferredModel::DefaultRenderFlag(false);
	cShadowMap::GetInstance().SetRender();
	m_pSceneData[0]->MeshDraw();			//シャドウ用の深度マップが作られる
}

void cSceneManager::DeferredRenderingPass()
{
	//G-Bufferを作成する
	cDeferredModel::DefaultRenderFlag(true);
	cDeferredRendering::GetInstance().SetDeferredRendering(cShadowMap::GetInstance().GetDepthResourceView());		//シャドウマップ作成のため深度マップを渡す
	m_pSceneData[0]->MeshDraw();		//ここでG-Bufferが完成する
}

ID3D11ShaderResourceView * cSceneManager::PostEffectPath(GBuffer& GraphicBuffer)
{
	//肌部分にSSSSSを適用する
	m_SSSSS->DrawSSS(GraphicBuffer.vSSSSS, GraphicBuffer.vNormal, GraphicBuffer.vDiffuse);

	//輪郭線を強調する
	m_OutlineEmphasis->DrawOutLine(m_SSSSS->GetResourceView(), GraphicBuffer.vNormal);

	//アンチエイリアス処理を行う
	m_FXAA->DrawFXAA(m_OutlineEmphasis->GetResourceView());

	//今後ポストエフェクトの最終パスが変わればここで返却するテクスチャを変更する
	return m_FXAA->GetResourceView();
}

void cSceneManager::BackBufferRendering(ID3D11ShaderResourceView * LastData)
{
	m_pSceneData[0]->SetRendBuffer();
	m_pSceneData[0]->SetBlendState(BrendStateNo::NONE);

	//画面へレンダリングする
	cSprite2DDraw::GetInstance().Draw(LastData, { 0.0f,0.0f }, { (float)WINDOW_SIZE_X,(float)WINDOW_SIZE_Y });
}

void cSceneManager::DrawStart() {
	// 描画ターゲットのクリア
	//const float ClearColor[4] = { 0.1f, 0.2f, 0.6f, 1.0f };
	const float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_pImmediateContext->ClearRenderTargetView(
		m_pRenderTargetView, // クリアする描画ターゲット
		ClearColor);         // クリアする値

							 // 深度/ステンシルのクリア
	m_pImmediateContext->ClearDepthStencilView(
		m_pDepthStencilView, // クリアする深度/ステンシル・ビュー
		D3D11_CLEAR_DEPTH,   // 深度値だけをクリアする
		1.0f,                // 深度バッファをクリアする値
		0);                  // ステンシル・バッファをクリアする値(この場合、無関係)

	m_pDefaultRnedTex->SetRenderTargetTex();
}

void cSceneManager::DrawEnd() {
	m_pSwapChain->Present(1,	// 垂直同期で更新する
		0);	// 画面を実際に更新する
}

void cSceneManager::PopCheck()
{
	//最後にpopされているかを確認し、されていればPOP
	if (m_ScenePopFlag) {
		m_pSceneData.pop_front();	//ここでPOP！
		m_ScenePopFlag = false;		//POPが完了すればもとに戻す
	}
}

/// <summary>
/// フリーカメラのアップデートを行う
/// </summary>
void cSceneManager::FreeCameraUpdate()
{
	m_pSceneData[0]->m_CameraData.FreeCameraOperation();
}

/// <summary>
/// G-Bufferを表示させる関数
/// </summary>
/// <param name="_GBuffer"></param>
void cSceneManager::GBufferDraw(GBuffer & _GBuffer)
{
	//画像一枚分のサイズ
	const float offsetX = (float)WINDOW_SIZE_X / 8.0f;
	const float offsetY = (float)WINDOW_SIZE_Y / 8.0f;

	const float startX = 0.0f;												//一番左から描画
	const float startY = WINDOW_SIZE_Y - offsetY * 3.0f;	//一番下のピクセルから画像三枚目の位置

	//2017/12/22時点でG-Bufferは5枚で構成される
	cSprite2DDraw::GetInstance().Draw(_GBuffer.vDiffuse, { startX,startY + offsetY * 0 }, { offsetX,offsetY });
	cSprite2DDraw::GetInstance().Draw(_GBuffer.vPosition, { startX,startY + offsetY * 1 }, { offsetX,offsetY });
	cSprite2DDraw::GetInstance().Draw(_GBuffer.vNormal, { startX,startY + offsetY * 2 }, { offsetX,offsetY });

	cSprite2DDraw::GetInstance().Draw(_GBuffer.vHighBrightness, { offsetX,startY + offsetY * 0 }, { offsetX,offsetY });
	cSprite2DDraw::GetInstance().Draw(_GBuffer.vSSSSS, { offsetX,startY + offsetY * 1 }, { offsetX,offsetY });

	cSprite2DDraw::GetInstance().Draw(cShadowMap::GetInstance().GetDepthResourceView(), { offsetX * 2,startY + offsetY * 0 }, { offsetY * 3,offsetY * 3 });
}

void cSceneManager::SetProcessRate(int _No, int _Rate)
{
	m_ProcessRate[_No] = _Rate;
}

void cSceneManager::SetProcessNum(int _Num)
{
	m_ProcessNum = _Num;
}

void cSceneManager::ProcessRateDraw()
{
	for (int i = 0; i < m_ProcessNum; i++) {
		float MaxLen = 50.0f;
		MaxLen *= (m_ProcessRate[i] / 100.0f);
		cSprite2DDraw::GetInstance().Draw(m_pCPURateBar, { 80.0f,238.0f + (i * 15.0f) }, { MaxLen,10.0f });
	}

	//メモリー情報
	MemoryData Memory = cMemoryMonitor::GetInstance().GetMemoryData();
	float Rate = ((float)Memory.vUsed / (float)Memory.vSize);	//メモリーの使用率を表示
	float MaxLen = 200.0f;	//最大長
	float Len = MaxLen * Rate;
	float pos = 265 + (m_ProcessNum * 15) + 8;
	cSprite2DDraw::GetInstance().Draw(m_pBaseBar, { 15.0f,pos }, { MaxLen,20.0f });
	cSprite2DDraw::GetInstance().Draw(m_pMemoryRateBar, { 15.0f,pos }, { Len,20.0f });
}

//シーンを削除する
void cSceneManager::Destroy()
{
	m_pSceneData.erase(m_pSceneData.begin(), m_pSceneData.end());		//シーンをすべて削除
}

void Scene::Pop()
{
	cSceneManager::GetInstance().PopScene();
}

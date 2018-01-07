/*=================================================
//								FILE : DirectX11.cpp
//		ファイル説明 :
//		システムの初期化や設定、開放の記述を行う。
//		WinMainの隠蔽化を主な目的とする。
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "FontDraw.h"
#include "DirectX11.h"
#include "WindProc.h"
#include "SceneManager.h"
#include "MainScene.h"
#include "TextureManager.h"
#include "Input.h"
#include "Mesh3D.h"
#include "SpriteDraw.h"
#include "MemoryMonitor.h"
#include "ComboBox.h"
#include "Tessellation.h"
#include "ShaderMonitoring.h"

#include "PostEffects.h"

cDirectX11* cDirectX11::m_pDX11 = nullptr;

//他のファイルでは使用しない
namespace {
	MSG msg;				//メッセージ構造体
#if defined(DEBUG) || defined(_DEBUG)
	CHAR		g_szAppTitle[] = "UnisonFramework(F5:Debug)";
#else
	CHAR		g_szAppTitle[] = "UnisonFramework";
#endif
	CHAR		g_szWndClass[] = "UnisonFramework";

	D3D_FEATURE_LEVEL g_pFeatureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };
	UINT              g_FeatureLevels = 3;   // 配列の要素数
	D3D_FEATURE_LEVEL g_FeatureLevelsSupported; // デバイス作成時に返される機能レベル
}

/// <summary>
/// ここでシーンマネージャによるゲームの更新を行う
/// </summary>
void cDirectX11::MainGameUpdate()
{
	//==========================システムの更新部分====================================

	//リソースが解放されたかのチェックをして必要なければ解放処理を行う。
	cTexManager::GetInstance().CheckDelete();
	MeshManager::GetInstance().CheckDelete();
	SHADER::GetInstance()->CheckDelete();
	cTessellationManager::GetInstance().CheckDelete();

	//入力の更新部分処理
	GetKey->UpdateKeyboard();
	GetGamePad->UpdateGamePad();

	cSprite2DDraw::GetInstance().m_NowDepth = 0.99999f;		//スプライト描画を最背面に設定する
#if defined(DEBUG) || defined(_DEBUG)
	cShaderMonitoring::GetInstance().MonitorChanges();		//シェーダファイルの更新がされたかどうかを判定する
	cMemoryMonitor::GetInstance().Update();						//デバッグ中はメモリー利用状況を更新する
#endif

	//============================================================================
	//==========================ゲームの更新部分======================================

#if defined(DEBUG) || defined(_DEBUG)
	m_TimeAllProcess.TimerStart();			//デバッグモード中は処理時間を計測する
#endif

	//アップデート出来るかチェックする
	if (GameUpdateFlag())
		cSceneManager::GetInstance().Update();
	//フリーカメラ操作フラグが立っていればフリーカメラのアップデートを行う
	if (m_FreeCameraFlag)
		cSceneManager::GetInstance().FreeCameraUpdate();
	cSceneManager::GetInstance().Draw(m_DebugFlag, m_FreeCameraFlag,m_GBufferDrawFlag);
	cSceneManager::GetInstance().PopCheck();							//最後にループ中に遷移のPOPがあったかを確認して実行する

#if defined(DEBUG) || defined(_DEBUG)
	m_TimeAllProcess.TimerEnd();			//時間計測終了
	GameProcessingTime();						//計測した時間をまとめる
#endif

	//=============================================================================
}

/// <summary>
/// ゲームの更新処理を行ってもいいかチェックする関数
/// </summary>
/// <returns></returns>
bool cDirectX11::GameUpdateFlag() const
{
#if defined(DEBUG) || defined(_DEBUG)
	//デバッグ中
	if (!m_DebugFlag) return true;		//デバッグ中ではないならそのまま実行していい
	if (!m_PauseFlag) return true;		//ポーズしてないならそのまま実行
	if (GetKey->Trigger(DIK_F3)) return true;		//ポーズ中でもF3が押された瞬間は実行していい

	return false;
#else
	//リリース中
	return true;		//リリースモードではゲームを止めることがない
#endif
}

/// <summary>
/// ゲームループで測定した計測時間をまとめる
/// </summary>
void cDirectX11::GameProcessingTime()
{
	//デバッグモード中しか利用しない
#if defined(DEBUG) || defined(_DEBUG)
	//変数の宣言
	static unsigned TimeCnt;	//フレームのカウント
	static float AllTime;		//1フレームにかかった時間
	static float UpdateTime;	//更新にかかった時間
	static float DrawTime;			//描画にかかった時間
	static float Estimated;		//1フレームにかかる想定時間
	static int UpdateRate;		//更新にかかった比率(パーセント)
	static int DrawRate;				//描画にかかった比率(パーセント)
	static int AllRate;				//想定するフレームレートに対してかかった時間率(パーセント)
	static int ProcTime;	//単位をミリに変換
	static CameraData Camera;	//カメラ情報
	static int ProcessRate[MAX_PROCESSORS_COUNT];
	static float AllMemory;
	static float UseMemory;
	//まずは現在デバッグモードになっているかチェックする
	if (m_DebugFlag) {
		m_pComboBox->display();
		//1フレーム内で4回処理する
		if (TimeCnt % (FRAME_RATE / 4) == 0) {
			AllTime = m_TimeAllProcess.GetProcessingTime() * 1000.0f;		//1フレームにかかった時間
			UpdateTime = cSceneManager::GetInstance().m_TimeUpdate.GetProcessingTime() * 1000.0f;	//更新にかかった時間
			DrawTime = cSceneManager::GetInstance().m_TimeDraw.GetProcessingTime() * 1000.0f;			//描画にかかった時間
			Estimated = 1.0f / (float)FRAME_RATE;		//1フレームにかかる想定時間

			UpdateRate = (int)(UpdateTime / AllTime * 100.0f);		//更新にかかった比率(パーセント)
			DrawRate = (int)(DrawTime / AllTime * 100.0f);				//描画にかかった比率(パーセント)
			AllRate = (int)(AllTime / Estimated * 100.0f);				//想定するフレームレートに対してかかった時間率(パーセント)

			ProcTime = 1000.0f / (AllTime);	//単位をフレームレートに変換

			Camera = cSceneManager::GetInstance().m_pSceneData[0]->m_CameraData.GetCameraData(m_PauseFlag);		//カメラの情報を取得する
			int ProcessNum = m_ProcessMonitor.GetProcessorsNum();	//プロセッサ数
			for (int i = 0; i < ProcessNum + 1; i++) {
				ProcessRate[i] = m_ProcessMonitor.GetProcessRate(i);	//CPU使用率を取得
				cSceneManager::GetInstance().SetProcessRate(i, ProcessRate[i]);
			}
			//メモリの使用量を取得する
			MemoryData Memory = cMemoryMonitor::GetInstance().GetMemoryData();
			int _AllMemory = Memory.vSize / 1024.0f / 1024.0f;	//容量をMBに変換
			int _UseMemory = Memory.vUsed / 1024.0f / 1024.0f;
			AllMemory = (float)_AllMemory / 1024.0f;				//単位をGBに変換
			UseMemory = (float)_UseMemory / 1024.0f;
		}

			//デバッグモード中共通で表示するエリア
		cFontDraw::getInstance()->TextDraw("デバッグモード中", 10, 10, FontName::MSGothic, 1.5f);
		std::string ProcTimeChara = "全処理時間 : " + std::to_string(AllTime) + "ms(" + std::to_string(ProcTime) + "fps)";
		std::string ProcUpdateChara = "・更新時間 : " + std::to_string(UpdateTime) + "ms(" + std::to_string(UpdateRate) + "%)";
		std::string ProcDrawChara = "・描画時間 : " + std::to_string(DrawTime) + "ms(" + std::to_string(DrawRate) + "%)";
		//上記の処理時間を表示させる
		cFontDraw::getInstance()->TextDraw(ProcTimeChara, 15, 55,FontName::Mplus,0.6f);
		cFontDraw::getInstance()->TextDraw(ProcUpdateChara, 15, 75, FontName::Mplus, 0.6f);
		cFontDraw::getInstance()->TextDraw(ProcDrawChara, 15, 95, FontName::Mplus, 0.6f);

		//カメラ情報をまとめる
		std::string CameraPos = "・座標 : X" + std::to_string(Camera.vPos.x) + " Y" + std::to_string(Camera.vPos.y) + " Z" + std::to_string(Camera.vPos.z);
		std::string CameravLook = "・注視 : X" + std::to_string(Camera.vLook.x) + " Y" + std::to_string(Camera.vLook.y) + " Z" + std::to_string(Camera.vLook.z);
		std::string CameraNearFar = "・範囲 : " + std::to_string(Camera.vNear) + " ～ " + std::to_string(Camera.vFar);
		//カメラの情報を表示する
		cFontDraw::getInstance()->TextDraw("カメラ情報", 15, 125, FontName::Mplus, 0.6f);
		cFontDraw::getInstance()->TextDraw(CameraPos, 15, 145, FontName::Mplus, 0.4f);
		cFontDraw::getInstance()->TextDraw(CameravLook, 15, 160, FontName::Mplus, 0.4f);
		cFontDraw::getInstance()->TextDraw(CameraNearFar, 15, 175, FontName::Mplus, 0.4f);
		//CPU使用率を表示する
		int ProcessNum = m_ProcessMonitor.GetProcessorsNum();	//プロセッサ数
		std::string ProcessChara = "・プロセッサ数 : " + std::to_string(ProcessNum);
		cFontDraw::getInstance()->TextDraw("CPU使用状況", 15, 205, FontName::Mplus, 0.6f);
		cFontDraw::getInstance()->TextDraw(ProcessChara, 15, 225, FontName::Mplus, 0.4f);
		std::string ProcessRateChara;
		for (int i = 0; i < ProcessNum + 1; i++) {
			if (i == ProcessNum) {
				ProcessRateChara = "AVL : ";
			}
			else {
				ProcessRateChara = "No" + std::to_string(i) + " : ";
			}
			ProcessRateChara += std::to_string(ProcessRate[i]) + "%";
			cFontDraw::getInstance()->TextDraw(ProcessRateChara, 15, 245 + (i * 15), FontName::Mplus, 0.4f);
		}
		//メモリ使用状況
		int InitPos = (ProcessNum) * 15;
		std::string AllMemoryChara =std::to_string(UseMemory) + "GB/" + std::to_string(AllMemory) + "GB";
		cFontDraw::getInstance()->TextDraw("メモリ使用量", 15, 245 + (InitPos) + 20, FontName::Mplus, 0.6f);
		cFontDraw::getInstance()->TextDraw(AllMemoryChara, 25, 265 + (InitPos)+20, FontName::Mplus, 0.4f);

		//終了表示を出す
		cFontDraw::getInstance()->TextDraw("F5:デバッグモードOFF", 940, 700);
		cFontDraw::getInstance()->TextDraw("F4:ポーズ機能ON・OFF", 940, 660);

		//G-Buffer表示中にUI
		if (!m_GBufferDrawFlag) {
			cFontDraw::getInstance()->TextDraw("F6:G-Buffer表示", 10, 710, FontName::MSGothic, 0.7f);
		}

		//ポーズモード中のみに表示するエリア
		if (m_PauseFlag) {
			cFontDraw::getInstance()->TextDraw("(ポーズモード)", 400, 10, FontName::MSGothic, 1.5f);
			cFontDraw::getInstance()->TextDraw("フリーカメラ操作", 1000, 10);
			cFontDraw::getInstance()->TextDraw("WASD : 移動", 1115, 40, FontName::MSGothic, 0.8f);
			cFontDraw::getInstance()->TextDraw("十字 : 回転", 1115, 65, FontName::MSGothic, 0.8f);
			cFontDraw::getInstance()->TextDraw("QZEC : 範囲", 1115, 90, FontName::MSGothic, 0.8f);
			cFontDraw::getInstance()->TextDraw("ENTER: 戻す", 1115, 115, FontName::MSGothic, 0.8f);
			cFontDraw::getInstance()->TextDraw("F3:コマ送り", 1115, 140, FontName::MSGothic, 0.8f);
		}
		TimeCnt++;
		//カウンターを初期化する
		if (TimeCnt == FRAME_RATE)
			TimeCnt = 0;
	}
	else {
		m_pComboBox->Hide();	//スクリプト名コンボボックス非表示
	}
#endif
}

/// <summary>
/// 初期化だけ行う
/// </summary>
cDirectX11::cDirectX11() {
	m_sizeWindow = { WINDOW_SIZE_X, WINDOW_SIZE_Y };		//ウィンドウサイズ
	m_hWindow = nullptr;
	m_hInstance = nullptr;
	m_pD3DDevice = nullptr;
	m_pDepthStencilState = nullptr;
	m_pImmediateContext = nullptr;
	m_pSwapChain = nullptr;
	m_pRenderTargetView = nullptr;
	m_pDepthStencilView = nullptr;
	m_pCullNoneRasterizerState = nullptr;
	m_pCullLeftRasterizerState = nullptr;
	m_pCullRightRasterizerState = nullptr;
	m_pBlendState = nullptr;
	v_pAlphaState = nullptr;

	//デバッグフラグたち
	m_DebugFlag = false;
	m_PauseFlag = false;
	m_FreeCameraFlag = false;
	m_GBufferDrawFlag = false;
	m_pComboBox = nullptr;
}

/// <summary>
/// シングルトンのポインタに実態を作成し、システムの各種初期化を行っていく
/// </summary>
void cDirectX11::Init()
{
	if (!m_pDX11) {
		// デバッグ ヒープ マネージャによるメモリ割り当ての追跡方法を設定
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		m_pDX11 = new cDirectX11;
	}
}

/// <summary>
/// ここがゲームループが格納されている関数
/// </summary>
void cDirectX11::Update()
{
	do
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//ここにメイン処理
			MainGameUpdate();
		}
	} while (msg.message != WM_QUIT);		//なにかあればそのまま処理を抜ける
}

/// <summary>
/// ウィンドウの作成、DirectXの初期化を行う
/// </summary>
/// <param name="_hInst">WinMainで利用されたHINSTANCEをそのまま渡す</param>
/// <returns></returns>
HRESULT cDirectX11::Create(HINSTANCE _hInst)
{
	//まずはウィンドウの生成から行う
	if (FAILED(InitWindow(_hInst))) {
		_ASSERT(true, "WinMain InitWindow() err");
		return E_FAIL;
	}

	//次にDirectX初期化
	if (FAILED(InitDirect3D())) {
		_ASSERT(true, "WinMain InitDirect3D() err");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT cDirectX11::InitWindow(HINSTANCE _hInst)
{
	// アプリケーションのインスタンス ハンドルを保存
	m_hInstance = _hInst;

	// ウインドウ クラスの登録
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = _hInst;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_szWndClass;

	if (!RegisterClass(&wc))
		return DXTRACE_ERR("InitApp", GetLastError());

	// メイン ウインドウ作成
	RECT rect;
	rect.top = 0;
	rect.left = 0;
	rect.right = m_sizeWindow.cx;
	rect.bottom = m_sizeWindow.cy;
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, TRUE);

	m_hWindow = CreateWindow(g_szWndClass, g_szAppTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top,
		NULL, NULL, _hInst, NULL);
	if (m_hWindow == NULL)
		return DXTRACE_ERR("InitApp", GetLastError());

	// ウインドウ表示
	ShowWindow(m_hWindow, SW_SHOWNORMAL);
	UpdateWindow(m_hWindow);

	return S_OK;
}

/// <summary>
/// ゲームで利用する機能の初期化を行う
/// </summary>
/// <returns></returns>
bool cDirectX11::InitApp()
{
	//利用する機能が増えれば記述する

	//シーンマネージャの初期化
	InitSceneManager();

	//シーンにデバイス情報を渡しておく
	DirectXData data{
	m_pD3DDevice,									//デバイスハンドル
	m_pDepthStencilState,
	m_pImmediateContext,			// デバイス・コンテキスト
	m_pSwapChain,								 // スワップ・チェイン
	m_pRenderTargetView,	//ターゲットビュー
	m_pDepthStencil,						//深度・ステンシル
	m_pDepthStencilView,		//深度ステンシルビュー
	m_ViewPort,									//ビューポート
	m_pCullNoneRasterizerState,		//ラスタライザ
	m_pCullLeftRasterizerState,
	m_pCullRightRasterizerState,
	m_pWIREFRAMERasterizerState,
	m_pBlendState,							//デフォルトブレンドステート
	v_pAlphaState,
	v_pAddState
	};
	cGameScene::SetDirectXData(data);
	cGameScene::m_TexDrawData = cSceneManager::GetInstance().m_pDefaultRnedTex->GetDrawData();

#if defined(DEBUG) || defined(_DEBUG)
	cSceneManager::GetInstance().SetProcessNum(m_ProcessMonitor.GetProcessorsNum() + 1);
	for (int i = 0; i < m_ProcessMonitor.GetProcessorsNum() + 1; i++) {
		cSceneManager::GetInstance().SetProcessRate(i, m_ProcessMonitor.GetProcessRate(i));
	}
	cMemoryMonitor::GetInstance();	//メモリの初期値を取得する
#endif

	return true;
}

void cDirectX11::InitSceneManager()
{
	cSceneManager::GetInstance().m_pDepthStencilView = m_pDepthStencilView;
	cSceneManager::GetInstance().m_pImmediateContext = m_pImmediateContext;
	cSceneManager::GetInstance().m_pRenderTargetView = m_pRenderTargetView;
	cSceneManager::GetInstance().m_pSwapChain = m_pSwapChain;
	Scene::Push<cMainScene>();
}

/// <summary>
/// DirectXの初期化を担当する関数
/// </summary>
/// <returns></returns>
HRESULT cDirectX11::InitDirect3D()
{
	// ウインドウのクライアント エリア
	RECT rc;
	GetClientRect(m_hWindow, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	// デバイスとスワップ チェインの作成
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));	// 構造体の値を初期化
	sd.BufferCount = 3;		// バック バッファ数
	sd.BufferDesc.Width = width;	// バック バッファの幅
	sd.BufferDesc.Height = height;	// バック バッファの高さ
									//sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // フォーマット
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // フォーマット
	sd.BufferDesc.RefreshRate.Numerator = 60;  // リフレッシュ レート(分子)
	sd.BufferDesc.RefreshRate.Denominator = 1; // リフレッシュ レート(分母)
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;	// スキャンライン
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;			// スケーリング
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // バック バッファの使用法
	sd.OutputWindow = m_hWindow;	// 関連付けるウインドウ
	sd.SampleDesc.Count = 1;		// マルチ サンプルの数
	sd.SampleDesc.Quality = 0;		// マルチ サンプルのクオリティ
	sd.Windowed = TRUE;				// ウインドウ モード
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // モード自動切り替え

#if defined(DEBUG) || defined(_DEBUG)
	UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#else
	UINT createDeviceFlags = 0;
#endif

	// ハードウェア・デバイスを作成
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags,
		g_pFeatureLevels, g_FeatureLevels, D3D11_SDK_VERSION, &sd,
		&m_pSwapChain, &m_pD3DDevice, &g_FeatureLevelsSupported,
		&m_pImmediateContext);
	if (FAILED(hr)) {
		// WARPデバイスを作成
		hr = D3D11CreateDeviceAndSwapChain(
			NULL, D3D_DRIVER_TYPE_WARP, NULL, createDeviceFlags,
			g_pFeatureLevels, g_FeatureLevels, D3D11_SDK_VERSION, &sd,
			&m_pSwapChain, &m_pD3DDevice, &g_FeatureLevelsSupported,
			&m_pImmediateContext);
		if (FAILED(hr)) {
			// リファレンス・デバイスを作成
			hr = D3D11CreateDeviceAndSwapChain(
				NULL, D3D_DRIVER_TYPE_REFERENCE, NULL, createDeviceFlags,
				g_pFeatureLevels, g_FeatureLevels, D3D11_SDK_VERSION, &sd,
				&m_pSwapChain, &m_pD3DDevice, &g_FeatureLevelsSupported,
				&m_pImmediateContext);
			if (FAILED(hr)) {
				return DXTRACE_ERR("InitDirect3D D3D11CreateDeviceAndSwapChain", hr);
			}
		}
	}

	// **********************************************************
	// ブレンド・ステート・オブジェクトの作成
	D3D11_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
	BlendState.AlphaToCoverageEnable = TRUE;
	BlendState.IndependentBlendEnable = FALSE;
	BlendState.RenderTarget[0].BlendEnable = FALSE;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = m_pD3DDevice->CreateBlendState(&BlendState, &m_pBlendState);
	if (FAILED(hr))
		return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateBlendState", hr);

	D3D11_BLEND_DESC BlendDesc;
	ZeroMemory(&BlendDesc, sizeof(BlendDesc));
	BlendDesc.AlphaToCoverageEnable = TRUE;
	BlendDesc.IndependentBlendEnable = FALSE;
	BlendDesc.RenderTarget[0].BlendEnable = TRUE;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;			//ONE
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_DEST_COLOR;		//ONE
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;		//ONE
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	GetDirectX::Device()->CreateBlendState(&BlendDesc, &v_pAlphaState);

	D3D11_BLEND_DESC AddDesc;
	ZeroMemory(&AddDesc, sizeof(AddDesc));
	AddDesc.AlphaToCoverageEnable = TRUE;
	AddDesc.IndependentBlendEnable = FALSE;
	AddDesc.RenderTarget[0].BlendEnable = TRUE;
	AddDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;			//ONE
	AddDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;		//ONE
	AddDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	AddDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	AddDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;		//ONE
	AddDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	AddDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	GetDirectX::Device()->CreateBlendState(&AddDesc, &v_pAddState);

	// ラスタライザ・ステート・オブジェクトの作成
	D3D11_RASTERIZER_DESC RSDesc;
	RSDesc.FillMode = D3D11_FILL_SOLID;   // 普通に描画する
	RSDesc.CullMode = D3D11_CULL_NONE;    // 両面を描画する
	RSDesc.FrontCounterClockwise = FALSE; // 時計回りが表面
	RSDesc.DepthBias = 0;
	RSDesc.DepthBiasClamp = 0;
	RSDesc.SlopeScaledDepthBias = 0;
	RSDesc.DepthClipEnable = TRUE;
	RSDesc.ScissorEnable = FALSE;
	RSDesc.MultisampleEnable = FALSE;
	RSDesc.AntialiasedLineEnable = FALSE;
	hr = m_pD3DDevice->CreateRasterizerState(&RSDesc, &m_pCullNoneRasterizerState);
	if (FAILED(hr))
		return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateRasterizerState", hr);

	RSDesc.CullMode = D3D11_CULL_FRONT;    // 左手座標上の表を描画する
	hr = m_pD3DDevice->CreateRasterizerState(&RSDesc, &m_pCullLeftRasterizerState);
	if (FAILED(hr))
		return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateRasterizerState", hr);

	RSDesc.CullMode = D3D11_CULL_BACK;    // 左手座標上の裏を描画する
	hr = m_pD3DDevice->CreateRasterizerState(&RSDesc, &m_pCullRightRasterizerState);
	if (FAILED(hr))
		return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateRasterizerState", hr);

	D3D11_RASTERIZER_DESC wRSDesc;
	wRSDesc.CullMode = D3D11_CULL_NONE;    // 両面を描画する
	wRSDesc.FillMode = D3D11_FILL_WIREFRAME;		//ワイヤーフレーム
	wRSDesc.FrontCounterClockwise = FALSE; // 時計回りが表面
	wRSDesc.DepthBias = 0;
	wRSDesc.DepthBiasClamp = 0;
	wRSDesc.SlopeScaledDepthBias = 0;
	wRSDesc.DepthClipEnable = TRUE;
	wRSDesc.ScissorEnable = FALSE;
	wRSDesc.MultisampleEnable = FALSE;
	wRSDesc.AntialiasedLineEnable = FALSE;
	hr = m_pD3DDevice->CreateRasterizerState(&wRSDesc, &m_pWIREFRAMERasterizerState);
	if (FAILED(hr))
		return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateRasterizerState", hr);

	// **********************************************************
	// 深度/ステンシル・ステート・オブジェクトの作成
	D3D11_DEPTH_STENCIL_DESC DepthStencil;
	DepthStencil.DepthEnable = TRUE; // 深度テストあり
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
	hr = m_pD3DDevice->CreateDepthStencilState(&DepthStencil,
		&m_pDepthStencilState);
	if (FAILED(hr))
		return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateDepthStencilState", hr);

	// **********************************************************
	// バック バッファの初期化
	hr = InitBackBuffer();
	if (FAILED(hr))
		return DXTRACE_ERR("InitDirect3D InitBackBuffer", hr);

	return hr;
}

/// <summary>
/// 描画用のバックバッファの初期化を行う
/// </summary>
/// <returns></returns>
HRESULT cDirectX11::InitBackBuffer()
{
	HRESULT hr;

	// スワップ・チェインから最初のバック・バッファを取得する
	ID3D11Texture2D *pBackBuffer;  // バッファのアクセスに使うインターフェイス
	hr = m_pSwapChain->GetBuffer(
		0,                         // バック・バッファの番号
		__uuidof(ID3D11Texture2D), // バッファにアクセスするインターフェイス
		(LPVOID*)&pBackBuffer);    // バッファを受け取る変数
	if (FAILED(hr))
		return DXTRACE_ERR("InitBackBuffer g_pSwapChain->GetBuffer", hr);  // 失敗

																		   // バック・バッファの情報
	D3D11_TEXTURE2D_DESC descBackBuffer;
	pBackBuffer->GetDesc(&descBackBuffer);

	// バック・バッファの描画ターゲット・ビューを作る
	hr = m_pD3DDevice->CreateRenderTargetView(
		pBackBuffer,           // ビューでアクセスするリソース
		NULL,                  // 描画ターゲット・ビューの定義
		&m_pRenderTargetView); // 描画ターゲット・ビューを受け取る変数
	SAFE_RELEASE(pBackBuffer);  // 以降、バック・バッファは直接使わないので解放
	if (FAILED(hr))
		return DXTRACE_ERR("InitBackBuffer g_pD3DDevice->CreateRenderTargetView", hr);  // 失敗

																						// 深度/ステンシル・テクスチャの作成
	D3D11_TEXTURE2D_DESC descDepth = descBackBuffer;
	//	descDepth.Width     = descBackBuffer.Width;   // 幅
	//	descDepth.Height    = descBackBuffer.Height;  // 高さ
	descDepth.MipLevels = 1;       // ミップマップ・レベル数
	descDepth.ArraySize = 1;       // 配列サイズ
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;  // フォーマット(深度のみ)
											   //	descDepth.SampleDesc.Count   = 1;  // マルチサンプリングの設定
											   //	descDepth.SampleDesc.Quality = 0;  // マルチサンプリングの品質
	descDepth.Usage = D3D11_USAGE_DEFAULT;      // デフォルト使用法
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL; // 深度/ステンシルとして使用
	descDepth.CPUAccessFlags = 0;   // CPUからはアクセスしない
	descDepth.MiscFlags = 0;   // その他の設定なし
	hr = m_pD3DDevice->CreateTexture2D(
		&descDepth,         // 作成する2Dテクスチャの設定
		NULL,               // 
		&m_pDepthStencil);  // 作成したテクスチャを受け取る変数
	if (FAILED(hr))
		return DXTRACE_ERR("InitBackBuffer g_pD3DDevice->CreateTexture2D", hr);  // 失敗

																				 // 深度/ステンシル ビューの作成
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = descDepth.Format;            // ビューのフォーマット
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Flags = 0;
	descDSV.Texture2D.MipSlice = 0;
	hr = m_pD3DDevice->CreateDepthStencilView(
		m_pDepthStencil,       // 深度/ステンシル・ビューを作るテクスチャ
		&descDSV,              // 深度/ステンシル・ビューの設定
		&m_pDepthStencilView); // 作成したビューを受け取る変数
	if (FAILED(hr))
		return DXTRACE_ERR("InitBackBuffer g_pD3DDevice->CreateDepthStencilView", hr);  // 失敗

																						// ビューポートの設定
	m_ViewPort.TopLeftX = 0.0f;    // ビューポート領域の左上X座標。
	m_ViewPort.TopLeftY = 0.0f;    // ビューポート領域の左上Y座標。
	m_ViewPort.Width = (FLOAT)descBackBuffer.Width;   // ビューポート領域の幅
	m_ViewPort.Height = (FLOAT)descBackBuffer.Height;  // ビューポート領域の高さ
	m_ViewPort.MinDepth = 0.0f; // ビューポート領域の深度値の最小値
	m_ViewPort.MaxDepth = 1.0f; // ビューポート領域の深度値の最大値

								//サイズを保存
	m_sizeWindow.cx = descBackBuffer.Width;
	m_sizeWindow.cy = descBackBuffer.Height;

	return S_OK;
}

/// <summary>
/// DirectXの終了処理を行う
/// </summary>
/// <returns></returns>
bool cDirectX11::CleanupDirect3D()
{
	// デバイス・ステートのクリア
	if (m_pImmediateContext)
		m_pImmediateContext->ClearState();

	// スワップ チェインをウインドウ モードにする
	if (m_pSwapChain)
		m_pSwapChain->SetFullscreenState(FALSE, NULL);

	// 取得したインターフェイスの開放
	SAFE_RELEASE(m_pDepthStencilState);

	SAFE_RELEASE(m_pDepthStencilView);
	SAFE_RELEASE(m_pDepthStencil);

	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pSwapChain);

	SAFE_RELEASE(m_pImmediateContext);
	SAFE_RELEASE(m_pD3DDevice);

	return true;
}

/// <summary>
/// アプリケーションを終了させる
/// </summary>
void cDirectX11::CleanupApp()
{
	// ウインドウ クラスの登録解除
	UnregisterClass(g_szWndClass, m_hInstance);
	delete m_pComboBox;
	m_pComboBox = nullptr;
	delete m_pDX11;
	m_pDX11 = nullptr;
}

/// <summary>
/// 使用する機能をハードウェアが対応しているか確認する。今後機能が追加されればここに記述する
/// </summary>
/// <returns></returns>
bool cDirectX11::CheckSystem()
{
	// DirectX Mathライブラリのサポート チェック
	if (DirectX::XMVerifyCPUSupport() != TRUE)
	{
		DXTRACE_MSG("WinMain XMVerifyCPUSupport");
		return false;
	}

	//処理時間計測に使用するパフォーマンスカウンタの対応を確認する
	LARGE_INTEGER freq;
	if (!QueryPerformanceFrequency(&freq)) {
#if defined(DEBUG) || defined(_DEBUG)
		MessageBox(0, "この処理系ではハイパフォーマンスカウンタをサポートしていないため処理時間計測が行えません", NULL, MB_OK);
#endif
	}

	return true;		//すべてOK
}

/// <summary>
/// ゲーム中で利用した機能で解放処理を行うものがあれば記述する
/// </summary>
void cDirectX11::DestroyApp()
{
	//シーンマネージャの解放処理
	cSceneManager::GetInstance().Destroy();
	XInputEnable(false);	//コントローラ解除
	cPostEffects::Destroy();
}

/// <summary>
/// 終了処理を行い、WinMainがシステムに返却する値を返す。
/// WinMainはその値をそのままreturnする
/// </summary>
/// <returns></returns>
int cDirectX11::Destroy()
{
	CleanupDirect3D();
	CleanupApp();
	//アセットの解放確認
	cTexManager::GetInstance().CheckDelete();
	MeshManager::GetInstance().CheckDelete();
	return (int)msg.wParam;
}

ID3D11Device* GetDirectX::Device() {
	return cDirectX11::GetInstance()->Device();
}

ID3D11DeviceContext* GetDirectX::Context() {
	return cDirectX11::GetInstance()->Context();
}

HWND GetDirectX::Hwnd() {
	return cDirectX11::GetInstance()->GetHwnd();
}

HINSTANCE	GetDirectX::HInstance() {
	return cDirectX11::GetInstance()->HInstance();
}

void GetDirectX::InitOMSetRenderTargets()
{
	ID3D11RenderTargetView* pRTV[8] = { NULL };
	GetDirectX::Context()->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, pRTV, NULL);

	ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { NULL };
	GetDirectX::Context()->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);
	GetDirectX::Context()->GSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);
	GetDirectX::Context()->VSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);
}
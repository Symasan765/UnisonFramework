/*=================================================
//								FILE : DirectX11.h
//		ファイル説明 :
//		DIrectX11が行う初期化やデバイスの保持などを
//		行うクラスを定義し、記述する。
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once

#include "WinMain.h"
#include "TimeCheck.h"
#include "ProcessMonitor.h"

// TODO 2017/11/17現在 DirectX11オブジェクトのリリース処理が少ない。必ず点検すること

class cComboBox;

/// <summary>
/// DirectX11の機能を包括する。
/// 名称で競合は発生しないと思うのでわかりやすくこのまま命名。
/// シングルトンで作成するがよほどのことがなければアクセスしないような設計にする予定
/// </summary>
class cDirectX11 {
public:
	//WinMainとウィンドウプロシージャでのみプライベートにアクセスし、初期化などを行うことで他から呼び出されても問題がないようにする
	friend int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	friend LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam);
	cDirectX11(const cDirectX11&) = delete;			//コピーコンストラクタを delete 指定
	cDirectX11& operator=(const cDirectX11&) = delete;	//コピー代入演算子も delete 指定
	cDirectX11(cDirectX11&&) = delete;			//ムーブコンストラクタを delete 指定
	cDirectX11& operator=(cDirectX11&&) = delete;		//ムーブ代入演算子も delete 指定

	static cDirectX11* GetInstance() {
		return m_pDX11;
	}

	ID3D11Device* Device() { return m_pD3DDevice; };
	ID3D11DeviceContext* Context() { return m_pImmediateContext; };
	HWND GetHwnd() { return m_hWindow; };
	HINSTANCE	HInstance() {return m_hInstance;};
	D3D11_VIEWPORT GetViewPort() { return m_ViewPort; };
private:
	cDirectX11();									//コンストラクタを private に置く
	~cDirectX11() = default;								//デストラクタを private に置く
	static void Init();											//これで作成する。フレンド関数のWinMainからしか呼べない
	void Update();												//ゲームループの更新を開始する
	HRESULT Create(HINSTANCE _hInst);				//実際にシステムの作成を行う
	HRESULT InitWindow(HINSTANCE _hInst);			//ウィンドウ初期化
	bool InitApp();												//ゲーム中で利用するアプリケーションの初期化を行う
	void InitSceneManager();
	HRESULT InitDirect3D();									//Direct3Dの初期化を行う
	HRESULT InitBackBuffer();								//バックバッファの初期化
	bool CleanupDirect3D();
	void CleanupApp();
	bool CheckSystem();
	void DestroyApp();
	int Destroy();

	//ゲームの更新に必要な関数群
	void MainGameUpdate();
	bool GameUpdateFlag()const;
	void GameProcessingTime();		//計測した処理時間をまとめる

	//==========================管理するオブジェクト===============================
	static cDirectX11* m_pDX11;									//実態を格納
	ID3D11Device* m_pD3DDevice;									//デバイスハンドル
	HWND m_hWindow;														//ウィンドウハンドル
	HINSTANCE	m_hInstance;
	ID3D11DepthStencilState* m_pDepthStencilState;
	ID3D11DeviceContext* m_pImmediateContext;			// デバイス・コンテキスト
	IDXGISwapChain* m_pSwapChain;								 // スワップ・チェイン
	ID3D11RenderTargetView* m_pRenderTargetView;	//ターゲットビュー
	ID3D11Texture2D* m_pDepthStencil;						//深度・ステンシル
	ID3D11DepthStencilView* m_pDepthStencilView;		//深度ステンシルビュー
	D3D11_VIEWPORT m_ViewPort;									//ビューポート
	ID3D11RasterizerState* m_pCullNoneRasterizerState;		//ラスタライザ
	ID3D11RasterizerState* m_pCullLeftRasterizerState;		//左手系座標用
	ID3D11RasterizerState* m_pCullRightRasterizerState;		//右手系座標用
	ID3D11RasterizerState* m_pWIREFRAMERasterizerState;
	ID3D11BlendState* m_pBlendState;
	ID3D11BlendState* v_pAlphaState;							//アルファブレンドステート
	ID3D11BlendState* v_pAddState;							//加算合成ステート
	SIZE		m_sizeWindow;											// ウィンドウサイズ

	//====処理時間計測====
	cTimeCheck m_TimeAllProcess;		//フレーム全体の処理時間を計測
	cProcessMonitor m_ProcessMonitor;	//CPU使用率測定
	//=================

	//===============デバッグ機能で利用するフラグたち================

	bool m_DebugFlag;					//デバッグ中かの大元のフラグ
	bool m_PauseFlag;					//ポーズ機能を有効にするかどうかのフラグ
	bool m_FreeCameraFlag;		//カメラを自由に動かすかどうかのフラグ
	bool m_GBufferDrawFlag;		//デバッグ中にG-Bufferを表示するかどうか？
	cComboBox* m_pComboBox;

	//=======================================================
};
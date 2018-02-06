/*=================================================
//								FILE : SceneManager.h
//		ファイル説明 :
//		ゲームシーンの切り替えなどを管理するマネージャ。
//		ゲームループを隠蔽することが目的
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include <deque>
#include <memory>
#include "GameScene.h"
#include "TimeCheck.h"
#include "ProcessMonitor.h"
#include "ScreenSpaceSSS.h"
#include "CrossFilter.h"
//#include "SMAntialiasing.h"
#include "FXAntialiasing.h"
#include "OutlineEmphasis.h"
#include "SkyDome.h"
#include "DepthOfField.h"

/// <summary>
/// シーンを管理する。ゲームを通して一つでいいのでシングルトンとする
/// </summary>
class cSceneManager {
public:
	//更新処理や解放処理を行うためにcDirectXクラスをフレンド化しておく
	friend class cDirectX11;
	cSceneManager(const cSceneManager&) = delete;			//コピーコンストラクタを delete 指定
	cSceneManager& operator=(const cSceneManager&) = delete;	//コピー代入演算子も delete 指定
	cSceneManager(cSceneManager&&) = delete;			//ムーブコンストラクタを delete 指定
	cSceneManager& operator=(cSceneManager&&) = delete;		//ムーブ代入演算子も delete 指定

	static cSceneManager& GetInstance() {
		static cSceneManager inst;			// privateなコンストラクタを呼び出す
		return inst;
	}

	//=============================================
	//			シーン切り替えのPOP,PUSH,GOTOを記述
	// TODO 将来的に画面切り替え処理を入れる

	void cSceneManager::PopScene() {
		//現在実行中のシーンを削除するがここで直接POPするとシーンが自殺することになるので
		//フラグで管理をして描画終了後、更新が終わったタイミングでPOPする
		m_ScenePopFlag = true;
	};

	template<typename T>void PushScene() {
		//現在実行しているものの上に新しくシーンを作成する
		m_pSceneData.push_front(std::unique_ptr<T>(new T));
	};

	template<typename T>void GotoScene() {
		//現在実行中のシーンをPOPしてからPushする
		PopScene();
		PushScene<T>();
	};
	//=============================================

private:
	//各種更新処理
	void Update();
	//====== 描画系関数=====
	void Draw(bool _DebugFlag,bool _FreeCameraFlag,bool _GBufferDrawFlag);
	void RenderingStandby(bool _FreeCameraFlag);
	void RenderingShadowMap();
	void DeferredRenderingPass(bool _FreeCameraFlag);
	ID3D11ShaderResourceView* PostEffectPath(GBuffer& GraphicBuffer);
	void BackBufferRendering(ID3D11ShaderResourceView* LastData);
	//===================
	void DrawStart();
	void DrawEnd();
	void PopCheck();
	void FreeCameraUpdate();
	void GBufferDraw(GBuffer& _GBuffer);
	void SetProcessRate(int _No, int _Rate);
	void SetProcessNum(int _Num);
	void ProcessRateDraw();

	void Destroy();
	//=============================================


	cSceneManager();					//コンストラクタを private に置く
	~cSceneManager();					//デストラクタを private に置く
	std::deque<std::unique_ptr<cGameScene>> m_pSceneData;		//シーンデータを格納するdeque
	const cGameScene* m_pOldScene;							//更新処理を不可にするためconstは絶対に外さないこと
	cRenderTargetTex* m_pDefaultRnedTex;				//システムとして標準的に描画するテクスチャ

	IDXGISwapChain* m_pSwapChain;								 // スワップ・チェイン
	ID3D11DeviceContext* m_pImmediateContext;			// デバイス・コンテキスト
	ID3D11RenderTargetView* m_pRenderTargetView;	//ターゲットビュー
	ID3D11DepthStencilView* m_pDepthStencilView;		//深度ステンシルビュー

	//=====エフェクト========
	cSkyDome* m_SkyDome;	//スカイドーム描画に利用する
	cScreenSpaceSSS* m_SSSSS;
	cCrossFilter* m_CrossFilter;
	cFXAA* m_FXAA;
	cOutlineEmphasis* m_OutlineEmphasis;
	cDepthOfField* m_DepthOfField;


	//=====処理時間計測=====
	cTimeCheck m_TimeUpdate;				//ゲームアップデートの処理時間計測を行う
	cTimeCheck m_TimeDraw;					//描画の処理時間計測を行う
	int m_ProcessRate[MAX_PROCESSORS_COUNT];	//CPU使用率をシステムからもらって保持しておく
	int m_ProcessNum;		//プロセス数
	ID3D11ShaderResourceView* m_pCPURateBar;	//CPU使用率ゲージを表示させるのに利用する
	ID3D11ShaderResourceView* m_pMemoryRateBar;	//メモリの使用率を表示する
	ID3D11ShaderResourceView* m_pBaseBar;			//他のバーの下に表示する
	//===================
	bool m_ScenePopFlag;											//シーンがPOPされたかどうかのフラグ
};

//シーンマネージャをラップして呼び出す
namespace Scene {
	void Pop();
	template<typename T>void Push() {
		//現在実行しているものの上に新しくシーンを作成する
		cSceneManager::GetInstance().PushScene<T>();
	};
	template<typename T>void Goto() {
		//現在実行しているものの上に新しくシーンを作成する
		cSceneManager::GetInstance().GotoScene<T>();
	};
}
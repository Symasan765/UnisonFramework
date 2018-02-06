/*=================================================
//								FILE :GameScene.h
//		ファイル説明 :
//		ゲームシーンの基底クラスを作成する。
//		ゲームシーンにはDXのデバイスやカメラ、ディクショナルライト情報も保持させる
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "WinMain.h"
#include "DirectXData.h"
#include "Camera.h"
#include "DirectionalLight.h"
#include "GBuffer.h"
#include "ToonParam.h"

/// <summary>
/// シーンに適用するシェーダのフラグを管理する
/// </summary>
struct SeneRenderFlag {
	bool vShaowFlag;		//シャドウを適用するか？
};

//ブレンドステートを設定する情報
enum class BrendStateNo {
	NONE = 0,
	ALPHA = 1,
	ADD = 2
};

//カリング方法を選択する
enum class CullMode {
	NONE = 0,		//カリングなし
	LEFT,				//左手系表
	RIGHT,			//右手系表
	WIRE
};

/// <summary>
/// 各ゲームシーンに派生させる基底クラス
/// 利用する時は Scene::Push<シーンクラス名>(); と記述して扱う
/// </summary>
class cGameScene {
public:
	//更新処理や解放処理を行うためにcDirectXクラスをフレンド化しておく
	friend class cDirectX11;
	cGameScene();
	virtual ~cGameScene();
	virtual void Update() = 0;
	virtual void MeshDraw() = 0;				//ディファードレンダリングで利用するGbufferを作成する
	virtual void UIDraw() = 0;				//UIを描画する
	
	void SetDefaultRender();
	void SetBlendState(BrendStateNo _OnFlag);
	void SetFogColor(const DirectX::XMFLOAT4& _Color);
	void SetFogFactor(const float _f);
	void SetShadowFlag(bool _Flag);
	void SetRasterizerState(CullMode mode = CullMode::NONE);
	void SetRendBuffer();
	void SetRadialBlur(DirectX::XMFLOAT3 _Pos, float _Delta, int _Num);
	void SetDepthStencilState();
	bool IsRadialBlur();
	RadialBlurData GetRadialBlur(const DirectX::XMFLOAT4X4& _VP);
	const FogData& GetFogData() { return m_Fog; };
	const DoFData& GetDoFData() { return m_DoF; };

	cCamera m_CameraData;
	cDirectionalLight m_DirectionalLight;	//ディクショナルライト
	SeneRenderFlag m_RenderFlag;				//適用するシェーダフラグ

	cToonParam m_ToonParam;		//トゥーンパラメータ用クラス
private:
	static void SetDirectXData(DirectXData _data);
	static DirectXData m_DirectXData;		//システムデータをシーン全体で保持する
	static DirectXDrawData m_TexDrawData;
	
	FogData m_Fog;	//フォグの情報
	DoFData m_DoF;	//被写界深度情報
	RadialBlurBuf m_Blur;	//ラジアルブラーデータ
};
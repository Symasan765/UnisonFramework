/*=================================================
//								FILE : DeferredRendering.h
//		ファイル説明 :
//		ディファードレンダリングを取り扱うクラス
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "WinMain.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "GBuffer.h"

/// <summary>
/// ディファードレンダリングを管理するクラス
/// </summary>
class cDeferredDemo {
public:
	cDeferredDemo(const cDeferredDemo&) = delete;			//コピーコンストラクタを delete 指定
	cDeferredDemo& operator=(const cDeferredDemo&) = delete;	//コピー代入演算子も delete 指定
	cDeferredDemo(cDeferredDemo&&) = delete;			//ムーブコンストラクタを delete 指定
	cDeferredDemo& operator=(cDeferredDemo&&) = delete;		//ムーブ代入演算子も delete 指定

	static cDeferredDemo& GetInstance() {
		static cDeferredDemo inst;			// privateなコンストラクタを呼び出す
		return inst;
	}

	void SetDeferredRendering(ID3D11ShaderResourceView* _DepthMap);
	ID3D11ShaderResourceView* GetResourceView(int _no);
	GBuffer GetGraphicBuffer();
	void SetDeferredShader();
private:
	enum RenderName {
		DIFFUSE = 0,		//カラー(照明計算済)
		POSITION,			//座標
		NORMAL,				//法線(wに深度値を格納する)
		BRIGHT,				//高輝度情報
		SSSSS,				//xyz : SSS適用部ディフューズ w : SSS適用部マスク
		TARGET_MAX		//作成テクスチャ数
	};
	cDeferredDemo();					//コンストラクタを private に置く
	~cDeferredDemo();					//デストラクタを private に置く

											//===================変数======================//
	cRenderTargetTex* m_pRenderTergetTex;		//G-Bufferで使用するテクスチャを保持する
	ID3D11SamplerState* m_pSampleLinear;			//サンプラ。ここにいるんかな…？
	std::shared_ptr<ShaderData> m_Shader;									//ディファードレンダリングシェーダを保持
};
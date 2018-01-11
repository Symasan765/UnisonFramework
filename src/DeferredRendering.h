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

//ディファードレンダリング上で使用できるシェーダ(ビット演算にて複数対応)
enum class ShaderType {
	Lambert = 1,				//ランバート拡散光
	BlinnPhong = 2,			//ブリンフォン
	CookTorrance = 4,		//クックトランス金属反射モデル
	Edge = 8,					//エッジ抽出
	DOF = 16,					//被写界深度
	Toon = 32					//トゥーンシェード
};

/// <summary>
/// ディファードレンダリングを管理するクラス
/// </summary>
class cDeferredRendering {
public:
	cDeferredRendering(const cDeferredRendering&) = delete;			//コピーコンストラクタを delete 指定
	cDeferredRendering& operator=(const cDeferredRendering&) = delete;	//コピー代入演算子も delete 指定
	cDeferredRendering(cDeferredRendering&&) = delete;			//ムーブコンストラクタを delete 指定
	cDeferredRendering& operator=(cDeferredRendering&&) = delete;		//ムーブ代入演算子も delete 指定

	static cDeferredRendering& GetInstance() {
		static cDeferredRendering inst;			// privateなコンストラクタを呼び出す
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
	cDeferredRendering();					//コンストラクタを private に置く
	~cDeferredRendering();					//デストラクタを private に置く

	//===================変数======================//
	cRenderTargetTex* m_pRenderTergetTex;		//G-Bufferで使用するテクスチャを保持する
	ID3D11SamplerState* m_pSampleLinear;			//サンプラ。ここにいるんかな…？
	std::shared_ptr<ShaderData> m_Shader;									//ディファードレンダリングシェーダを保持
};
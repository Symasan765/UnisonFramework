/*=================================================
//								FILE : ShadowMap.cpp
//		ファイル説明 :
//		シャドウマップを取り扱うクラス
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "WinMain.h"
#include "ConstantBuffer.h"
#include "ShaderStruct.h"
#include "RenderTarget.h"
#include "Shader.h"

#define SHADOW_MAP_WIDTH 4096
#define SHADOW_MAP_HEIGHT 4096
#define SHADOW_MAP_NEAR 1.0f
#define SHADOW_MAP_FAR 300.0f
#define SHADOW_MAP_VIEWING_ANGLE 10.0f

/// <summary>
/// シャドウマップを取り扱うクラス
/// 現段階でテクスチャ１枚による標準的なシャドウマップに対応させる
/// </summary>
class cShadowMap {
public:
	cShadowMap(const cShadowMap&) = delete;			//コピーコンストラクタを delete 指定
	cShadowMap& operator=(const cShadowMap&) = delete;	//コピー代入演算子も delete 指定
	cShadowMap(cShadowMap&&) = delete;			//ムーブコンストラクタを delete 指定
	cShadowMap& operator=(cShadowMap&&) = delete;		//ムーブ代入演算子も delete 指定

	static cShadowMap& GetInstance() {
		static cShadowMap inst;			// privateなコンストラクタを呼び出す
		return inst;
	}
	void SetRender();		//描画準備
	ID3D11ShaderResourceView* GetDepthResourceView();
private:
	cShadowMap();					//コンストラクタを private に置く
	~cShadowMap();					//デストラクタを private に置く

	//==============================================================//
	cRenderTargetTex* m_DepthTarget;			//深度書き込みテクスチャ
	std::shared_ptr<ShaderData> m_Shader;							//深度マップ作成シェーダ
};


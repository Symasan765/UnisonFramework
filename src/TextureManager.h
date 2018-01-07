/*=================================================
//								FILE : TextureManager.h
//		ファイル説明 :
//		テクスチャの複数読み込みを防止するためにテクスチャを管理する
//		マネージャクラスを定義してここから読むようにする。
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once

#include <d3d11.h>
#include <memory>
#include <map>

/// <summary>
/// テクスチャの管理を行うクラス
/// テクスチャを読み込む際はLoadTexの戻り値をshard_ptrで受け取ればテクスチャを取得できる
/// </summary>
struct Texture2D {
	ID3D11ShaderResourceView* vpTexture;

	~Texture2D() {
		if (vpTexture) {
			vpTexture->Release();
			vpTexture = nullptr;
		}
	}
};

class cTexManager {
public:
	//更新処理や解放処理を行うためにcDirectXクラスをフレンド化しておく
	friend class cDirectX11;
	cTexManager(const cTexManager&) = delete;			//コピーコンストラクタを delete 指定
	cTexManager& operator=(const cTexManager&) = delete;	//コピー代入演算子も delete 指定
	cTexManager(cTexManager&&) = delete;			//ムーブコンストラクタを delete 指定
	cTexManager& operator=(cTexManager&&) = delete;		//ムーブ代入演算子も delete 指定

	static cTexManager& GetInstance() {
		static cTexManager inst;			// privateなコンストラクタを呼び出す
		return inst;
	}

	enum SampleState {
		WRAP = 0,
		CLAMP
	};

	std::shared_ptr<Texture2D> LoadTexData(std::string _fileName,bool _meshFlag = false);	//読み込み関数		TODO:
	HRESULT CreateTexSampler(ID3D11SamplerState** m_pSampleLinear, SampleState samp = WRAP);
private:
	HRESULT GetTex(std::string _fileName, Texture2D* pTex);		//テクスチャを新しくロードする場合に使用する
	void CheckDelete();
	cTexManager() = default;					//コンストラクタを private に置く
	~cTexManager() = default;					//デストラクタを private に置く

	std::map<std::string, std::shared_ptr<Texture2D>> m_TexMap;		//テクスチャを保持するマップ
};
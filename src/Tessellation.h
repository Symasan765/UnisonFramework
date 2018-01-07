/*=================================================
//								FILE : Tessellation.h
//		ファイル説明 :
//		テッセレーションステージであるハルシェーダ、ドメインシェーダーの
//		コンパイルを担当する
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "WinMain.h"
#include <memory>
#include <map>
#include <string>

/// <summary>
/// テッセレーションで利用するシェーダ各種を保持する
/// </summary>
struct TessellationStruct {
	ID3D11HullShader* m_pHullShader;
	ID3D11DomainShader* m_pDomainShader;

	TessellationStruct() {
		m_pHullShader = nullptr;
		m_pDomainShader = nullptr;
	}

	void SetTessellation() {
		GetDirectX::Context()->HSSetShader(m_pHullShader, NULL, 0);
		GetDirectX::Context()->DSSetShader(m_pDomainShader, NULL, 0);
	}

	~TessellationStruct() {
		release();
	}

	void release() {
		SAFE_RELEASE(m_pHullShader);
		SAFE_RELEASE(m_pDomainShader);
	}
};

/// <summary>
/// テッセレーションのロードなどの管理を担当する
/// </summary>
class cTessellationManager {
public:
	//更新処理や解放処理を行うためにcDirectXクラスをフレンド化しておく
	friend class cDirectX11;
	cTessellationManager(const cTessellationManager&) = delete;			//コピーコンストラクタを delete 指定
	cTessellationManager& operator=(const cTessellationManager&) = delete;	//コピー代入演算子も delete 指定
	cTessellationManager(cTessellationManager&&) = delete;			//ムーブコンストラクタを delete 指定
	cTessellationManager& operator=(cTessellationManager&&) = delete;		//ムーブ代入演算子も delete 指定

	static cTessellationManager& GetInstance() {
		static cTessellationManager inst;			// privateなコンストラクタを呼び出す
		return inst;
	}

	std::shared_ptr<TessellationStruct> LoadTessellationData(std::string);
	void SetTessellationToNull();
	HRESULT ShaderReload(std::string _fileName);
private:
	HRESULT GetShader(std::string _fileName, TessellationStruct* pTesse);
	void CheckDelete();
	cTessellationManager() = default;					//コンストラクタを private に置く
	~cTessellationManager() = default;					//デストラクタを private に置く
	std::map<std::string, std::shared_ptr<TessellationStruct>> m_Map;		//テッセレーションを保持するマップ
};
/*=================================================
//								FILE : TextureManager.h
//		ファイル説明 :
//		テクスチャの複数読み込みを防止するためにテクスチャを管理する
//		マネージャクラスを定義してここから読むようにする。
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "TextureManager.h"
#include "WinMain.h"
#include <d3dx11.h>

/// <summary>
/// テクスチャを読み込む時に利用する関数
/// </summary>
/// <param name="_fileName">ファイル名。(フォルダまでのパスは自動で付与する)</param>
/// <param name="_meshFlag">メッシュに付与するデータなのか？(メッシュのデータの場合、パスが変わる)</param>
/// <returns>テクスチャを保持したシェアードポインタ</returns>
std::shared_ptr<Texture2D> cTexManager::LoadTexData(std::string _fileName,bool _meshFlag)
{
	//まずそのテクスチャがモデルから読み込まれたのか通常状態で読まれるのかでパスを変更する
	std::string path;
	if (_meshFlag) {
		path = _fileName;
	}
	else {
		path = "asset/Texture/";
		path += _fileName;
	}

	//その名前がmapに登録されているか確認する
	auto itr = m_TexMap.find(path);        // "xyz" が設定されているか？
	if (itr != m_TexMap.end()) {
		//設定されている場合の処理
		return m_TexMap[path];		//shared_ptrのコピー
	}
	//次になければmapに領域を確保してから拡張子を判別して各パーサーを呼び出してデータを取得する
	else {
		//設定されていない場合の処理
		m_TexMap[path] = std::make_shared<Texture2D>();

		GetTex(path, &*m_TexMap[path]);
	}
	return m_TexMap[path];		//コピー
}

/// <summary>
/// テクスチャサンプラを作ってくれる関数
/// </summary>
/// <param name="m_pSampleLinear">出力するためのサンプラステート</param>
/// <param name="samp">作成するサンプラ種類指定</param>
/// <returns>成否</returns>
HRESULT cTexManager::CreateTexSampler(ID3D11SamplerState** m_pSampleLinear, SampleState samp)
{
	//テクスチャー用サンプラー作成
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));

	switch (samp)
	{
	case cTexManager::WRAP:
		SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		break;
	case cTexManager::CLAMP:
		SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	default:
		break;
	}
	
	GetDirectX::Device()->CreateSamplerState(&SamDesc, m_pSampleLinear);

	// TODO 将来的にはもっと自由にパラメータを指定して設定できるようにする。

	return S_OK;
}

/// <summary>
/// 内部で隠蔽されたテクスチャ読み込み部分。LoadTexDataから呼び出される
/// </summary>
/// <param name="_fileName">ファイル名</param>
/// <param name="pTex">出力するテクスチャ</param>
/// <returns>成否</returns>
HRESULT cTexManager::GetTex(std::string _fileName, Texture2D* pTex) {

	if (FAILED(D3DX11CreateShaderResourceViewFromFileA(GetDirectX::Device(), _fileName.c_str(), NULL, NULL, &pTex->vpTexture, NULL))) {
		if (FAILED(D3DX11CreateShaderResourceViewFromFileA(GetDirectX::Device(), _fileName.c_str(), NULL, NULL, &pTex->vpTexture, NULL))) {
			MessageBox(0, "テクスチャー読み込み失敗", NULL, MB_OK);
			return E_FAIL;
		}
	}
	return S_OK;
}

/// <summary>
/// ループの最初にテクスチャが解放されたかどうかをチェックするために関数。
/// </summary>
void cTexManager::CheckDelete()
{
	for (auto itr = m_TexMap.begin(); itr != m_TexMap.end(); ++itr) {
		//mapしかポインタを保持していない
		if (itr->second.use_count() == 1) {
			m_TexMap.erase(itr->first);		//データ削除
			itr = m_TexMap.begin();
			if (itr == m_TexMap.end())
				return;
		}
	}
}
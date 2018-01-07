/*=================================================
//								FILE : Tessellation.cpp
//		ファイル説明 :
//		テッセレーションステージであるハルシェーダ、ドメインシェーダーの
//		コンパイルを担当する
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "Tessellation.h"
#include "ShaderMonitoring.h"

namespace {
	std::string dire = "HLSL/";		//HLSLが格納されるディレクトリ
}

std::shared_ptr<TessellationStruct> cTessellationManager::LoadTessellationData(std::string _FileName)
{
	std::string path = dire + _FileName;

	//その名前がmapに登録されているか確認する
	auto itr = m_Map.find(path);
	if (itr != m_Map.end()) {
		//設定されている場合の処理
		return m_Map[path];		//shared_ptrのコピー
	}
	//次になければmapに領域を確保してから拡張子を判別して各パーサーを呼び出してデータを取得する
	else {
		//設定されていない場合の処理
		m_Map[path] = std::make_shared<TessellationStruct>();

		GetShader(path, &*m_Map[path]);
		cShaderMonitoring::GetInstance().AddFileDate(path);
	}
	return m_Map[path];		//コピー
}

/// <summary>
/// シェーダを利用しない際にまとめてテッセレーションステージをオフにする
/// </summary>
void cTessellationManager::SetTessellationToNull()
{
	GetDirectX::Context()->HSSetShader(NULL, NULL, 0);
	GetDirectX::Context()->DSSetShader(NULL, NULL, 0);
}

HRESULT cTessellationManager::ShaderReload(std::string path)
{
	auto itr = m_Map.find(path);
	if (itr != m_Map.end()) {
		//設定されている場合の処理
		m_Map[path]->release();
		return GetShader(path, &*m_Map[path]);
	}
	
	return S_OK;		//なければ別に問題ない
}

HRESULT cTessellationManager::GetShader(std::string _fileName, TessellationStruct * pTesse)
{
	ID3DBlob *pCompiledShader = nullptr;
	ID3DBlob *pErr = nullptr;
	//ブロブからハルシェーダー作成
	if (FAILED(D3DX11CompileFromFile(_fileName.c_str(), NULL, NULL, "HS", "hs_5_0", 0, 0, NULL, &pCompiledShader, &pErr, NULL)))
	{
		MessageBox(0, (_fileName + std::string("の読み込みに失敗")).c_str(), NULL, MB_OKCANCEL);
		return E_FAIL;
	}
	SAFE_RELEASE(pErr);
	if (FAILED(GetDirectX::Device()->CreateHullShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), NULL, &pTesse->m_pHullShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, (_fileName + std::string("のHS作成に失敗")).c_str(), NULL, MB_OKCANCEL);
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);

	//ブロブからドメインシェーダー作成
	if (FAILED(D3DX11CompileFromFile(_fileName.c_str(), NULL, NULL, "DS", "ds_5_0", 0, 0, NULL, &pCompiledShader, &pErr, NULL)))
	{
		MessageBox(0, (_fileName + std::string("の読み込みに失敗")).c_str(), NULL, MB_OKCANCEL);
		return E_FAIL;
	}
	SAFE_RELEASE(pErr);
	if (FAILED(GetDirectX::Device()->CreateDomainShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), NULL, &pTesse->m_pDomainShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, (_fileName + std::string("のDS作成に失敗")).c_str(), NULL, MB_OKCANCEL);
		return E_FAIL;
	}

	SAFE_RELEASE(pCompiledShader);
	return S_OK;
}

/// <summary>
/// 削除されて使われていないデータが残っているか確認
/// </summary>
void cTessellationManager::CheckDelete()
{
	for (auto itr = m_Map.begin(); itr != m_Map.end(); ++itr) {
		//mapしかポインタを保持していない
		if (itr->second.use_count() == 1) {
			m_Map.erase(itr->first);		//データ削除
			itr = m_Map.begin();
			if (itr == m_Map.end())
				return;
		}
	}
}

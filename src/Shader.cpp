/*=================================================
//								FILE : Shader.cpp
//		ファイル説明 :
//		シェーダの読み込みなどを行うクラスを記述する
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "Shader.h"
#include "CompileOptions.h"
#include <WinBase.h>
#include <WinBase.h>
#include "ShaderMonitoring.h"

using namespace CompileOptions;
namespace {
	std::string DirectoryPath = "HLSL/";
}

/// <summary>
/// シェーダファイルをロードし、コンパイルする
/// </summary>
/// <param name="_fileName">ファイル名。フォルダまでのパスは自動で付与</param>
/// <param name="bitFlag"></param>
/// <param name="_GSflag"></param>
/// <returns></returns>
std::shared_ptr<ShaderData> SHADER::LoadShaderFile(std::string _fileName, const short bitFlag, bool _GSflag)
{
	//まずそのテクスチャがモデルから読み込まれたのか通常状態で読まれるのかでパスを変更する
	std::string path = DirectoryPath + _fileName;

	//その名前がmapに登録されているか確認する
	auto itr = m_Map.find(path);        // "xyz" が設定されているか？
	if (itr != m_Map.end()) {
		//設定されている場合の処理
		return m_Map[path];		//shared_ptrのコピー
	}
	//次になければmapに領域を確保してから拡張子を判別して各パーサーを呼び出してデータを取得する
	else {
		//設定されていない場合の処理
		m_Map[path] = std::make_shared<ShaderData>();

		//ここにシェーダロード関数を入れる
		LoadShader(path, bitFlag,&*m_Map[path], _GSflag);
		cShaderMonitoring::GetInstance().AddFileDate(path);
	}
	return m_Map[path];		//コピー
}

/// <summary>
/// シェーダのリロード処理を行う
/// </summary>
/// <param name="path"></param>
/// <returns></returns>
HRESULT SHADER::ShaderReload(std::string path)
{
	auto itr = m_Map.find(path);
	if (itr != m_Map.end()) {
		//設定されている場合の処理
		bool GSFlag = m_Map[path]->release();
		return LoadShader(itr->first, 0, &*m_Map[path], GSFlag);
	}

	return S_OK;		//なければ別に問題ない
}

/// <summary>
/// シェーダのロードを行う
/// </summary>
/// <param name="_ShaderFileName">ファイルパス</param>
/// <param name="bitFlag">読み込むシェーダの種類をビット演算で指定する</param>
/// <param name="_data">格納するポインタ</param>
/// <param name="_GSflag">ジオメトリシェーダを読み込むフラグ</param>
/// <returns></returns>
HRESULT SHADER::LoadShader(std::string _ShaderFileName, const short bitFlag, ShaderData* _data, bool _GSflag)
{
	// 頂点シェーダのコードをコンパイル===========================================
	ID3DBlob* pBlobVS = NULL;
	HRESULT hr = D3DX11CompileFromFile(
		_ShaderFileName.c_str(),  // ファイル名
		NULL,          // マクロ定義(なし)
		NULL,          // インクルード・ファイル定義(なし)
		"VS",          // 「VS関数」がシェーダから実行される
		"vs_5_0",      // 頂点シェーダ
		g_flagCompile, // コンパイル・オプション
		0,             // エフェクトのコンパイル・オプション(なし)
		NULL,          // 直ぐにコンパイルしてから関数を抜ける。
		&pBlobVS,      // コンパイルされたバイト・コード
		NULL,          // エラーメッセージは不要
		NULL);         // 戻り値
	if (FAILED(hr))
		return DXTRACE_ERR("InitDirect3D D3DX11CompileShaderFromFile", hr);

	// 頂点シェーダの作成
	hr = GetDirectX::Device()->CreateVertexShader(
		pBlobVS->GetBufferPointer(), // バイト・コードへのポインタ
		pBlobVS->GetBufferSize(),    // バイト・コードの長さ
		NULL,
		&_data->vpVertexShader); // 頂点シェーダを受け取る変数
						   //	SAFE_RELEASE(pBlobVS);  // バイト・コードを解放
	if (FAILED(hr)) {
		SAFE_RELEASE(pBlobVS);
		return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateVertexShader", hr);
	}

	//ここにレイアウト呼び出し
	GetInputLayout(_data, pBlobVS, bitFlag);
	SAFE_RELEASE(pBlobVS);


	// ピクセル・シェーダのコードをコンパイル
	ID3DBlob* pBlobPS = NULL;
	hr = D3DX11CompileFromFile(
		_ShaderFileName.c_str(),  // ファイル名
		NULL,          // マクロ定義(なし)
		NULL,          // インクルード・ファイル定義(なし)
		"PS",          // 「PS関数」がシェーダから実行される
		"ps_5_0",      // ピクセル・シェーダ
		g_flagCompile, // コンパイル・オプション
		0,             // エフェクトのコンパイル・オプション(なし)
		NULL,          // 直ぐにコンパイルしてから関数を抜ける。
		&pBlobPS,      // コンパイルされたバイト・コード
		NULL,          // エラーメッセージは不要
		NULL);         // 戻り値
	if (FAILED(hr))
		return DXTRACE_ERR("InitDirect3D D3DX11CompileShaderFromFile", hr);

	// ピクセル・シェーダの作成
	hr = GetDirectX::Device()->CreatePixelShader(
		pBlobPS->GetBufferPointer(), // バイト・コードへのポインタ
		pBlobPS->GetBufferSize(),    // バイト・コードの長さ
		NULL,
		&_data->vpPixelShader); // ピクセル・シェーダを受け取る変数
	SAFE_RELEASE(pBlobPS);  // バイト・コードを解放
	if (FAILED(hr))
		return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreatePixelShader", hr);

	if (_GSflag) {
		ID3DBlob* pBlobGS = NULL;
		hr = D3DX11CompileFromFile(
			_ShaderFileName.c_str(),  // ファイル名
			NULL,          // マクロ定義(なし)
			NULL,          // インクルード・ファイル定義(なし)
			"GS",          // 「GS関数」がシェーダから実行される
			"gs_5_0",      // ジオメトリ・シェーダ
			g_flagCompile, // コンパイル・オプション
			0,             // エフェクトのコンパイル・オプション(なし)
			NULL,          // 直ぐにコンパイルしてから関数を抜ける。
			&pBlobGS,      // コンパイルされたバイト・コード
			NULL,          // エラーメッセージは不要
			NULL);         // 戻り値
		if (FAILED(hr))
			return DXTRACE_ERR("InitDirect3D D3DX11CompileShaderFromFile", hr);

		// ジオメトリ・シェーダの作成
		hr = GetDirectX::Device()->CreateGeometryShader(
			pBlobGS->GetBufferPointer(), // バイト・コードへのポインタ
			pBlobGS->GetBufferSize(),    // バイト・コードの長さ
			NULL,
			&_data->vpGeometryShader); // ジオメトリ・シェーダを受け取る変数
		SAFE_RELEASE(pBlobGS);  // バイト・コードを解放
		if (FAILED(hr))
			return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreatePixelShader", hr);
	}
	else {
		_data->vpGeometryShader = nullptr;
	}
}

//力技シリーズ
HRESULT SHADER::GetInputLayout(ShaderData* _data, ID3DBlob* blob, const short bitFlag)
{
	//頂点、法線、UV
	if (bitFlag == (inPOSITION | inNORMAL | inTEX_UV)) {
		// 入力要素================================================
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(DirectX::XMFLOAT4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::XMFLOAT4) + sizeof(DirectX::XMFLOAT4), D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		// 入力レイアウト・オブジェクトの作成
		HRESULT hr = GetDirectX::Device()->CreateInputLayout(
			layout,                            // 定義の配列
			_countof(layout),                  // 定義の要素数
			blob->GetBufferPointer(),       // バイト・コードへのポインタ
			blob->GetBufferSize(),          // バイト・コードのサイズ
			&_data->vpInputLayout);                  // 受け取る変数のポインタ

		if (FAILED(hr))
			return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateInputLayout", hr);
	}
	else if (bitFlag == (inPOSITION | inTEX_UV)) {
		// 入力要素================================================
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::XMFLOAT4), D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		// 入力レイアウト・オブジェクトの作成
		HRESULT hr = GetDirectX::Device()->CreateInputLayout(
			layout,                            // 定義の配列
			_countof(layout),                  // 定義の要素数
			blob->GetBufferPointer(),       // バイト・コードへのポインタ
			blob->GetBufferSize(),          // バイト・コードのサイズ
			&_data->vpInputLayout);                  // 受け取る変数のポインタ

		if (FAILED(hr))
			return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateInputLayout", hr);
	}
	else if (bitFlag == (inPOSITION | inNORMAL | inTEX_UV | inBone | inWeight)) {
		// 入力要素================================================
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(DirectX::XMFLOAT4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::XMFLOAT4) + sizeof(DirectX::XMFLOAT4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONE_INDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, sizeof(DirectX::XMFLOAT4) + sizeof(DirectX::XMFLOAT4) + sizeof(DirectX::XMFLOAT2), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONE_WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(DirectX::XMFLOAT4) + sizeof(DirectX::XMFLOAT4) + sizeof(DirectX::XMFLOAT2) + sizeof(DirectX::XMUINT4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// 入力レイアウト・オブジェクトの作成
		HRESULT hr = GetDirectX::Device()->CreateInputLayout(
			layout,                            // 定義の配列
			_countof(layout),                  // 定義の要素数
			blob->GetBufferPointer(),       // バイト・コードへのポインタ
			blob->GetBufferSize(),          // バイト・コードのサイズ
			&_data->vpInputLayout);                  // 受け取る変数のポインタ

		if (FAILED(hr))
			return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateInputLayout", hr);
	}
}

void SHADER::CheckDelete()
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

SHADER::SHADER() {
	ConstWorldMtx = nullptr;
	ConstLightVec = nullptr;
	ConstMaterial = nullptr;
}

SHADER::~SHADER() {
	SAFE_RELEASE(ConstWorldMtx);
	SAFE_RELEASE(ConstLightVec);
	SAFE_RELEASE(ConstMaterial);
}

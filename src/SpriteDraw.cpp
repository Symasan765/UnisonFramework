/*=================================================
//								FILE :　SpriteDraw.cpp
//		ファイル説明 :
//		２Dスプライトの描画を行う
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "SpriteDraw.h"
#include "Vertex.h"
#include "WinMain.h"
#include "CompileOptions.h"

using namespace CompileOptions;

/// <summary>
/// スプライトの描画を行う
/// </summary>
/// <param name="_tex">描画するテクスチャ</param>
/// <param name="_LeftTop">右上の座標(スクリーンスペース)</param>
/// <param name="_RightDown">左下の座標(スクリーンスペース)</param>
void cSprite2DDraw::Draw(ID3D11ShaderResourceView * _tex, DirectX::XMFLOAT2 _LeftTop, DirectX::XMFLOAT2 _RightDown)
{
	//まずは変換行列を作成する
	DirectX::XMFLOAT4X4 mvp;		//スクリーン空間への補正

	mvp._11 = 2.0f / (float)WINDOW_SIZE_X;
	mvp._12 = 0;
	mvp._13 = 0;
	mvp._14 = 0;

	mvp._21 = 0;
	mvp._22 = -2.0f / (float)WINDOW_SIZE_Y;
	mvp._23 = 0;
	mvp._24 = 0;

	mvp._31 = 0;
	mvp._32 = 0;
	mvp._33 = 1.0f;
	mvp._34 = 0;

	mvp._41 = -1.0f;
	mvp._42 = 1.0f;
	mvp._43 = m_NowDepth;
	mvp._44 = 1.0f;

	const float PosX = _LeftTop.x;
	const float PosY = _LeftTop.y;
	const float ScaleX = _RightDown.x;
	const float ScaleY = _RightDown.y;
	DirectX::XMMATRIX mw = DirectX::XMMatrixScaling(ScaleX, ScaleY, 0);		//サイズ1のポリゴンを指定サイズに拡大
	mw *= DirectX::XMMatrixTranslation(PosX, PosY, 0);								//指定位置に移動させる
	mw *= DirectX::XMLoadFloat4x4(&mvp);														//スクリーン空間に変換する
	DirectX::XMStoreFloat4x4(&m_ShaderConst.data, DirectX::XMMatrixTranspose(mw));
	
	m_ShaderConst.Set(0);		//定数をシェーダにセットする

	GetDirectX::Context()->IASetInputLayout(vpInputLayout);
	GetDirectX::Context()->VSSetShader(vpVertexShader, NULL, 0);
	GetDirectX::Context()->GSSetShader(NULL, NULL, 0);
	GetDirectX::Context()->PSSetShader(vpPixelShader, NULL, 0);

	GetDirectX::Context()->PSSetSamplers(0, 1, &m_pSampleLinear);

	//バーテックスバッファーをセット（バーテックスバッファーは一つでいい）
	UINT stride = sizeof(XYZUV);
	UINT offset = 0;
	GetDirectX::Context()->IASetVertexBuffers(0, 1, &vVertexBuf, &stride, &offset);

	GetDirectX::Context()->PSSetShaderResources(0, 1, &_tex);		//テクスチャセット
	//プリミティブ・トポロジーをセット
	GetDirectX::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//Draw
	GetDirectX::Context()->Draw(4, 0);

	m_NowDepth -= 0.001f;		//スプライトの描画を前面にずらす
}

/// <summary>
/// 初期化
/// </summary>
cSprite2DDraw::cSprite2DDraw()
{
	vpVertexShader = nullptr;
	vpPixelShader = nullptr;
	vpInputLayout = nullptr;
	vVertexBuf = nullptr;			//頂点バッファ

	Create();
}

/// <summary>
/// 解放処理
/// </summary>
cSprite2DDraw::~cSprite2DDraw()
{
	SAFE_RELEASE(vpVertexShader);
	SAFE_RELEASE(vpPixelShader);
	SAFE_RELEASE(vpInputLayout);
	SAFE_RELEASE(vVertexBuf);
}

/// <summary>
/// 描画に利用するデータ群を作成する(初期化時にのみ作成)
/// </summary>
void cSprite2DDraw::Create()
{
	CreateVertex();
	CreateShader();
	//テクスチャー用サンプラー作成
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));

	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	GetDirectX::Device()->CreateSamplerState(&SamDesc, &m_pSampleLinear);
}

/// <summary>
/// 頂点情報作成
/// </summary>
/// <returns></returns>
HRESULT cSprite2DDraw::CreateVertex()
{
	if (true) {
		const float max = 1.0f;
		XYZUV vertex[4] = {
			{{0.0f,max,0.0f,1.0f},{0.0f,1.0f}},	//左下
			{{0.0f,0.0f,0.0f,1.0f},{0.0f,0.0f}},	//左上
			{{max,max,0.0f,1.0f},{1.0f,1.0f}},	//右下
			{{max,0.0f,0.0f,1.0f},{1.0f,0.0f}}		//右上
		};

		// 頂点バッファ①の定義
		D3D11_BUFFER_DESC xyzBufferDesc;
		xyzBufferDesc.Usage = D3D11_USAGE_DEFAULT;      // デフォルト使用法
		xyzBufferDesc.ByteWidth = sizeof(vertex);    // 8頂点
		xyzBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 頂点バッファ
		xyzBufferDesc.CPUAccessFlags = 0;
		xyzBufferDesc.MiscFlags = 0;
		xyzBufferDesc.StructureByteStride = 0;

		// 頂点バッファ①のサブリソースの定義
		D3D11_SUBRESOURCE_DATA xyzSubData;
		xyzSubData.pSysMem = vertex;  // バッファ・データの初期値
		xyzSubData.SysMemPitch = 0;
		xyzSubData.SysMemSlicePitch = 0;

		// 頂点バッファ①の作成
		HRESULT hr = GetDirectX::Device()->CreateBuffer(&xyzBufferDesc, &xyzSubData, &vVertexBuf);
		if (FAILED(hr))
			return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateBuffer", hr);
	}
}

/// <summary>
/// 2Dスプライトを描画するために必要な最もシンプルなシェーダをロードしておく
/// </summary>
/// <returns>成否</returns>
HRESULT cSprite2DDraw::CreateShader()
{
	const char* _filePath = "HLSL/Sprite2DDraw.hlsl";

	// **********************************************************
	// 頂点シェーダのコードをコンパイル
	ID3DBlob* pBlobVS = NULL;
	HRESULT hr = D3DX11CompileFromFile(
		_filePath,  // ファイル名
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
		&vpVertexShader); // 頂点シェーダを受け取る変数
						   //	SAFE_RELEASE(pBlobVS);  // バイト・コードを解放
	if (FAILED(hr)) {
		SAFE_RELEASE(pBlobVS);
		return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateVertexShader", hr);
	}

	// **********************************************************
	// 入力要素
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::XMFLOAT4), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// 入力レイアウト・オブジェクトの作成
	hr = GetDirectX::Device()->CreateInputLayout(
		layout,                            // 定義の配列
		_countof(layout),                  // 定義の要素数
		pBlobVS->GetBufferPointer(),       // バイト・コードへのポインタ
		pBlobVS->GetBufferSize(),          // バイト・コードのサイズ
		&vpInputLayout);                  // 受け取る変数のポインタ
	SAFE_RELEASE(pBlobVS);
	if (FAILED(hr))
		return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateInputLayout", hr);

	// **********************************************************
	// ピクセル・シェーダのコードをコンパイル
	ID3DBlob* pBlobPS = NULL;
	hr = D3DX11CompileFromFile(
		_filePath,  // ファイル名
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
		&vpPixelShader); // ピクセル・シェーダを受け取る変数
	SAFE_RELEASE(pBlobPS);  // バイト・コードを解放
	if (FAILED(hr))
		return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreatePixelShader", hr);
}
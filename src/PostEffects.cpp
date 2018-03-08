/*=================================================
//								FILE : PostEffects.cpp
//		ファイル説明 :
//		ポストエフェクト処理を扱いやすくするようなクラスを作成
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "PostEffects.h"
#include "Vertex.h"

ID3D11Buffer* cPostEffects::vVertexBuf = nullptr;			//頂点バッファ
ID3D11SamplerState* cPostEffects::m_pSampleLinear = nullptr;
ID3D11SamplerState* cPostEffects::m_pSamplePoint = nullptr;

/// <summary>
/// ポストエフェクトに使用する情報をプログラム開始後、一度だけ設定する
/// </summary>
/// <param name="_fileName"></param>
cPostEffects::cPostEffects(std::string _fileName)
{
	//まだ一度も読み込まれていない
	if (!vVertexBuf) {
		Create();		//頂点シェーダなどを作成する
	}
	m_Shader = SHADER::GetInstance()->LoadShaderFile(_fileName, inPOSITION | inTEX_UV, false);
}

/// <summary>
/// 解放処理
/// </summary>
cPostEffects::~cPostEffects()
{
}

/// <summary>
/// 指定したサンプラをセットする
/// </summary>
/// <param name="slot"></param>
/// <param name="flag"></param>
void cPostEffects::SetTexSamplerState(const int slot, Sampler flag)
{
	ID3D11SamplerState** samp = nullptr;

	switch (flag)
	{
	case cPostEffects::LINEAR:
		samp = &m_pSampleLinear;
		break;
	case cPostEffects::POINT:
		samp = &m_pSamplePoint;
		break;
	default:
		break;
	}

	GetDirectX::Context()->PSSetSamplers(slot, 1, samp);
}

/// <summary>
/// 描画
/// </summary>
/// <param name="_tex"></param>
/// <param name="_ResourceNum"></param>
void cPostEffects::Draw(ID3D11ShaderResourceView** _tex, int _ResourceNum)
{
	//まずは変換行列を作成する
	DirectX::XMFLOAT4X4 mvp;		//スクリーン空間への補正
	DirectX::XMFLOAT2 _LeftTop = {0,0};
	DirectX::XMFLOAT2 _RightDown = { (float)WINDOW_SIZE_X ,(float)WINDOW_SIZE_Y };

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
	mvp._43 = 0;
	mvp._44 = 1.0f;

	const float PosX = _LeftTop.x;
	const float PosY = _LeftTop.y;
	const float ScaleX = _RightDown.x;
	const float ScaleY = _RightDown.y;
	DirectX::XMMATRIX mw = DirectX::XMMatrixScaling(ScaleX, ScaleY, 0);		//サイズ1のポリゴンを指定サイズに拡大
	mw *= DirectX::XMMatrixTranslation(PosX, PosY, 0);								//指定位置に移動させる
	mw *= DirectX::XMLoadFloat4x4(&mvp);														//スクリーン空間に変換する
	DirectX::XMStoreFloat4x4(&m_ShaderConst.data, DirectX::XMMatrixTranspose(mw));

	m_Shader->Set();
	m_ShaderConst.Set(0);		//定数をシェーダにセットする

	//GET_CONTEXT->PSSetSamplers(0, 1, &m_pSampleLinear);

	//バーテックスバッファーをセット（バーテックスバッファーは一つでいい）
	UINT stride = sizeof(XYZUV);
	UINT offset = 0;
	GetDirectX::Context()->IASetVertexBuffers(0, 1, &vVertexBuf, &stride, &offset);

	GetDirectX::Context()->PSSetShaderResources(0, _ResourceNum, _tex);		//テクスチャセット
														//プリミティブ・トポロジーをセット
	GetDirectX::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//Draw
	GetDirectX::Context()->Draw(4, 0);
}

/// <summary>
/// 解放処理
/// </summary>
void cPostEffects::Destroy()
{
	SAFE_RELEASE(vVertexBuf);
	SAFE_RELEASE(m_pSampleLinear);
	SAFE_RELEASE(m_pSamplePoint);
}

/// <summary>
/// シェーダと定数をセット
/// </summary>
void cPostEffects::SetShader()
{
	m_Shader->Set();
	m_ShaderConst.Set(0);		//定数をシェーダにセットする
}

/// <summary>
/// プログラム開始後、一度だけ設定
/// </summary>
void cPostEffects::Create()
{
	CreateVertex();
	//テクスチャー用サンプラー作成
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));

	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	GetDirectX::Device()->CreateSamplerState(&SamDesc, &m_pSampleLinear);

	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	GetDirectX::Device()->CreateSamplerState(&SamDesc, &m_pSamplePoint);
}

/// <summary>
/// 頂点情報を取得
/// </summary>
/// <returns></returns>
HRESULT cPostEffects::CreateVertex()
{
	if (true) {
		const float max = 1.0f;
		XYZUV vertex[4] = {
			{ { 0.0f,max,0.0f,1.0f },{ 0.0f,1.0f } },	//左下
			{ { max,max,0.0f,1.0f },{ 1.0f,1.0f } },	//右下
			{ { 0.0f,0.0f,0.0f,1.0f },{ 0.0f,0.0f } },	//左上
			{ { max,0.0f,0.0f,1.0f },{ 1.0f,0.0f } }		//右上
		};
		// TODO 現在、テクスチャ描画時に向きが逆転するので反時計回りで定義している。将来的になんとかすること

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

/*=================================================
//								FILE : RenderTarget.cpp
//		ファイル説明 :
//		レンダーターゲットをクラスで扱う
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "RenderTarget.h"
#include "WinMain.h"

cRenderTargetTex::cRenderTargetTex()
{
	m_pDepthTexture = nullptr;
	m_pDepthRenderTargetView = nullptr;
	m_pDepthStencil = nullptr;
	m_pDepthStencilView = nullptr;
	m_pTextureView = nullptr;

	m_InitColor = { 0,0,0 };		//初期化カラーとりあえず標準は黒
}

cRenderTargetTex::~cRenderTargetTex()
{
	SAFE_RELEASE(m_pDepthTexture);
	SAFE_RELEASE(m_pDepthRenderTargetView);
	SAFE_RELEASE(m_pDepthStencil);
	SAFE_RELEASE(m_pDepthStencilView);
	SAFE_RELEASE(m_pTextureView);
}

HRESULT cRenderTargetTex::RTCreate(const float _width, const float _height, DirectX::XMUINT3 _initColor, DXGI_FORMAT _format)
{
	//一度だけ呼べばいい
	if (!m_pTextureView) {
		m_InitColor = _initColor;
		if (FAILED(RenderTex(_width, _height, _format))) {
			MessageBox(0, "レンダーターゲットテクスチャ作成失敗", NULL, MB_OK);
			return E_FAIL;
		}
		if (FAILED(DepthStencilView(_width, _height, _format))) {
			MessageBox(0, "レンダーターゲットテクスチャ作成失敗", NULL, MB_OK);
			return E_FAIL;
		}
		if (FAILED(ShaderResourceView(_width, _height, _format))) {
			MessageBox(0, "レンダーターゲットテクスチャ作成失敗", NULL, MB_OK);
			return E_FAIL;
		}

		//ビューポートの設定
		m_ViewPort.Width = _width;
		m_ViewPort.Height = _height;
		m_ViewPort.MinDepth = 0.0f;
		m_ViewPort.MaxDepth = 1.0f;
		m_ViewPort.TopLeftX = 0;
		m_ViewPort.TopLeftY = 0;
	}
	return S_OK;
}

void cRenderTargetTex::SetRenderTargetTex(const int _pathNo)
{
	GetDirectX::InitOMSetRenderTargets();
	GetDirectX::Context()->RSSetViewports(1, &m_ViewPort);
	GetDirectX::Context()->OMSetRenderTargets(1, &m_pDepthRenderTargetView, m_pDepthStencilView);
	float ClearColor[4] = { (float)m_InitColor.x / 255.0f,(float)m_InitColor.y / 255.0f ,(float)m_InitColor.z / 255.0f ,0.0f };
	GetDirectX::Context()->ClearRenderTargetView(m_pDepthRenderTargetView, ClearColor);//画面クリア
	GetDirectX::Context()->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);//深度バッファクリア
}

RTTex cRenderTargetTex::GetRenderTextureStruct()
{
	//レンダリング設定を構造体に入れて返す
	RTTex _TexStruct;
	_TexStruct.m_pDepthRenderTargetView = m_pDepthRenderTargetView;
	_TexStruct.m_pDepthStencilView = m_pDepthStencilView;
	_TexStruct.m_pTextureView = m_pTextureView;
	_TexStruct.m_ViewPort = m_ViewPort;
	_TexStruct.ClereColor[0] = (float)m_InitColor.x / 255.0f;
	_TexStruct.ClereColor[1] = (float)m_InitColor.y / 255.0f;
	_TexStruct.ClereColor[2] = (float)m_InitColor.z / 255.0f;
	_TexStruct.ClereColor[3] = 0.0f;
	return _TexStruct;
}

ID3D11ShaderResourceView * cRenderTargetTex::GetTextureResourceView()
{
	return m_pTextureView;
}

ID3D11DepthStencilView * cRenderTargetTex::GetDepthStencilView()
{
	return m_pDepthStencilView;
}

DirectXDrawData cRenderTargetTex::GetDrawData()
{
	DirectXDrawData data;
	data.v_pDepthStencilView = m_pDepthStencilView;
	data.v_pRenderTargetView = m_pDepthRenderTargetView;
	data.v_ViewPort = m_ViewPort;
	data.v_ShaderResourceView = m_pTextureView;

	return data;
}


//===================================================================================//
HRESULT cRenderTargetTex::RenderTex(const float _width, const float _height, DXGI_FORMAT _format)
{
	D3D11_TEXTURE2D_DESC tdesc;
	ZeroMemory(&tdesc, sizeof(D3D11_TEXTURE2D_DESC));
	tdesc.Width = _width;
	tdesc.Height = _height;
	tdesc.MipLevels = 1;
	tdesc.ArraySize = 1;
	tdesc.MiscFlags = 0;
	tdesc.Format = _format;
	tdesc.SampleDesc.Count = 1;
	tdesc.SampleDesc.Quality = 0;
	tdesc.Usage = D3D11_USAGE_DEFAULT;
	tdesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	tdesc.CPUAccessFlags = 0;

	GetDirectX::Device()->CreateTexture2D(&tdesc, NULL, &m_pDepthTexture);


	//深度テクスチャ用　レンダーターゲットビュー作成    （レンダリング時用）
	D3D11_RENDER_TARGET_VIEW_DESC DescRT;
	DescRT.Format = tdesc.Format;
	DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	DescRT.Texture2D.MipSlice = 0;

	///3で作ったテクスチャを設定する。
	if (FAILED(GetDirectX::Device()->CreateRenderTargetView(m_pDepthTexture, &DescRT, &m_pDepthRenderTargetView)))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT cRenderTargetTex::DepthStencilView(const float _width, const float _height, DXGI_FORMAT _format)
{
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = _width;
	descDepth.Height = _height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	//descDepth.Format = DXGI_FORMAT_R8G8B8A8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	GetDirectX::Device()->CreateTexture2D(&descDepth, NULL, &m_pDepthStencil);
	GetDirectX::Device()->CreateDepthStencilView(m_pDepthStencil, NULL, &m_pDepthStencilView);

	return S_OK;
}

HRESULT cRenderTargetTex::ShaderResourceView(const float _width, const float _height, DXGI_FORMAT _format)
{
	//深度テクスチャ用　シェーダーリソースビュー作成    
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = _format;
	//SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	if (FAILED(GetDirectX::Device()->CreateShaderResourceView(m_pDepthTexture, &SRVDesc, &m_pTextureView)))
	{
		return E_FAIL;
	}
	return S_OK;
}

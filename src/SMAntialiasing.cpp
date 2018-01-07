#include "SMAntialiasing.h"
#include "AreaTex.h"
#include "SearchTex.h"

cSMAntialiasing::cSMAntialiasing()
{
	//事前計算テクスチャを作成する
	CreateSMAATexture();

	m_pEdge = new cSMAAEdge;
	m_pBlendingWeight = new cSMAABlendingWeight;
	m_pBlending = new cSMAABlending;
}

cSMAntialiasing::~cSMAntialiasing()
{
	SAFE_RELEASE(pAreaSRV);
	SAFE_RELEASE(pAreaTex);
	SAFE_RELEASE(pSearchTex);
	SAFE_RELEASE(pSearchSRV);

	delete m_pEdge;
	delete m_pBlendingWeight;
	delete m_pBlending;
}

void cSMAntialiasing::DrawSMAA(ID3D11ShaderResourceView * _color)
{
	m_pEdge->DrawEdge(_color);
	m_pBlendingWeight->DrawBlendingWeight(m_pEdge->GetResourceView(), pAreaSRV, pSearchSRV);
	m_pBlending->DrawBlending(_color, m_pBlendingWeight->GetResourceView());
}

ID3D11ShaderResourceView * cSMAntialiasing::Get()
{
	return m_pBlending->GetResourceView();
}

HRESULT cSMAntialiasing::CreateSMAATexture()
{
	// 事前計算のテクスチャ読み込み（ヘッダファイルで定義）
	{//AreaTex.h
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = areaTexBytes;
		data.SysMemPitch = AREATEX_PITCH;
		data.SysMemSlicePitch = 0;

		D3D11_TEXTURE2D_DESC descTex;
		descTex.Width = AREATEX_WIDTH;
		descTex.Height = AREATEX_HEIGHT;
		descTex.MipLevels = descTex.ArraySize = 1;
		descTex.Format = DXGI_FORMAT_R8G8_UNORM;
		descTex.SampleDesc.Count = 1;
		descTex.SampleDesc.Quality = 0;
		descTex.Usage = D3D11_USAGE_DEFAULT;
		descTex.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		descTex.CPUAccessFlags = 0;
		descTex.MiscFlags = 0;
		if (FAILED(GetDirectX::Device()->CreateTexture2D(&descTex, &data, &pAreaTex))) {
			MessageBox(0, "SMAA事前計算テクスチャ作成失敗", NULL, MB_OK);
			return E_FAIL;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC descSRV;
		descSRV.Format = descTex.Format;
		descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		descSRV.Texture2D.MostDetailedMip = 0;
		descSRV.Texture2D.MipLevels = 1;

		if (FAILED(GetDirectX::Device()->CreateShaderResourceView(pAreaTex, &descSRV, &pAreaSRV))) {
			MessageBox(0, "SMAA事前計算テクスチャ作成失敗", NULL, MB_OK);
			return E_FAIL;
		}

	}
	{//SearchTex.h
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = searchTexBytes;
		data.SysMemPitch = SEARCHTEX_PITCH;
		data.SysMemSlicePitch = 0;

		D3D11_TEXTURE2D_DESC descTex;
		descTex.Width = SEARCHTEX_WIDTH;
		descTex.Height = SEARCHTEX_HEIGHT;
		descTex.MipLevels = descTex.ArraySize = 1;
		descTex.Format = DXGI_FORMAT_R8_UNORM;
		descTex.SampleDesc.Count = 1;
		descTex.SampleDesc.Quality = 0;
		descTex.Usage = D3D11_USAGE_DEFAULT;
		descTex.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		descTex.CPUAccessFlags = 0;
		descTex.MiscFlags = 0;
		if (FAILED(GetDirectX::Device()->CreateTexture2D(&descTex, &data, &pSearchTex))) {
			MessageBox(0, "SMAA事前計算テクスチャ作成失敗", NULL, MB_OK);
			return E_FAIL;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC descSRV;
		descSRV.Format = descTex.Format;
		descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		descSRV.Texture2D.MostDetailedMip = 0;
		descSRV.Texture2D.MipLevels = 1;
		if (FAILED(GetDirectX::Device()->CreateShaderResourceView(pSearchTex, &descSRV, &pSearchSRV))) {
			MessageBox(0, "SMAA事前計算テクスチャ作成失敗", NULL, MB_OK);
			return E_FAIL;
		}
	}
}

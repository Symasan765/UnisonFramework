/*=================================================
//								FILE : BillBoard.cpp
//		ファイル説明 :
//		3D空間上で常にカメラを向き続ける2Dテクスチャを表示する
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "BillBoard.h"
#include "Vertex.h"

ID3D11Buffer* cBillBoard::m_VertexBuf = nullptr;

cBillBoard::cBillBoard()
{
	//このクラスが呼び出される最初だけ設定する
	if (m_VertexBuf == nullptr) {
		CreateVertexBuf();
	}
	m_ShaderType = 0;
	m_Constant.SetWorldMatrix(GetWorldMatrix());
}

cBillBoard::cBillBoard(std::string _fileName)
{
	//このクラスが呼び出される最初だけ設定する
	if (m_VertexBuf == nullptr) {
		CreateVertexBuf();
	}
	m_ShaderType = 0;
	m_Constant.SetWorldMatrix(GetWorldMatrix());
	LoadData(_fileName);
}

void cBillBoard::LoadData(std::string _fileName)
{
	m_pTex = cTexManager::GetInstance().LoadTexData(_fileName);
}

void cBillBoard::Draw()
{
	//m_pMesh->Draw();		//標準の描画使うとシェーダセットが出来ないから使わない方向かな…？

	SetConstant();

	//バーテックスバッファーをセット（バーテックスバッファーは一つでいい）
	UINT stride = sizeof(VERTEX3D);
	UINT offset = 0;
	GetDirectX::Context()->IASetVertexBuffers(0, 1, &m_VertexBuf, &stride, &offset);

	//テクスチャーをシェーダーに渡す
	if (m_pTex != nullptr)
	{
		GetDirectX::Context()->PSSetShaderResources(0, 1, &m_pTex->vpTexture);
		GetDirectX::Context()->VSSetShaderResources(0, 1, &m_pTex->vpTexture);
	}

	//プリミティブ・トポロジーをセット
	GetDirectX::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//Draw
	GetDirectX::Context()->Draw(4, 0);

}

void cBillBoard::SetEmission(DirectX::XMFLOAT4 _color)
{
	m_Constant.SetEmission(_color);
}

HRESULT cBillBoard::CreateVertexBuf()
{
	const float max = 1.0f;
	VERTEX3D vertex[4] = {
		{ { -max,max,0.0f,1.0f },{0.0f,0.0f,-1.0f,0.0f}, { 0.0f,1.0f },{ 128,128,128,128 } ,{ 0.0f,0.0f,0.0f,0.0f } },	//左下
		{ { -max,-max,0.0f,1.0f },{ 0.0f,0.0f,-1.0f,0.0f },{ 0.0f,0.0f },{ 128,128,128,128 } ,{ 0.0f,0.0f,0.0f,0.0f } },	//左上
		{ { max,max,0.0f,1.0f },{ 0.0f,0.0f,-1.0f,0.0f },{ 1.0f,1.0f },{ 128,128,128,128 } ,{ 0.0f,0.0f,0.0f,0.0f } },	//右下
		{ { max,-max,0.0f,1.0f },{ 0.0f,0.0f,-1.0f,0.0f },{ 1.0f,0.0f },{ 128,128,128,128 } ,{ 0.0f,0.0f,0.0f,0.0f } }		//右上
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
	HRESULT hr = GetDirectX::Device()->CreateBuffer(&xyzBufferDesc, &xyzSubData, &m_VertexBuf);
	if (FAILED(hr))
		return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateBuffer", hr);
}

void cBillBoard::SetConstant()
{
	//ビルボードのためのワールド行列を作成する
	DirectX::XMFLOAT3 Vec = { 0,0,0 };		//視点ベクトル
	DirectX::XMFLOAT4X4 W = GetWorldMatrix();	//本来のワールド行列
	const CameraData Camera = m_Constant.GetCameraData();	//カメラの情報を取得する
	Vec = { Camera.vLook.x - Camera.vPos.x, Camera.vLook.y - Camera.vPos.y ,Camera.vLook.z - Camera.vPos.z };
	DirectX::XMMATRIX Mat = DirectX::XMMatrixLookAtLH({ 0,0,0 }, DirectX::XMLoadFloat3(&Vec), DirectX::XMLoadFloat3(&Camera.vUp));
	Mat = DirectX::XMMatrixInverse(NULL, Mat);
	Mat *= DirectX::XMLoadFloat4x4(&W);
	DirectX::XMStoreFloat4x4(&W, Mat);

	//ワールド行列を保存
	m_Constant.SetWorldMatrix(W);		//ここで渡す行列を工夫する
	m_Constant.SetShaderType(m_ShaderType);
	m_Constant.SetShader();
}

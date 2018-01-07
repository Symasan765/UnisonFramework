/*=================================================
//								FILE : ConstantBuffer.h
//		ファイル説明 :
//		コンスタントバッファを作成して利用できるようにするクラス
//
//							HAL大阪 : 松本 雄之介
=================================================*/
#pragma once
#include "WinMain.h"

/// <summary>
/// 定数バッファを取り扱うテンプレートクラス
/// </summary>
template <typename T> class cConstBuf {
public:
	friend class cDirectX11;
	cConstBuf();				//コンストラクタで定数バッファ作る
	~cConstBuf();				//デストラクタで解放
	void Set(const unsigned slot);
	
	T data;				//ここにデータを格納する
private:
	bool SetConstBuf();	//現在格納されているデータをバッファに格納し、シェーダへセットする
	ID3D11Buffer* m_ConstBuffer;				//定数バッファ本体
};

template<typename T>
inline cConstBuf<T>::cConstBuf()
{
	m_ConstBuffer = nullptr;
	memset(&data, 0, sizeof(T));

	D3D11_BUFFER_DESC cb1;
	cb1.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb1.ByteWidth = sizeof(T);
	cb1.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb1.MiscFlags = 0;
	cb1.StructureByteStride = 0;
	cb1.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(GetDirectX::Device()->CreateBuffer(&cb1, NULL, &m_ConstBuffer)))
	{
		MessageBox(0, "定数バッファ作成失敗", NULL, MB_OK);
	}
}

template<typename T>
inline cConstBuf<T>::~cConstBuf()
{
	SAFE_RELEASE(m_ConstBuffer);
}

template<typename T>
inline void cConstBuf<T>::Set(const unsigned slot)
{
	SetConstBuf();
	GetDirectX::Context()->VSSetConstantBuffers(slot, 1, &m_ConstBuffer);
	GetDirectX::Context()->GSSetConstantBuffers(slot, 1, &m_ConstBuffer);
	GetDirectX::Context()->HSSetConstantBuffers(slot, 1, &m_ConstBuffer);
	GetDirectX::Context()->DSSetConstantBuffers(slot, 1, &m_ConstBuffer);
	GetDirectX::Context()->PSSetConstantBuffers(slot, 1, &m_ConstBuffer);
}

template<typename T>
inline bool cConstBuf<T>::SetConstBuf()
{
	bool succes = false;
	D3D11_MAPPED_SUBRESOURCE pData1;
	if (SUCCEEDED(GetDirectX::Context()->Map(m_ConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData1)))
	{
		memcpy_s(pData1.pData, pData1.RowPitch, (void*)(&data), sizeof(T));
		succes = true;
	}
	GetDirectX::Context()->Unmap(m_ConstBuffer, 0);

	return succes;
}

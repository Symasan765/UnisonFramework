/*=================================================
//								FILE : ConstantBuffer.h
//		�t�@�C������ :
//		�R���X�^���g�o�b�t�@���쐬���ė��p�ł���悤�ɂ���N���X
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#pragma once
#include "WinMain.h"

/// <summary>
/// �萔�o�b�t�@����舵���e���v���[�g�N���X
/// </summary>
template <typename T> class cConstBuf {
public:
	friend class cDirectX11;
	cConstBuf();				//�R���X�g���N�^�Œ萔�o�b�t�@���
	~cConstBuf();				//�f�X�g���N�^�ŉ��
	void Set(const unsigned slot);
	
	T data;				//�����Ƀf�[�^���i�[����
private:
	bool SetConstBuf();	//���݊i�[����Ă���f�[�^���o�b�t�@�Ɋi�[���A�V�F�[�_�փZ�b�g����
	ID3D11Buffer* m_ConstBuffer;				//�萔�o�b�t�@�{��
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
		MessageBox(0, "�萔�o�b�t�@�쐬���s", NULL, MB_OK);
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

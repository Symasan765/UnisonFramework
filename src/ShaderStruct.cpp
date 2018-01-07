/*=================================================
//								FILE : ShaderStruct.cpp
//		ファイル説明 :
//		シェーダで利用する定数バッファ構造体を定義しておくファイル
//		主に格納しやすくするメンバ変数を記述する
//							HAL大阪 : 松本 雄之介
=================================================*/
#include "ShaderStruct.h"

void MatrixData::Transpose() {

}

void MatrixData::SetWorldMatrix(const DirectX::XMMATRIX& mat) {
	DirectX::XMMATRIX world = XMMatrixTranspose(mat);		//まず転置する
	DirectX::XMStoreFloat4x4(&mW, world);
}

void MatrixData::SetWVPMatrix(const DirectX::XMMATRIX& mat) {
	DirectX::XMMATRIX worldView = XMMatrixTranspose(mat);		//まず転置する
	DirectX::XMStoreFloat4x4(&mWVP, worldView);
}

void MatrixData::SetW_VPMatrix(const DirectX::XMFLOAT4X4& w, const DirectX::XMFLOAT4X4& vp)
{
	DirectX::XMMATRIX _w = DirectX::XMLoadFloat4x4(&w);
	DirectX::XMMATRIX _vp = DirectX::XMLoadFloat4x4(&vp);

	_w *= _vp;

	_w = XMMatrixTranspose(_w);
	DirectX::XMStoreFloat4x4(&mWVP, _w);
}

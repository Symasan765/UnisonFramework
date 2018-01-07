/*=================================================
//								FILE : ShaderStruct.cpp
//		�t�@�C������ :
//		�V�F�[�_�ŗ��p����萔�o�b�t�@�\���̂��`���Ă����t�@�C��
//		��Ɋi�[���₷�����郁���o�ϐ����L�q����
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "ShaderStruct.h"

void MatrixData::Transpose() {

}

void MatrixData::SetWorldMatrix(const DirectX::XMMATRIX& mat) {
	DirectX::XMMATRIX world = XMMatrixTranspose(mat);		//�܂��]�u����
	DirectX::XMStoreFloat4x4(&mW, world);
}

void MatrixData::SetWVPMatrix(const DirectX::XMMATRIX& mat) {
	DirectX::XMMATRIX worldView = XMMatrixTranspose(mat);		//�܂��]�u����
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

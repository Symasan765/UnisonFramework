/*=================================================
//								FILE : BillBoard.cpp
//		�t�@�C������ :
//		3D��ԏ�ŏ�ɃJ����������������2D�e�N�X�`����\������
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "BillBoard.h"
#include "Vertex.h"

ID3D11Buffer* cBillBoard::m_VertexBuf = nullptr;

cBillBoard::cBillBoard()
{
	//���̃N���X���Ăяo�����ŏ������ݒ肷��
	if (m_VertexBuf == nullptr) {
		CreateVertexBuf();
	}
	m_ShaderType = 0;
	m_Constant.SetWorldMatrix(GetWorldMatrix());
}

cBillBoard::cBillBoard(std::string _fileName)
{
	//���̃N���X���Ăяo�����ŏ������ݒ肷��
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
	//m_pMesh->Draw();		//�W���̕`��g���ƃV�F�[�_�Z�b�g���o���Ȃ�����g��Ȃ��������ȁc�H

	SetConstant();

	//�o�[�e�b�N�X�o�b�t�@�[���Z�b�g�i�o�[�e�b�N�X�o�b�t�@�[�͈�ł����j
	UINT stride = sizeof(VERTEX3D);
	UINT offset = 0;
	GetDirectX::Context()->IASetVertexBuffers(0, 1, &m_VertexBuf, &stride, &offset);

	//�e�N�X�`���[���V�F�[�_�[�ɓn��
	if (m_pTex != nullptr)
	{
		GetDirectX::Context()->PSSetShaderResources(0, 1, &m_pTex->vpTexture);
		GetDirectX::Context()->VSSetShaderResources(0, 1, &m_pTex->vpTexture);
	}

	//�v���~�e�B�u�E�g�|���W�[���Z�b�g
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
		{ { -max,max,0.0f,1.0f },{0.0f,0.0f,-1.0f,0.0f}, { 0.0f,1.0f },{ 128,128,128,128 } ,{ 0.0f,0.0f,0.0f,0.0f } },	//����
		{ { -max,-max,0.0f,1.0f },{ 0.0f,0.0f,-1.0f,0.0f },{ 0.0f,0.0f },{ 128,128,128,128 } ,{ 0.0f,0.0f,0.0f,0.0f } },	//����
		{ { max,max,0.0f,1.0f },{ 0.0f,0.0f,-1.0f,0.0f },{ 1.0f,1.0f },{ 128,128,128,128 } ,{ 0.0f,0.0f,0.0f,0.0f } },	//�E��
		{ { max,-max,0.0f,1.0f },{ 0.0f,0.0f,-1.0f,0.0f },{ 1.0f,0.0f },{ 128,128,128,128 } ,{ 0.0f,0.0f,0.0f,0.0f } }		//�E��
	};

	// ���_�o�b�t�@�@�̒�`
	D3D11_BUFFER_DESC xyzBufferDesc;
	xyzBufferDesc.Usage = D3D11_USAGE_DEFAULT;      // �f�t�H���g�g�p�@
	xyzBufferDesc.ByteWidth = sizeof(vertex);    // 8���_
	xyzBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // ���_�o�b�t�@
	xyzBufferDesc.CPUAccessFlags = 0;
	xyzBufferDesc.MiscFlags = 0;
	xyzBufferDesc.StructureByteStride = 0;

	// ���_�o�b�t�@�@�̃T�u���\�[�X�̒�`
	D3D11_SUBRESOURCE_DATA xyzSubData;
	xyzSubData.pSysMem = vertex;  // �o�b�t�@�E�f�[�^�̏����l
	xyzSubData.SysMemPitch = 0;
	xyzSubData.SysMemSlicePitch = 0;

	// ���_�o�b�t�@�@�̍쐬
	HRESULT hr = GetDirectX::Device()->CreateBuffer(&xyzBufferDesc, &xyzSubData, &m_VertexBuf);
	if (FAILED(hr))
		return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateBuffer", hr);
}

void cBillBoard::SetConstant()
{
	//�r���{�[�h�̂��߂̃��[���h�s����쐬����
	DirectX::XMFLOAT3 Vec = { 0,0,0 };		//���_�x�N�g��
	DirectX::XMFLOAT4X4 W = GetWorldMatrix();	//�{���̃��[���h�s��
	const CameraData Camera = m_Constant.GetCameraData();	//�J�����̏����擾����
	Vec = { Camera.vLook.x - Camera.vPos.x, Camera.vLook.y - Camera.vPos.y ,Camera.vLook.z - Camera.vPos.z };
	DirectX::XMMATRIX Mat = DirectX::XMMatrixLookAtLH({ 0,0,0 }, DirectX::XMLoadFloat3(&Vec), DirectX::XMLoadFloat3(&Camera.vUp));
	Mat = DirectX::XMMatrixInverse(NULL, Mat);
	Mat *= DirectX::XMLoadFloat4x4(&W);
	DirectX::XMStoreFloat4x4(&W, Mat);

	//���[���h�s���ۑ�
	m_Constant.SetWorldMatrix(W);		//�����œn���s����H�v����
	m_Constant.SetShaderType(m_ShaderType);
	m_Constant.SetShader();
}

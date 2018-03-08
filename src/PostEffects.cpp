/*=================================================
//								FILE : PostEffects.cpp
//		�t�@�C������ :
//		�|�X�g�G�t�F�N�g�����������₷������悤�ȃN���X���쐬
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "PostEffects.h"
#include "Vertex.h"

ID3D11Buffer* cPostEffects::vVertexBuf = nullptr;			//���_�o�b�t�@
ID3D11SamplerState* cPostEffects::m_pSampleLinear = nullptr;
ID3D11SamplerState* cPostEffects::m_pSamplePoint = nullptr;

/// <summary>
/// �|�X�g�G�t�F�N�g�Ɏg�p��������v���O�����J�n��A��x�����ݒ肷��
/// </summary>
/// <param name="_fileName"></param>
cPostEffects::cPostEffects(std::string _fileName)
{
	//�܂���x���ǂݍ��܂�Ă��Ȃ�
	if (!vVertexBuf) {
		Create();		//���_�V�F�[�_�Ȃǂ��쐬����
	}
	m_Shader = SHADER::GetInstance()->LoadShaderFile(_fileName, inPOSITION | inTEX_UV, false);
}

/// <summary>
/// �������
/// </summary>
cPostEffects::~cPostEffects()
{
}

/// <summary>
/// �w�肵���T���v�����Z�b�g����
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
/// �`��
/// </summary>
/// <param name="_tex"></param>
/// <param name="_ResourceNum"></param>
void cPostEffects::Draw(ID3D11ShaderResourceView** _tex, int _ResourceNum)
{
	//�܂��͕ϊ��s����쐬����
	DirectX::XMFLOAT4X4 mvp;		//�X�N���[����Ԃւ̕␳
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
	DirectX::XMMATRIX mw = DirectX::XMMatrixScaling(ScaleX, ScaleY, 0);		//�T�C�Y1�̃|���S�����w��T�C�Y�Ɋg��
	mw *= DirectX::XMMatrixTranslation(PosX, PosY, 0);								//�w��ʒu�Ɉړ�������
	mw *= DirectX::XMLoadFloat4x4(&mvp);														//�X�N���[����Ԃɕϊ�����
	DirectX::XMStoreFloat4x4(&m_ShaderConst.data, DirectX::XMMatrixTranspose(mw));

	m_Shader->Set();
	m_ShaderConst.Set(0);		//�萔���V�F�[�_�ɃZ�b�g����

	//GET_CONTEXT->PSSetSamplers(0, 1, &m_pSampleLinear);

	//�o�[�e�b�N�X�o�b�t�@�[���Z�b�g�i�o�[�e�b�N�X�o�b�t�@�[�͈�ł����j
	UINT stride = sizeof(XYZUV);
	UINT offset = 0;
	GetDirectX::Context()->IASetVertexBuffers(0, 1, &vVertexBuf, &stride, &offset);

	GetDirectX::Context()->PSSetShaderResources(0, _ResourceNum, _tex);		//�e�N�X�`���Z�b�g
														//�v���~�e�B�u�E�g�|���W�[���Z�b�g
	GetDirectX::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//Draw
	GetDirectX::Context()->Draw(4, 0);
}

/// <summary>
/// �������
/// </summary>
void cPostEffects::Destroy()
{
	SAFE_RELEASE(vVertexBuf);
	SAFE_RELEASE(m_pSampleLinear);
	SAFE_RELEASE(m_pSamplePoint);
}

/// <summary>
/// �V�F�[�_�ƒ萔���Z�b�g
/// </summary>
void cPostEffects::SetShader()
{
	m_Shader->Set();
	m_ShaderConst.Set(0);		//�萔���V�F�[�_�ɃZ�b�g����
}

/// <summary>
/// �v���O�����J�n��A��x�����ݒ�
/// </summary>
void cPostEffects::Create()
{
	CreateVertex();
	//�e�N�X�`���[�p�T���v���[�쐬
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
/// ���_�����擾
/// </summary>
/// <returns></returns>
HRESULT cPostEffects::CreateVertex()
{
	if (true) {
		const float max = 1.0f;
		XYZUV vertex[4] = {
			{ { 0.0f,max,0.0f,1.0f },{ 0.0f,1.0f } },	//����
			{ { max,max,0.0f,1.0f },{ 1.0f,1.0f } },	//�E��
			{ { 0.0f,0.0f,0.0f,1.0f },{ 0.0f,0.0f } },	//����
			{ { max,0.0f,0.0f,1.0f },{ 1.0f,0.0f } }		//�E��
		};
		// TODO ���݁A�e�N�X�`���`�掞�Ɍ������t�]����̂Ŕ����v���Œ�`���Ă���B�����I�ɂȂ�Ƃ����邱��

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
		HRESULT hr = GetDirectX::Device()->CreateBuffer(&xyzBufferDesc, &xyzSubData, &vVertexBuf);
		if (FAILED(hr))
			return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateBuffer", hr);
	}
}

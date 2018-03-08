/*=================================================
//								FILE :�@SpriteDraw.cpp
//		�t�@�C������ :
//		�QD�X�v���C�g�̕`����s��
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "SpriteDraw.h"
#include "Vertex.h"
#include "WinMain.h"
#include "CompileOptions.h"

using namespace CompileOptions;

/// <summary>
/// �X�v���C�g�̕`����s��
/// </summary>
/// <param name="_tex">�`�悷��e�N�X�`��</param>
/// <param name="_LeftTop">�E��̍��W(�X�N���[���X�y�[�X)</param>
/// <param name="_RightDown">�����̍��W(�X�N���[���X�y�[�X)</param>
void cSprite2DDraw::Draw(ID3D11ShaderResourceView * _tex, DirectX::XMFLOAT2 _LeftTop, DirectX::XMFLOAT2 _RightDown)
{
	//�܂��͕ϊ��s����쐬����
	DirectX::XMFLOAT4X4 mvp;		//�X�N���[����Ԃւ̕␳

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
	mvp._43 = m_NowDepth;
	mvp._44 = 1.0f;

	const float PosX = _LeftTop.x;
	const float PosY = _LeftTop.y;
	const float ScaleX = _RightDown.x;
	const float ScaleY = _RightDown.y;
	DirectX::XMMATRIX mw = DirectX::XMMatrixScaling(ScaleX, ScaleY, 0);		//�T�C�Y1�̃|���S�����w��T�C�Y�Ɋg��
	mw *= DirectX::XMMatrixTranslation(PosX, PosY, 0);								//�w��ʒu�Ɉړ�������
	mw *= DirectX::XMLoadFloat4x4(&mvp);														//�X�N���[����Ԃɕϊ�����
	DirectX::XMStoreFloat4x4(&m_ShaderConst.data, DirectX::XMMatrixTranspose(mw));
	
	m_ShaderConst.Set(0);		//�萔���V�F�[�_�ɃZ�b�g����

	GetDirectX::Context()->IASetInputLayout(vpInputLayout);
	GetDirectX::Context()->VSSetShader(vpVertexShader, NULL, 0);
	GetDirectX::Context()->GSSetShader(NULL, NULL, 0);
	GetDirectX::Context()->PSSetShader(vpPixelShader, NULL, 0);

	GetDirectX::Context()->PSSetSamplers(0, 1, &m_pSampleLinear);

	//�o�[�e�b�N�X�o�b�t�@�[���Z�b�g�i�o�[�e�b�N�X�o�b�t�@�[�͈�ł����j
	UINT stride = sizeof(XYZUV);
	UINT offset = 0;
	GetDirectX::Context()->IASetVertexBuffers(0, 1, &vVertexBuf, &stride, &offset);

	GetDirectX::Context()->PSSetShaderResources(0, 1, &_tex);		//�e�N�X�`���Z�b�g
	//�v���~�e�B�u�E�g�|���W�[���Z�b�g
	GetDirectX::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//Draw
	GetDirectX::Context()->Draw(4, 0);

	m_NowDepth -= 0.001f;		//�X�v���C�g�̕`���O�ʂɂ��炷
}

/// <summary>
/// ������
/// </summary>
cSprite2DDraw::cSprite2DDraw()
{
	vpVertexShader = nullptr;
	vpPixelShader = nullptr;
	vpInputLayout = nullptr;
	vVertexBuf = nullptr;			//���_�o�b�t�@

	Create();
}

/// <summary>
/// �������
/// </summary>
cSprite2DDraw::~cSprite2DDraw()
{
	SAFE_RELEASE(vpVertexShader);
	SAFE_RELEASE(vpPixelShader);
	SAFE_RELEASE(vpInputLayout);
	SAFE_RELEASE(vVertexBuf);
}

/// <summary>
/// �`��ɗ��p����f�[�^�Q���쐬����(���������ɂ̂ݍ쐬)
/// </summary>
void cSprite2DDraw::Create()
{
	CreateVertex();
	CreateShader();
	//�e�N�X�`���[�p�T���v���[�쐬
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));

	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	GetDirectX::Device()->CreateSamplerState(&SamDesc, &m_pSampleLinear);
}

/// <summary>
/// ���_���쐬
/// </summary>
/// <returns></returns>
HRESULT cSprite2DDraw::CreateVertex()
{
	if (true) {
		const float max = 1.0f;
		XYZUV vertex[4] = {
			{{0.0f,max,0.0f,1.0f},{0.0f,1.0f}},	//����
			{{0.0f,0.0f,0.0f,1.0f},{0.0f,0.0f}},	//����
			{{max,max,0.0f,1.0f},{1.0f,1.0f}},	//�E��
			{{max,0.0f,0.0f,1.0f},{1.0f,0.0f}}		//�E��
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
		HRESULT hr = GetDirectX::Device()->CreateBuffer(&xyzBufferDesc, &xyzSubData, &vVertexBuf);
		if (FAILED(hr))
			return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateBuffer", hr);
	}
}

/// <summary>
/// 2D�X�v���C�g��`�悷�邽�߂ɕK�v�ȍł��V���v���ȃV�F�[�_�����[�h���Ă���
/// </summary>
/// <returns>����</returns>
HRESULT cSprite2DDraw::CreateShader()
{
	const char* _filePath = "HLSL/Sprite2DDraw.hlsl";

	// **********************************************************
	// ���_�V�F�[�_�̃R�[�h���R���p�C��
	ID3DBlob* pBlobVS = NULL;
	HRESULT hr = D3DX11CompileFromFile(
		_filePath,  // �t�@�C����
		NULL,          // �}�N����`(�Ȃ�)
		NULL,          // �C���N���[�h�E�t�@�C����`(�Ȃ�)
		"VS",          // �uVS�֐��v���V�F�[�_������s�����
		"vs_5_0",      // ���_�V�F�[�_
		g_flagCompile, // �R���p�C���E�I�v�V����
		0,             // �G�t�F�N�g�̃R���p�C���E�I�v�V����(�Ȃ�)
		NULL,          // �����ɃR���p�C�����Ă���֐��𔲂���B
		&pBlobVS,      // �R���p�C�����ꂽ�o�C�g�E�R�[�h
		NULL,          // �G���[���b�Z�[�W�͕s�v
		NULL);         // �߂�l
	if (FAILED(hr))
		return DXTRACE_ERR("InitDirect3D D3DX11CompileShaderFromFile", hr);

	// ���_�V�F�[�_�̍쐬
	hr = GetDirectX::Device()->CreateVertexShader(
		pBlobVS->GetBufferPointer(), // �o�C�g�E�R�[�h�ւ̃|�C���^
		pBlobVS->GetBufferSize(),    // �o�C�g�E�R�[�h�̒���
		NULL,
		&vpVertexShader); // ���_�V�F�[�_���󂯎��ϐ�
						   //	SAFE_RELEASE(pBlobVS);  // �o�C�g�E�R�[�h�����
	if (FAILED(hr)) {
		SAFE_RELEASE(pBlobVS);
		return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateVertexShader", hr);
	}

	// **********************************************************
	// ���͗v�f
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::XMFLOAT4), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// ���̓��C�A�E�g�E�I�u�W�F�N�g�̍쐬
	hr = GetDirectX::Device()->CreateInputLayout(
		layout,                            // ��`�̔z��
		_countof(layout),                  // ��`�̗v�f��
		pBlobVS->GetBufferPointer(),       // �o�C�g�E�R�[�h�ւ̃|�C���^
		pBlobVS->GetBufferSize(),          // �o�C�g�E�R�[�h�̃T�C�Y
		&vpInputLayout);                  // �󂯎��ϐ��̃|�C���^
	SAFE_RELEASE(pBlobVS);
	if (FAILED(hr))
		return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateInputLayout", hr);

	// **********************************************************
	// �s�N�Z���E�V�F�[�_�̃R�[�h���R���p�C��
	ID3DBlob* pBlobPS = NULL;
	hr = D3DX11CompileFromFile(
		_filePath,  // �t�@�C����
		NULL,          // �}�N����`(�Ȃ�)
		NULL,          // �C���N���[�h�E�t�@�C����`(�Ȃ�)
		"PS",          // �uPS�֐��v���V�F�[�_������s�����
		"ps_5_0",      // �s�N�Z���E�V�F�[�_
		g_flagCompile, // �R���p�C���E�I�v�V����
		0,             // �G�t�F�N�g�̃R���p�C���E�I�v�V����(�Ȃ�)
		NULL,          // �����ɃR���p�C�����Ă���֐��𔲂���B
		&pBlobPS,      // �R���p�C�����ꂽ�o�C�g�E�R�[�h
		NULL,          // �G���[���b�Z�[�W�͕s�v
		NULL);         // �߂�l
	if (FAILED(hr))
		return DXTRACE_ERR("InitDirect3D D3DX11CompileShaderFromFile", hr);

	// �s�N�Z���E�V�F�[�_�̍쐬
	hr = GetDirectX::Device()->CreatePixelShader(
		pBlobPS->GetBufferPointer(), // �o�C�g�E�R�[�h�ւ̃|�C���^
		pBlobPS->GetBufferSize(),    // �o�C�g�E�R�[�h�̒���
		NULL,
		&vpPixelShader); // �s�N�Z���E�V�F�[�_���󂯎��ϐ�
	SAFE_RELEASE(pBlobPS);  // �o�C�g�E�R�[�h�����
	if (FAILED(hr))
		return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreatePixelShader", hr);
}
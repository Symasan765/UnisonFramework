/*=================================================
//								FILE : Shader.cpp
//		�t�@�C������ :
//		�V�F�[�_�̓ǂݍ��݂Ȃǂ��s���N���X���L�q����
//
//							HAL��� : ���{ �Y�V��
=================================================*/
#include "Shader.h"
#include "CompileOptions.h"
#include <WinBase.h>
#include <WinBase.h>
#include "ShaderMonitoring.h"

using namespace CompileOptions;
namespace {
	std::string DirectoryPath = "HLSL/";
}

/// <summary>
/// �V�F�[�_�t�@�C�������[�h���A�R���p�C������
/// </summary>
/// <param name="_fileName">�t�@�C�����B�t�H���_�܂ł̃p�X�͎����ŕt�^</param>
/// <param name="bitFlag"></param>
/// <param name="_GSflag"></param>
/// <returns></returns>
std::shared_ptr<ShaderData> SHADER::LoadShaderFile(std::string _fileName, const short bitFlag, bool _GSflag)
{
	//�܂����̃e�N�X�`�������f������ǂݍ��܂ꂽ�̂��ʏ��Ԃœǂ܂��̂��Ńp�X��ύX����
	std::string path = DirectoryPath + _fileName;

	//���̖��O��map�ɓo�^����Ă��邩�m�F����
	auto itr = m_Map.find(path);        // "xyz" ���ݒ肳��Ă��邩�H
	if (itr != m_Map.end()) {
		//�ݒ肳��Ă���ꍇ�̏���
		return m_Map[path];		//shared_ptr�̃R�s�[
	}
	//���ɂȂ����map�ɗ̈���m�ۂ��Ă���g���q�𔻕ʂ��Ċe�p�[�T�[���Ăяo���ăf�[�^���擾����
	else {
		//�ݒ肳��Ă��Ȃ��ꍇ�̏���
		m_Map[path] = std::make_shared<ShaderData>();

		//�����ɃV�F�[�_���[�h�֐�������
		LoadShader(path, bitFlag,&*m_Map[path], _GSflag);
		cShaderMonitoring::GetInstance().AddFileDate(path);
	}
	return m_Map[path];		//�R�s�[
}

/// <summary>
/// �V�F�[�_�̃����[�h�������s��
/// </summary>
/// <param name="path"></param>
/// <returns></returns>
HRESULT SHADER::ShaderReload(std::string path)
{
	auto itr = m_Map.find(path);
	if (itr != m_Map.end()) {
		//�ݒ肳��Ă���ꍇ�̏���
		bool GSFlag = m_Map[path]->release();
		return LoadShader(itr->first, 0, &*m_Map[path], GSFlag);
	}

	return S_OK;		//�Ȃ���Εʂɖ��Ȃ�
}

/// <summary>
/// �V�F�[�_�̃��[�h���s��
/// </summary>
/// <param name="_ShaderFileName">�t�@�C���p�X</param>
/// <param name="bitFlag">�ǂݍ��ރV�F�[�_�̎�ނ��r�b�g���Z�Ŏw�肷��</param>
/// <param name="_data">�i�[����|�C���^</param>
/// <param name="_GSflag">�W�I���g���V�F�[�_��ǂݍ��ރt���O</param>
/// <returns></returns>
HRESULT SHADER::LoadShader(std::string _ShaderFileName, const short bitFlag, ShaderData* _data, bool _GSflag)
{
	// ���_�V�F�[�_�̃R�[�h���R���p�C��===========================================
	ID3DBlob* pBlobVS = NULL;
	HRESULT hr = D3DX11CompileFromFile(
		_ShaderFileName.c_str(),  // �t�@�C����
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
		&_data->vpVertexShader); // ���_�V�F�[�_���󂯎��ϐ�
						   //	SAFE_RELEASE(pBlobVS);  // �o�C�g�E�R�[�h�����
	if (FAILED(hr)) {
		SAFE_RELEASE(pBlobVS);
		return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateVertexShader", hr);
	}

	//�����Ƀ��C�A�E�g�Ăяo��
	GetInputLayout(_data, pBlobVS, bitFlag);
	SAFE_RELEASE(pBlobVS);


	// �s�N�Z���E�V�F�[�_�̃R�[�h���R���p�C��
	ID3DBlob* pBlobPS = NULL;
	hr = D3DX11CompileFromFile(
		_ShaderFileName.c_str(),  // �t�@�C����
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
		&_data->vpPixelShader); // �s�N�Z���E�V�F�[�_���󂯎��ϐ�
	SAFE_RELEASE(pBlobPS);  // �o�C�g�E�R�[�h�����
	if (FAILED(hr))
		return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreatePixelShader", hr);

	if (_GSflag) {
		ID3DBlob* pBlobGS = NULL;
		hr = D3DX11CompileFromFile(
			_ShaderFileName.c_str(),  // �t�@�C����
			NULL,          // �}�N����`(�Ȃ�)
			NULL,          // �C���N���[�h�E�t�@�C����`(�Ȃ�)
			"GS",          // �uGS�֐��v���V�F�[�_������s�����
			"gs_5_0",      // �W�I���g���E�V�F�[�_
			g_flagCompile, // �R���p�C���E�I�v�V����
			0,             // �G�t�F�N�g�̃R���p�C���E�I�v�V����(�Ȃ�)
			NULL,          // �����ɃR���p�C�����Ă���֐��𔲂���B
			&pBlobGS,      // �R���p�C�����ꂽ�o�C�g�E�R�[�h
			NULL,          // �G���[���b�Z�[�W�͕s�v
			NULL);         // �߂�l
		if (FAILED(hr))
			return DXTRACE_ERR("InitDirect3D D3DX11CompileShaderFromFile", hr);

		// �W�I���g���E�V�F�[�_�̍쐬
		hr = GetDirectX::Device()->CreateGeometryShader(
			pBlobGS->GetBufferPointer(), // �o�C�g�E�R�[�h�ւ̃|�C���^
			pBlobGS->GetBufferSize(),    // �o�C�g�E�R�[�h�̒���
			NULL,
			&_data->vpGeometryShader); // �W�I���g���E�V�F�[�_���󂯎��ϐ�
		SAFE_RELEASE(pBlobGS);  // �o�C�g�E�R�[�h�����
		if (FAILED(hr))
			return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreatePixelShader", hr);
	}
	else {
		_data->vpGeometryShader = nullptr;
	}
}

//�͋Z�V���[�Y
HRESULT SHADER::GetInputLayout(ShaderData* _data, ID3DBlob* blob, const short bitFlag)
{
	//���_�A�@���AUV
	if (bitFlag == (inPOSITION | inNORMAL | inTEX_UV)) {
		// ���͗v�f================================================
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(DirectX::XMFLOAT4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::XMFLOAT4) + sizeof(DirectX::XMFLOAT4), D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		// ���̓��C�A�E�g�E�I�u�W�F�N�g�̍쐬
		HRESULT hr = GetDirectX::Device()->CreateInputLayout(
			layout,                            // ��`�̔z��
			_countof(layout),                  // ��`�̗v�f��
			blob->GetBufferPointer(),       // �o�C�g�E�R�[�h�ւ̃|�C���^
			blob->GetBufferSize(),          // �o�C�g�E�R�[�h�̃T�C�Y
			&_data->vpInputLayout);                  // �󂯎��ϐ��̃|�C���^

		if (FAILED(hr))
			return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateInputLayout", hr);
	}
	else if (bitFlag == (inPOSITION | inTEX_UV)) {
		// ���͗v�f================================================
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::XMFLOAT4), D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		// ���̓��C�A�E�g�E�I�u�W�F�N�g�̍쐬
		HRESULT hr = GetDirectX::Device()->CreateInputLayout(
			layout,                            // ��`�̔z��
			_countof(layout),                  // ��`�̗v�f��
			blob->GetBufferPointer(),       // �o�C�g�E�R�[�h�ւ̃|�C���^
			blob->GetBufferSize(),          // �o�C�g�E�R�[�h�̃T�C�Y
			&_data->vpInputLayout);                  // �󂯎��ϐ��̃|�C���^

		if (FAILED(hr))
			return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateInputLayout", hr);
	}
	else if (bitFlag == (inPOSITION | inNORMAL | inTEX_UV | inBone | inWeight)) {
		// ���͗v�f================================================
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(DirectX::XMFLOAT4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::XMFLOAT4) + sizeof(DirectX::XMFLOAT4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONE_INDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, sizeof(DirectX::XMFLOAT4) + sizeof(DirectX::XMFLOAT4) + sizeof(DirectX::XMFLOAT2), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONE_WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(DirectX::XMFLOAT4) + sizeof(DirectX::XMFLOAT4) + sizeof(DirectX::XMFLOAT2) + sizeof(DirectX::XMUINT4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// ���̓��C�A�E�g�E�I�u�W�F�N�g�̍쐬
		HRESULT hr = GetDirectX::Device()->CreateInputLayout(
			layout,                            // ��`�̔z��
			_countof(layout),                  // ��`�̗v�f��
			blob->GetBufferPointer(),       // �o�C�g�E�R�[�h�ւ̃|�C���^
			blob->GetBufferSize(),          // �o�C�g�E�R�[�h�̃T�C�Y
			&_data->vpInputLayout);                  // �󂯎��ϐ��̃|�C���^

		if (FAILED(hr))
			return DXTRACE_ERR("InitDirect3D g_pD3DDevice->CreateInputLayout", hr);
	}
}

void SHADER::CheckDelete()
{
	for (auto itr = m_Map.begin(); itr != m_Map.end(); ++itr) {
		//map�����|�C���^��ێ����Ă��Ȃ�
		if (itr->second.use_count() == 1) {
			m_Map.erase(itr->first);		//�f�[�^�폜
			itr = m_Map.begin();
			if (itr == m_Map.end())
				return;
		}
	}
}

SHADER::SHADER() {
	ConstWorldMtx = nullptr;
	ConstLightVec = nullptr;
	ConstMaterial = nullptr;
}

SHADER::~SHADER() {
	SAFE_RELEASE(ConstWorldMtx);
	SAFE_RELEASE(ConstLightVec);
	SAFE_RELEASE(ConstMaterial);
}
